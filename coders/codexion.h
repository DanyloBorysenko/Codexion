/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 15:39:00 by danborys          #+#    #+#             */
/*   Updated: 2026/04/25 20:12:45 by danborys         ###   ########.fr       */
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

#ifndef HEAP_SIZE
# define HEAP_SIZE 2
#endif

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
}				coder_t;

typedef struct monitor_s
{
	pthread_t	thread_id;
	t_config	*config;
	coder_t		*coders;
	dongle_t	*dongles;
	simul_t		*simul;
	int			finished;
	int			burned_out;
	pthread_mutex_t	lock;
	pthread_cond_t	cond;
} 				monitor_t;

typedef struct shared_arg_s
{
	t_config	*conf;
	simul_t		*sim;
	dongle_t	*dngls;
}				shared_arg_t;

t_config		*parse_arg(int argc, char **argv, char **possible_schedul_val);
void 			start_to_work(t_config *cfg, simul_t *simul);
simul_t			*init_simul(void);
void			destroy_simul(simul_t *sim);
void			log_event(simul_t	*sim, int id, char *msg, long long time);
monitor_t		*init_monitor(
	t_config *config,
	simul_t *simul,
	coder_t *coders,
	dongle_t *dongles);
// void destroy_monitor(monitor_t *mon);
coder_t			*init_coders(shared_arg_t init_arg);
void			destroy_coders(coder_t *coders, int count);
dongle_t    	*init_dongles(int coders_count, int cooldown, char *sched);
void 			destroy_dongles(dongle_t *dongles, int coders_count);
heap_t			*init_heap(int count, char *sched);
void			heap_insert(heap_t *heap, req_t req);
req_t			heap_extract(heap_t *heap, int index);
void 			destroy_heap(heap_t *heap);
long long 		get_current_time(void);
struct 			timespec get_abs_time(long long wake_up_time);

#endif