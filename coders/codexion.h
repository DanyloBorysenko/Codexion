/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 15:39:00 by danborys          #+#    #+#             */
/*   Updated: 2026/04/18 00:17:37 by danborys         ###   ########.fr       */
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

typedef struct s_config
{
	int		number_of_coders;
	int		time_to_burnout;
	int		time_to_compile;
	int		time_to_debug;
	int		time_to_refactor;
	int		number_of_compiles_required;
	int		dongle_cooldown;
	char	*scheduler;
}				t_config;

typedef struct simul_s
{
	int	finished_coders;
	long long start;
	pthread_mutex_t	sim_lock;
	pthread_mutex_t	print_lock;
}				simul_t;

typedef struct dongle_s
{
	int				num;
	pthread_mutex_t	lock;
}				dongle_t;

typedef struct coder_s coder_t;

typedef struct req_s
{
	coder_t		*coder;
	long long 	arr_time;
	long long 	deadline;
}				req_t;

typedef struct heap_s
{
	req_t			*reqs;
	int				size;
	int				capacity;
	char    		*scheduler;
	pthread_mutex_t	lock;
}				heap_t;

typedef struct coder_s
{
	int				id;
	pthread_t		thread_id;
	t_config		*config;
	dongle_t		*left_dng;
	dongle_t		*right_dng;
	int				compiles_done;
	pthread_mutex_t	coder_lock;
	pthread_cond_t	cond;
	int				perm;
	int				alive;
	long long		last_compile_time;
	simul_t			*simul;
	heap_t			*heap;
}				coder_t;

typedef struct scheduler_s
{
	pthread_t		thread_id;
	int				alive;
	pthread_mutex_t	lock;
	pthread_cond_t	cond;
	heap_t			*heap;
} 				scheduler_t;

typedef struct monitor_s
{
	pthread_t	thread_id;
	t_config	*config;
	coder_t		*coders;
	scheduler_t	*sched;
	simul_t		*simul;
} 				monitor_t;

typedef struct shared_arg_s
{
	t_config	*conf;
	simul_t		*sim;
	dongle_t	*dngls;
	heap_t		*heap;
}				shared_arg_t;

t_config		*parse_arg(int argc, char **argv, char **possible_schedul_val);
void 			start_to_work(t_config *cfg, simul_t *simul);
simul_t			*init_simul(void);
void			destroy_simul(simul_t *sim);
void			log_event(simul_t	*sim, int id, char *msg, long long time);
monitor_t		*init_monitor(t_config *config, simul_t *simul,
	coder_t *coders,
	scheduler_t	*sched);
coder_t			*init_coders(shared_arg_t init_arg);
void			destroy_coders(coder_t *coders, int count);
scheduler_t		*init_sched(heap_t	*heap);
void			destroy_sched(scheduler_t *sched);
dongle_t		*init_dongles(int coders_count);
void 			destroy_dongles(dongle_t *dongles, int coders_count);
heap_t			*init_heap(t_config *config);
void			heap_insert(heap_t *heap, req_t req);
void 			destroy_heap(heap_t *heap);
long long 		get_current_time(void);
struct 			timespec get_abs_time(long long wake_up_time);

#endif