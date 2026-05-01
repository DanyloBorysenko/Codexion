/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 15:39:00 by danborys          #+#    #+#             */
/*   Updated: 2026/05/01 16:26:00 by danborys         ###   ########.fr       */
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

# define ARGV_CODERS 1
# define ARGV_BURNOUT 2
# define ARGV_COMPILE 3
# define ARGV_DEBUG 4
# define ARGV_REFACTOR 5
# define ARGV_COMP_REQ 6
# define ARGV_COOLDOWN 7
# define ARGV_SCHED 8

typedef struct s_dongle	t_dongle;

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

typedef struct s_simul
{
	int				finished_coders;
	int				is_finished;
	long long		start;
	pthread_mutex_t	lock;
	pthread_mutex_t	print_lock;
	pthread_mutex_t	sched_lock;
	pthread_cond_t	sched_cond;
	pthread_cond_t	cond;
}				t_simul;

typedef struct s_coder
{
	int				id;
	pthread_t		thread_id;
	t_dongle		*left_dng;
	t_dongle		*right_dng;
	int				time_to_burnout;
	int				time_to_compile;
	int				time_to_debug;
	int				time_to_refactor;
	int				num_of_comp_req;
	int				compiles_done;
	pthread_mutex_t	lock;
	long long		last_compile_time;
	t_simul			*simul;
}				t_coder;

typedef struct s_req
{
	int			cod_id;
	t_coder		*coder;
	long long	arr_t;
	long long	deadl;
}				t_req;

typedef struct s_heap
{
	t_req	*reqs;
	int		size;
	int		capacity;
	char	*sched;
}				t_heap;

typedef struct s_dongle
{
	int				num;
	int				in_use;
	long long		release;
	int				cooldown;
	pthread_mutex_t	lock;
	t_heap			*heap;
}				t_dongle;

typedef struct s_monitor
{
	pthread_t	thread_id;
	int			cod_count;
	t_coder		*coders;
	t_simul		*simul;
}				t_monitor;

typedef struct s_components
{
	t_simul		*sim;
	t_dongle	*dongles;
	t_coder		*coders;
	t_monitor	*mon;
}	t_components;

void			parse_arg(int argc, char **argv, char **scheds, t_config *conf);
void			start_simul(t_config *cfg);
t_simul			*init_simul(void);
void			destroy_simul(t_simul *sim);
int				is_simul_finished(t_simul *sim);
void			log_event(t_simul	*sim, int id, char *msg, long long time);
t_monitor		*init_monitor(
					int coders_count,
					t_simul *simul,
					t_coder *coders);
void			wake_up_all(t_simul *sim);
t_coder			*init_coders(t_config *conf, t_simul *sim, t_dongle *don);
void			destroy_coders(t_coder *coders, int count);
t_dongle		*init_dongles(t_config *conf);
void			destroy_dongles(t_dongle *dongles, int coders_count);
t_heap			*init_heap(int count, char *sched);
void			heap_insert(t_heap *heap, t_req req);
t_req			heap_extract(t_heap *heap, int index);
void			destroy_heap(t_heap *heap);
void			print_heap(t_heap *heap, int don_id);
long long		get_cur_time(void);
struct timespec	get_abs_time(long long wake_up_time);

void			*mon_rout(void *arg);
void			*coder_rout(void *arg);
int				take_dongles(t_dongle *d1, t_dongle *d2, t_coder *coder);

#endif