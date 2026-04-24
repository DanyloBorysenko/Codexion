/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/04 15:55:19 by danborys          #+#    #+#             */
/*   Updated: 2026/04/23 14:19:10 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void log_event(simul_t	*sim, int id, char *msg, long long time)
{
	pthread_mutex_lock(&sim->print_lock);
	printf("%llu %d %s\n", time - sim->start, id, msg);
	pthread_mutex_unlock(&sim->print_lock);
}
