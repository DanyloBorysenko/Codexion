/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 15:39:00 by danborys          #+#    #+#             */
/*   Updated: 2026/04/13 18:01:09 by danborys         ###   ########.fr       */
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

typedef struct locks_s
{
	pthread_mutex_t	simul_state_lock;
	pthread_mutex_t	print_lock;
}				locks_t;

typedef struct simul_s
{
	int	finished_coders;
	int	is_simul_alive;
	long long start;
}				simul_t;

typedef struct dongle_s
{
	int				num;
	int				is_avail;
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
	locks_t			*locks;
	pthread_mutex_t	coder_lock;
	long long		last_compile_time;
	long long		burn_out_time;
	simul_t			*simul;
	heap_t			*heap;
}				coder_t;

typedef struct monitor_arg_s
{
	t_config		*config;
	coder_t			*coders;
	pthread_mutex_t *simul_lock;
	simul_t	*simul;
}				monitor_arg_t;

t_config		*parse_arg(int argc, char **argv, char **possible_schedul_val);
long long		get_current_time(struct timeval* tv);
void 			start_to_work(t_config *cfg, simul_t *simul);
void			destroy_locks(locks_t *locks);
locks_t			*create_locks(void);
simul_t	*init_simul(void);
void			log_event(pthread_mutex_t *mut, int id, char *msg, int time);
monitor_arg_t	*init_monitor(t_config *config,locks_t *locks,
	simul_t *simul_state,
	coder_t *coders);
coder_t	*init_coders(t_config *conf, locks_t *locks, simul_t *sim, dongle_t *dngls, heap_t *heap);
void			destroy_coders(coder_t *coders, int count);
dongle_t		*init_dongles(int coders_count);
void 			destroy_dongles(dongle_t *dongles, int coders_count);
heap_t			*init_heap(t_config *config);
void			heap_insert(heap_t *heap, req_t req);
void 			destroy_heap(heap_t *heap);

#endif