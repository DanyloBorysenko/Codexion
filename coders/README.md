*This project has been created as part of the 42 curriculum by \<danborys\>*

# Codexion

## Description

Codexion is a concurrency simulation written in C, inspired by the classic Dining
Philosophers problem. It models a group of coders sharing a circular co-working hub,
where each coder must acquire two adjacent USB dongles simultaneously in order to
compile their quantum code.

Each coder runs as an independent POSIX thread and cycles through three phases:
**compiling** (requires both dongles), **debugging**, and **refactoring**. After
refactoring, the coder immediately attempts to acquire dongles again. If a coder fails
to start compiling within `time_to_burnout` milliseconds since their last compile (or
since the start of the simulation), they burn out and the simulation ends.

The simulation also stops when every coder has compiled at least
`number_of_compiles_required` times.

A key feature of Codexion is its **fair arbitration system**: each dongle maintains a
priority queue (min-heap) of pending requests. When multiple coders compete for the
same dongle, access is granted according to one of two scheduling policies chosen at
launch:

- **FIFO** — the coder whose request arrived earliest gets priority.
- **EDF** (Earliest Deadline First) — the coder closest to burning out gets priority.

Dongles also have a configurable **cooldown period**: after being released, a dongle
cannot be acquired again until the cooldown has elapsed.

## Instructions

### Compilation

```bash
make
```

This compiles the project with `-Wall -Wextra -Werror -pthread` and produces the
`codexion` executable.

### Usage

```bash
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

All arguments are mandatory. All time values are in milliseconds.

| Argument | Description |
|---|---|
| `number_of_coders` | Number of coders (and dongles) in the simulation |
| `time_to_burnout` | Max time a coder can go without starting a compile |
| `time_to_compile` | Time spent compiling (both dongles held) |
| `time_to_debug` | Time spent debugging after each compile |
| `time_to_refactor` | Time spent refactoring after debugging |
| `number_of_compiles_required` | Simulation ends when all coders reach this count |
| `dongle_cooldown` | Time a dongle is unavailable after being released |
| `scheduler` | Arbitration policy: `fifo` or `edf` |

### Example

```bash
./codexion 5 800 200 200 200 3 50 edf
```

### Expected output format

Each state change is printed as:

```
timestamp_in_ms X has taken a dongle
timestamp_in_ms X has taken a dongle
timestamp_in_ms X is compiling
timestamp_in_ms X is debugging
timestamp_in_ms X is refactoring
timestamp_in_ms X burned out
```

Where `timestamp_in_ms` is milliseconds elapsed since simulation start and `X` is the
coder number.

### Input validation

The program rejects: negative numbers, non-integer values, and any scheduler value
other than `fifo` or `edf`.

## Blocking cases handled

### Deadlock prevention

Deadlock requires four conditions to hold simultaneously (Coffman's conditions):
**mutual exclusion**, **hold and wait**, **no preemption**, and **circular wait**.
Codexion eliminates both **hold and wait** and **circular wait** through its
all-or-nothing acquisition strategy: a coder only takes dongles when both are
simultaneously free and the coder is first in queue on both. The check and the take
happen atomically under a single shared mutex (`sched_lock`), so no coder ever holds
one dongle while waiting for the other. With hold and wait eliminated, circular wait
cannot form, making deadlock structurally impossible.

### Starvation prevention

Each dongle maintains a min-heap of pending requests ordered by the active scheduling
policy. Under **FIFO**, requests are served in arrival order, so no coder can be
indefinitely skipped. Under **EDF**, the coder with the earliest burnout deadline is
served first, ensuring that coders closest to burning out are prioritized. In both
cases, every request is eventually served as long as simulation parameters are feasible.

### Cooldown handling

After a coder releases both dongles, each dongle enters a cooldown period of
`dongle_cooldown` milliseconds during which it cannot be acquired. The scheduler
tracks each dongle's `release` timestamp and checks it atomically alongside the
in-use flag. If both dongles are free but still cooling down, the coder sleeps with
`pthread_cond_timedwait` until the later of the two cooldown expiries, avoiding
busy-waiting.

### Precise burnout detection

A dedicated monitor thread polls all coders every 1 ms, comparing the current time
against each coder's `last_compile_time`. If the elapsed time exceeds
`time_to_burnout`, the monitor immediately sets `is_finished`, broadcasts to all
waiting threads, and prints the burnout log. This guarantees the burnout message
appears within a few milliseconds of the actual deadline.

### Log serialization

All log output is protected by a dedicated `print_lock` mutex. Before printing,
the logger also checks `is_finished` under `sim->lock` to suppress stale messages —
except for `burned out`, which is always allowed through. This ensures no two messages
interleave on a single line and that the burnout message is never suppressed.

## Thread synchronization mechanisms

### Primitives used

| Primitive | Instance | Protects |
|---|---|---|
| `pthread_mutex_t` | `sim->lock` | `is_finished`, `finished_coders` |
| `pthread_mutex_t` | `sim->print_lock` | stdout output |
| `pthread_mutex_t` | `sim->sched_lock` | all dongle state: `in_use`, `release`, heaps |
| `pthread_mutex_t` | `coder->lock` | `last_compile_time` |
| `pthread_cond_t` | `sim->cond` | waking coders during compile/debug/refactor |
| `pthread_cond_t` | `sim->sched_cond` | waking coders waiting for dongles |

### Dongle acquisition

All dongle state — heap contents, `in_use` flag, `release` timestamp — is protected
by a single shared mutex `sched_lock`. This makes every check-and-take atomic: no
other coder can observe or modify any dongle between the state check and the
acquisition:

```c
pthread_mutex_lock(&sim->sched_lock);
// insert request into both heaps
insert_req(coder);
pthread_mutex_unlock(&sim->sched_lock);

// ...later in take_dongles:
pthread_mutex_lock(&sim->sched_lock);
state = get_state(coder);   // read both dongles atomically
if (state == 2)
    return (do_take(coder)); // mark both in_use, extract from heaps
// otherwise sleep on sched_cond, releasing sched_lock atomically
```

If the coder is not first in both queues or either dongle is unavailable, it sleeps on
`sched_cond`, which atomically releases `sched_lock` and re-acquires it on wakeup.

### Waking waiting coders

When dongles are released, `release_dongles` updates both dongle states and broadcasts
on `sched_cond` under `sched_lock`. All coders sleeping in `take_dongles` wake up,
re-evaluate `get_state`, and only the one that is first in both heaps with both dongles
free will proceed — all others return to sleep:

```c
pthread_mutex_lock(&sim->sched_lock);
left->in_use = 0;
left->release = get_cur_time() + left->cooldown;
right->in_use = 0;
right->release = get_cur_time() + right->cooldown;
pthread_cond_broadcast(&sim->sched_cond);
pthread_mutex_unlock(&sim->sched_lock);
```

### Simulation shutdown

When the monitor detects burnout or all coders finish, `wake_up_all` is called. It
sets `is_finished = 1` under `sim->lock`, then broadcasts on both `sim->cond` and
`sim->sched_cond` to wake every thread regardless of where it is sleeping:

```c
pthread_mutex_lock(&sim->lock);
sim->is_finished = 1;
pthread_cond_broadcast(&sim->cond);
pthread_mutex_unlock(&sim->lock);
pthread_mutex_lock(&sim->sched_lock);
pthread_cond_broadcast(&sim->sched_cond);
pthread_mutex_unlock(&sim->sched_lock);
```

Coders in `work()` check `is_finished` on every wakeup and exit immediately if set.
Coders in `take_dongles` call `is_simul_finished` at the top of their loop.

### Protecting last_compile_time

The monitor reads `last_compile_time` to detect burnout while the coder writes it at
the start of each compile. Access from both sides is protected by `coder->lock`:

```c
// coder thread (compile):
pthread_mutex_lock(&coder->lock);
coder->last_compile_time = current_time;
pthread_mutex_unlock(&coder->lock);

// monitor thread (burnout check):
pthread_mutex_lock(&cods[i].lock);
last = cods[i].last_compile_time;
pthread_mutex_unlock(&cods[i].lock);
```

### Race condition prevention summary

| Scenario | Prevention |
|---|---|
| Two coders taking the same dongle | Atomic check-and-take under `sched_lock` |
| Burnout log after simulation ends | `is_finished` check inside `log_event` under `print_lock` |
| Stale `last_compile_time` read | `coder->lock` on both read and write |
| Spurious wakeup in `take_dongles` | Condition re-evaluated at top of `while` loop on every wakeup |
| Spurious wakeup in `work()` | `while (!is_finished)` loop with time check on every wakeup |

## Resources

- [Thread Management Functions in C — GeeksforGeeks](https://www.geeksforgeeks.org/c/thread-functions-in-c-c/)
- [POSIX Threads in OS — GeeksforGeeks](https://www.geeksforgeeks.org/operating-systems/posix-threads-in-os/)
- [pthread manual page — Linux man-pages](https://man7.org/linux/man-pages/man7/pthreads.7.html)

### AI usage

Claude was used during this project for drafting and structuring this README.md
ChatGPT was used for:
- Reviewing synchronization logic and identifying potential race conditions
- Discussing scheduling policy trade-offs (FIFO vs EDF)
