/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 15:39:00 by danborys          #+#    #+#             */
/*   Updated: 2026/04/02 11:09:37 by danborys         ###   ########.fr       */
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
	long long start;
}				t_config;

typedef struct locks_s
{
	pthread_mutex_t	*coder_state_locks;
	pthread_mutex_t	*simul_state_lock;
	pthread_mutex_t	*print_lock;
}				locks_t;

typedef struct simul_state_s
{
	int	*finished_coders;
	int *burn_out_coders;
	int	*is_simul_alive;
}				simul_state_t;


typedef struct coder_s
{
	int				id;
	pthread_t		thread;
	t_config		*config;
	int				compiles_done;
	pthread_mutex_t	*print_lock;
	pthread_mutex_t *simul_state_lock;
	int				last_compile_time;
	simul_state_t	*simul_state;
}				coder_t;

typedef struct monitor_arg_s
{
	t_config		*config;
	pthread_mutex_t *simul_lock;
	simul_state_t	*simul_state;
}				monitor_arg_t;



t_config		*parse_arg(int argc, char **argv, char **possible_schedul_val);
long long		get_current_time(struct timeval* tv);
void 			start_to_work(t_config *config, locks_t *locks, simul_state_t *simul_state);
locks_t	*create_locks(void);
void	destroy_and_free_locks(locks_t *ptr);
simul_state_t	*init_simul(void);
void free_simul(simul_state_t *simul);

#endif