/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initialisation.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/04 16:05:57 by danborys          #+#    #+#             */
/*   Updated: 2026/04/15 00:25:11 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

monitor_arg_t	*init_monitor(
	t_config *config,
	locks_t *locks,
	simul_t *simul_state,
	coder_t *coders)
{
	monitor_arg_t	*m_arg;

	m_arg = malloc(sizeof(monitor_arg_t));
	if (!m_arg)
		return (NULL);
	m_arg->config = config;
	m_arg->coders = coders;
	m_arg->simul = simul_state;
	m_arg->locks = locks;
	return (m_arg);
}

coder_t	*init_coders(shared_arg_t init_arg)
{
	coder_t		*coders;
	int			i;

	coders = malloc(sizeof(coder_t) * init_arg.conf->number_of_coders);
	if (!coders)
		return (NULL);
	i = 0;
	while (i < init_arg.conf->number_of_coders)
	{
		coders[i].id = i + 1;
		coders[i].config = init_arg.conf;
		coders[i].allowed = 0;
		coders[i].left_dng = &init_arg.dngls[i];
		coders[i].right_dng = &init_arg.dngls[((i + 1) % init_arg.conf->number_of_coders)];
		coders[i].compiles_done = 0;
		coders[i].last_compile_time = init_arg.sim->start;
		coders[i].locks = init_arg.locks;
		coders[i].simul = init_arg.sim;
		coders[i].heap = init_arg.heap;
		pthread_mutex_init(&coders[i].coder_lock, NULL);
		i++;
	}
	return (coders);
}

void	destroy_coders(coder_t *coders, int count)
{
	int	i;

	if (!coders)
		return ;
	i = 0;
	while (i < count)
	{
		pthread_mutex_destroy(&coders[i].coder_lock);
		i++;
	}
	free(coders);
}

simul_t	*init_simul(void)
{
	simul_t			*ptr;
	struct timeval	tv;

	ptr = malloc(sizeof(simul_t));
	if (!ptr)
		return (NULL);
	ptr->finished_coders = 0;
	ptr->is_simul_alive = 1;
	ptr->start = get_current_time(&tv);
	return (ptr);
}
