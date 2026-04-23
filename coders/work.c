/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   work.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 14:14:20 by danborys          #+#    #+#             */
/*   Updated: 2026/04/23 10:56:57 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void wake_up_all(int count, coder_t	*coders, scheduler_t *sched)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_mutex_lock(&coders[i].coder_lock);
		coders[i].alive = 0;
		pthread_cond_signal(&coders[i].cond);
		pthread_mutex_unlock(&coders[i].coder_lock);
		i++;
	}
	pthread_mutex_lock(&sched->lock);
	sched->alive = 0;
	pthread_cond_signal(&sched->cond);
	pthread_mutex_unlock(&sched->lock);
}

void *monitor_routine(void *arg)
{
	monitor_t *mon;
	long long now;
	long long last;
	int stop;
	int i;

	mon = (monitor_t*)arg;
	stop = 0;
	while (stop == 0)
	{
		pthread_mutex_lock(&mon->simul->sim_lock);
		if (mon->simul->finished_coders == mon->config->number_of_coders)
		{
			pthread_mutex_unlock(&mon->simul->sim_lock);
			wake_up_all(mon->config->number_of_coders, mon->coders, mon->sched);
			printf("Finished = all\n");
			break;
		}
		pthread_mutex_unlock(&mon->simul->sim_lock);
		i = 0;
		while (i < mon->config->number_of_coders)
		{

			now = get_current_time();
			pthread_mutex_lock(&mon->coders[i].coder_lock);
			last = mon->coders[i].last_compile_time;
			pthread_mutex_unlock(&mon->coders[i].coder_lock);
			if (now - last > mon->config->time_to_burnout)
			{
				stop = 1;
				log_event(mon->simul, (mon->coders)[i].id, "burned out", now);
				wake_up_all(mon->config->number_of_coders, mon->coders, mon->sched);
				break;
			}
			i++;
		}
		if (stop == 1)
			break;
		usleep(1000);
	}
	return (NULL);
}

int work(coder_t *coder, long long end_time,struct timespec *ts)
{
	long long	current_time;

	pthread_mutex_lock(&coder->coder_lock);
	while (coder->alive)
	{
		current_time = get_current_time();
		if (current_time >= end_time)
			break;
		pthread_cond_timedwait(&coder->cond, &coder->coder_lock, ts);
	}
	if (!coder->alive)
	{
		pthread_mutex_unlock(&coder->coder_lock);
		return (0);
	}
	pthread_mutex_unlock(&coder->coder_lock);
	return (1);
}

int refact(coder_t *coder)
{
	struct timespec ts;
	long long current_time;
	long long end_time;

	current_time = get_current_time();
	end_time = current_time + coder->config->time_to_refactor;
	ts = get_abs_time(end_time);
	log_event(coder->simul, coder->id, "is refactoring", current_time);
	return (work(coder, end_time, &ts));
}

int debug(coder_t *coder)
{
	struct timespec ts;
	long long current_time;
	long long end_time;

	current_time = get_current_time();
	end_time = current_time + coder->config->time_to_debug;
	ts = get_abs_time(end_time);
	log_event(coder->simul, coder->id, "is debugging", current_time);
	return (work(coder, end_time, &ts));
}

int compile(coder_t *coder)
{
	struct timespec ts;
	long long current_time;
	long long end_time;

	current_time = get_current_time();
	end_time = current_time + coder->config->time_to_compile;
	ts = get_abs_time(end_time);
	pthread_mutex_lock(&coder->coder_lock);
	coder->last_compile_time = current_time;
	pthread_mutex_unlock(&coder->coder_lock);
	log_event(coder->simul, coder->id, "is compiling", current_time);
	pthread_mutex_lock(&coder->left_dng->lock);
	coder->left_dng->owner_id = 0;
	coder->left_dng->release_time = current_time + coder->config->dongle_cooldown;
	pthread_mutex_unlock(&coder->left_dng->lock);
	pthread_mutex_lock(&coder->right_dng->lock);
	coder->right_dng->owner_id = 0;
	coder->right_dng->release_time = current_time + coder->config->dongle_cooldown;
	pthread_mutex_unlock(&coder->right_dng->lock);
	if (!work(coder, end_time, &ts))
		return (0);
	coder->compiles_done++;
	if (coder->compiles_done == coder->config->number_of_compiles_required)
	{
		pthread_mutex_lock(&coder->simul->sim_lock);
		coder->simul->finished_coders = coder->simul->finished_coders + 1;
		pthread_mutex_unlock(&coder->simul->sim_lock);
	}
	return (1);
}

void *coders_routine(void *arg)
{
	coder_t *coder;
	coder = (coder_t *)arg;
	req_t request;
	long long now;

	while (1)
	{
		now = get_current_time();
		request.coder = coder;
		request.arr_time = now;
		request.deadline = coder->last_compile_time + coder->config->time_to_burnout;
		heap_insert(coder->heap, request);
		pthread_mutex_lock(&coder->sched->lock);
		coder->sched->called = 1;
		pthread_cond_signal(&coder->sched->cond);
		pthread_mutex_unlock(&coder->sched->lock);
		pthread_mutex_lock(&coder->coder_lock);
		while (coder->perm == 0 && coder->alive)
		{
			pthread_mutex_lock(&coder->simul->print_lock);
			printf("coder id %d goes sleep\n", coder->id);
			pthread_mutex_unlock(&coder->simul->print_lock);
			pthread_cond_wait(&coder->cond, &coder->coder_lock);
		}
		if (!coder->alive)
		{
			pthread_mutex_unlock(&coder->coder_lock);
			break;
		}
		coder->perm = 0;
		pthread_mutex_unlock(&coder->coder_lock);
		if (!compile(coder))
			break;
		if (!debug(coder))
			break;
		if (!refact(coder))
			break;
	}
	return (NULL);
}

int	check_dongles(dongle_t *d1, dongle_t *d2)
{
	long long now;
	int			res;

	if (d1 == d2)
		return (-1);
	pthread_mutex_lock(&d1->lock);
	pthread_mutex_lock(&d2->lock);
	if (d1->owner_id == 0 && d2->owner_id == 0)
	{
		now = get_current_time();
		res = now >= d1->release_time && now >= d2->release_time;
		pthread_mutex_unlock(&d1->lock);
		pthread_mutex_unlock(&d2->lock);
		return (res);
	}
	pthread_mutex_unlock(&d1->lock);
	pthread_mutex_unlock(&d2->lock);
	return (0);
}

void *sched_routine(void *arg)
{
	scheduler_t		*sched;
	coder_t			*coder;
	int				is_dongl_ok;
	long long		left_end;
	long long		right_end;
	long long		wake_up;
	long long		earliest_wake_up;
	struct timespec	wake_up_abs;
	int				i;

	sched = (scheduler_t *)arg;
	while (1)
	{
		pthread_mutex_lock(&sched->lock);
		while (sched->alive && !sched->called)
			pthread_cond_wait(&sched->cond, &sched->lock);
		if (!sched->alive)
		{
			pthread_mutex_unlock(&sched->lock);
			break;
		}
		sched->called = 0;
		pthread_mutex_unlock(&sched->lock);
		pthread_mutex_lock(&sched->heap->lock);
		i = 0;
		earliest_wake_up = get_current_time() + 1000;
		while (i < sched->heap->size)
		{
			coder = sched->heap->reqs[i].coder;
			is_dongl_ok = check_dongles(coder->left_dng, coder->right_dng);
			if (is_dongl_ok == 1)
			{
				heap_extract(sched->heap, i);
				pthread_mutex_lock(&coder->left_dng->lock);
				coder->left_dng->owner_id = coder->id;
				pthread_mutex_unlock(&coder->left_dng->lock);
				pthread_mutex_lock(&coder->right_dng->lock);
				coder->right_dng->owner_id = coder->id;
				pthread_mutex_unlock(&coder->right_dng->lock);
				pthread_mutex_lock(&coder->coder_lock);
				coder->perm = 1;
				pthread_cond_signal(&coder->cond);
				pthread_mutex_unlock(&coder->coder_lock);
				continue;
			}
			else
			{
				pthread_mutex_lock(&coder->left_dng->lock);
				pthread_mutex_lock(&coder->right_dng->lock);
				left_end = coder->left_dng->release_time;
				right_end = coder->right_dng->release_time;
				pthread_mutex_unlock(&coder->left_dng->lock);
				pthread_mutex_unlock(&coder->right_dng->lock);
				if (left_end > right_end)
					wake_up = left_end;
				else
					wake_up = right_end;
				if (wake_up < earliest_wake_up)
					earliest_wake_up = wake_up;
				i++;
			}
		}
		pthread_mutex_unlock(&sched->heap->lock);
		wake_up_abs = get_abs_time(earliest_wake_up);
		pthread_mutex_lock(&sched->lock);
		if (!sched->called)
			pthread_cond_timedwait(&sched->cond, &sched->lock, &wake_up_abs);
		sched->called = 1;
		pthread_mutex_unlock(&sched->lock);
	}
	return (NULL);
}

void *sched_routine(void *arg)
{
	scheduler_t		*sched;
	coder_t			*coder;
	int				is_dongl_ok;
	long long		left_end;
	long long		right_end;
	long long		wake_up;
	struct timespec	wake_up_abs;

	sched = (scheduler_t *)arg;
	while (1)
	{
		pthread_mutex_lock(&sched->lock);
		while (sched->alive && !sched->called)
			pthread_cond_wait(&sched->cond, &sched->lock);
		if (!sched->alive)
		{
			pthread_mutex_unlock(&sched->lock);
			break;
		}
		sched->called = 0;
		pthread_mutex_unlock(&sched->lock);

		pthread_mutex_lock(&sched->heap->lock);
		while (sched->heap->size > 0)
		{
			coder = sched->heap->reqs[0].coder;
			is_dongl_ok = check_dongles(coder->left_dng, coder->right_dng);
			if (is_dongl_ok == 1)
			{
				heap_extract(sched->heap, 0);
				coder->left_dng->owner_id = coder->id;
				coder->right_dng->owner_id = coder->id;
				pthread_mutex_lock(&coder->coder_lock);
				coder->perm = 1;
				pthread_cond_signal(&coder->cond);
				pthread_mutex_unlock(&coder->coder_lock);
				// reqs[0] is now the next coder, loop continues
			}
			else
			{
				// reqs[0] is blocked — compute when to wake up
				left_end = coder->left_dng->release_time;
				right_end = coder->right_dng->release_time;
				wake_up = (left_end > right_end) ? left_end : right_end;
				wake_up_abs = get_abs_time(wake_up);
				pthread_mutex_unlock(&sched->heap->lock);

				pthread_mutex_lock(&sched->lock);
				// a new request may have arrived while we worked — only sleep if not called
				if (!sched->called)
					pthread_cond_timedwait(&sched->cond, &sched->lock, &wake_up_abs);
				sched->called = 0;
				if (!sched->alive)
				{
					pthread_mutex_unlock(&sched->lock);
					return (NULL);
				}
				pthread_mutex_unlock(&sched->lock);
				pthread_mutex_lock(&sched->heap->lock);
				// re-check reqs[0] from the top after waking
			}
		}
		pthread_mutex_unlock(&sched->heap->lock);
	}
	return (NULL);
}

void start_to_work(t_config *config, simul_t *simul)
{
	heap_t *heap;
	dongle_t *dongles;
	coder_t *coders;
	shared_arg_t shared_arg;
	scheduler_t	*sched;
	monitor_t *mon;
	int i;

	heap = init_heap(config);
	if (!heap)
		return;
	dongles = init_dongles(config->number_of_coders);
	sched = init_sched(heap);
	shared_arg.conf = config;
	shared_arg.dngls = dongles;
	shared_arg.heap = heap;
	shared_arg.sim = simul;
	shared_arg.sched = sched;
	coders = init_coders(shared_arg);
	mon = init_monitor(config, simul, coders, sched);
	i = 0;
	while (i < config->number_of_coders)
	{
		pthread_create(&coders[i].thread_id, NULL, coders_routine, &coders[i]);
		i++;
	}
	pthread_create(&mon->thread_id, NULL, monitor_routine, mon);
	pthread_create(&sched->thread_id, NULL, sched_routine, sched);
	pthread_join(mon->thread_id, NULL);
	pthread_join(sched->thread_id, NULL);
	i = 0;
	while (i < config->number_of_coders)
	{
		pthread_join(coders[i].thread_id, NULL);
		i++;
	}
	destroy_coders(coders, config->number_of_coders);
	destroy_dongles(dongles, config->number_of_coders);
	free(mon);
	destroy_heap(heap);
	destroy_sched(sched);
}
