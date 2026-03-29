/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 15:39:00 by danborys          #+#    #+#             */
/*   Updated: 2026/03/29 17:44:02 by danborys         ###   ########.fr       */
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

typedef struct coder_s
{
	int				id;
	pthread_t		thread;
	t_config		*config;
	int				compiles_done;
	pthread_mutex_t	*print_lock;
	long long		last_compile_start_time;
}				coder_t;

typedef struct monitor_arg_s
{
	coder_t			*coders;
	t_config		*config;
	pthread_mutex_t	*print_lock;
	int				*is_simul_alive;
}				monitor_arg_t;



t_config	*parse_arg(int argc, char **argv, char **possible_schedul_val);
long long	get_current_time(struct timeval* tv);
void		start_to_work(t_config *config, pthread_mutex_t *print_lock);

#endif