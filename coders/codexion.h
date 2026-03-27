/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 15:39:00 by danborys          #+#    #+#             */
/*   Updated: 2026/03/27 17:52:37 by danborys         ###   ########.fr       */
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

typedef struct s_coder
{
	int			id;
	pthread_t	thread;
	t_config	*config;
	int			compiles_done;
}				t_coder;


t_config	*parse_arg(int argc, char **argv, char **possible_schedul_val, long long start);
long long	get_current_time(struct timeval* tv);
void		start_to_work(t_config *config);

#endif