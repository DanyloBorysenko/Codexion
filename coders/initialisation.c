/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initialisation.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/04 16:05:57 by danborys          #+#    #+#             */
/*   Updated: 2026/04/06 19:16:50 by danborys         ###   ########.fr       */
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
	m_arg->simul_lock = locks->simul_state_lock;
	return (m_arg);
}

coder_t	*init_coders(t_config *config, locks_t *locks, simul_t *simul)
{
	coder_t		*coders;
	int			i;

	coders = malloc(sizeof(coder_t) * config->number_of_coders);
	if (!coders)
		return (NULL);
	i = 0;
	while (i < config->number_of_coders)
	{
		coders[i].id = i + 1;
		coders[i].config = config;
		coders[i].compiles_done = 0;
		coders[i].last_compile_time = simul->start;
		coders[i].burn_out_time = simul->start + config->time_to_burnout;
		coders[i].print_lock = locks->print_lock;
		coders[i].simul_lock = locks->simul_state_lock;
		coders[i].coder_lock = &(locks->coder_state_locks[i]);
		coders[i].simul = simul;
		i++;
	}
	return (coders);
}

simul_t	*init_simul(void)
{
	simul_t	*ptr;
	struct timeval tv;

	ptr = malloc(sizeof(simul_t));
	if (!ptr)
		return (NULL);
	ptr->finished_coders = 0;
	ptr->is_simul_alive = 1;
	ptr->start = get_current_time(&tv);
	return (ptr);
}
