/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/17 11:10:51 by danborys          #+#    #+#             */
/*   Updated: 2026/04/22 13:36:13 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

monitor_t	*init_monitor(
	t_config *config,
	simul_t *simul_state,
	coder_t *coders)
{
	monitor_t	*mon;

	mon = malloc(sizeof(monitor_t));
	if (!mon)
		return (NULL);
	mon->config = config;
	mon->coders = coders;
	mon->simul = simul_state;
	return (mon);
}