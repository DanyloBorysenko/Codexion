/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/17 11:10:51 by danborys          #+#    #+#             */
/*   Updated: 2026/04/24 18:47:48 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

monitor_t	*init_monitor(
	t_config *config,
	simul_t *simul,
	coder_t *coders,
	dongle_t *dongles)
{
	monitor_t	*mon;

	mon = malloc(sizeof(monitor_t));
	if (!mon)
		return (NULL);
	mon->config = config;
	mon->coders = coders;
	mon->simul = simul;
	mon->dongles = dongles;
	// mon->finished = 0;
	// mon->burned_out = 0;
	// pthread_mutex_init(&mon->lock, NULL);
	// pthread_cond_init(&mon->cond, NULL);
	return (mon);
}

// void destroy_monitor(monitor_t *mon)
// {
// 	if (!mon)
// 		return ;
// 	pthread_mutex_destroy(&mon->lock);
// 	pthread_cond_destroy(&mon->cond);
// 	free(mon);
// }
