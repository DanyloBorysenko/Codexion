/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 15:39:00 by danborys          #+#    #+#             */
/*   Updated: 2026/04/28 16:08:22 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <string.h>
# include <limits.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/time.h>
# include <pthread.h>

# define SCHED_COUNT 2
# define ARG_COUNT 9
# define FIFO_SCHED_IND 0
# define EDF_SCHED_IND 1

# define HEAP_SIZE 2

#define ARGV_CODERS 1
#define ARGV_BURNOUT 2
#define ARGV_COMPILE 3
#define ARGV_DEBUG 4
#define ARGV_REFACTOR 5
#define ARGV_COMP_REQ 6
#define ARGV_COOLDOWN 7
#define ARGV_SCHED 8

typedef struct s_config
{
	int		num_of_cod;
	int		time_to_burnout;
	int		time_to_compile;
	int		time_to_debug;
	int		time_to_refactor;
	int		num_of_comp_req;
	int		dongle_cooldown;
	char	*scheduler;
}				t_config;

typedef struct simul_s
{
	int				finished_coders;
	int				is_finished;
	long long		start;
	pthread_mutex_t	sim_lock;
	pthread_mutex_t	print_lock;
	pthread_cond_t	cond;
}				simul_t;

typedef struct req_s
{
	int			coder_id;
	long long 	arr_time;
	long long 	deadline;
}				req_t;

typedef struct heap_s
{
	req_t			*reqs;
	int				size;
	int				capacity;
	char    		*sched;
}				heap_t;

typedef struct dongle_s
{
	int				num;
	int				in_use;
	long long		release;
	int				cooldown;
	pthread_mutex_t	lock;
	pthread_cond_t	cond;
	heap_t			*heap;
}				dongle_t;

typedef struct coder_s
{
	int				id;
	pthread_t		thread_id;
	dongle_t		*left_dng;
	dongle_t		*right_dng;
	int				time_to_burnout;
	int				time_to_compile;
	int				time_to_debug;
	int				time_to_refactor;
	int				num_of_comp_req;
	int				compiles_done;
	pthread_mutex_t	coder_lock;
	long long		last_compile_time;
	simul_t			*simul;
}				coder_t;

typedef struct monitor_s
{
	pthread_t	thread_id;
	int			cod_count;
	coder_t		*coders;
	dongle_t	*dongles;
	simul_t		*simul;
} 				monitor_t;

typedef struct s_components
{
	simul_t		*sim;
	dongle_t	*dongles;
	coder_t		*coders;
	monitor_t	*mon;
}	t_components;

void			parse_arg(int argc, char **argv, char **sched_vals, t_config *conf);
void 			start_simul(t_config *cfg);
simul_t			*init_simul(void);
void			destroy_simul(simul_t *sim);
int				is_simul_finished(simul_t *sim);
void			log_event(simul_t	*sim, int id, char *msg, long long time);
monitor_t		*init_monitor(
	int coders_count,
	simul_t *simul,
	coder_t *coders,
	dongle_t *dongles);
void			wake_up_all(int count, dongle_t *don, simul_t *sim);
coder_t			*init_coders(t_config *conf, simul_t *sim, dongle_t *don);
void			destroy_coders(coder_t *coders, int count);
dongle_t		*init_dongles(t_config *conf);
void 			destroy_dongles(dongle_t *dongles, int coders_count);
heap_t			*init_heap(int count, char *sched);
void			heap_insert(heap_t *heap, req_t req);
req_t			heap_extract(heap_t *heap, int index);
void 			destroy_heap(heap_t *heap);
long long 		get_cur_time(void);
struct 			timespec get_abs_time(long long wake_up_time);

void			*mon_rout(void *arg);
void			*coder_rout(void *arg);

#endif