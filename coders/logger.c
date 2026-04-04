/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/04 15:55:19 by danborys          #+#    #+#             */
/*   Updated: 2026/04/04 16:23:26 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void log_event(pthread_mutex_t *mut, int id, char *msg, int time)
{
	pthread_mutex_lock(mut);
	printf("%d %d %s\n", time, id, msg);
	pthread_mutex_unlock(mut);
}