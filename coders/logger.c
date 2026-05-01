/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/04 15:55:19 by danborys          #+#    #+#             */
/*   Updated: 2026/05/01 16:24:40 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	log_event(t_simul	*sim, int id, char *msg, long long time)
{
	int	is_finished;

	pthread_mutex_lock(&sim->print_lock);
	pthread_mutex_lock(&sim->lock);
	is_finished = sim->is_finished;
	pthread_mutex_unlock(&sim->lock);
	if (is_finished && strcmp(msg, "burned out") != 0)
	{
		pthread_mutex_unlock(&sim->print_lock);
		return ;
	}
	printf("%llu %d %s\n", time - sim->start, id, msg);
	pthread_mutex_unlock(&sim->print_lock);
}
