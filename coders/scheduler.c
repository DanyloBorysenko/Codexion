/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 16:12:50 by danborys          #+#    #+#             */
/*   Updated: 2026/04/19 09:58:24 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

scheduler_t	*init_sched(heap_t	*heap)
{
	scheduler_t	*sched;

	sched = malloc(sizeof(scheduler_t));
	if (!sched)
		return (NULL);
	sched->heap = heap;
	sched->alive = 1;
	sched->called = 0;
	pthread_mutex_init(&sched->lock, NULL);
	pthread_cond_init(&sched->cond, NULL);
	return (sched);
}

void destroy_sched(scheduler_t *sched)
{
	pthread_mutex_destroy(&sched->lock);
	pthread_cond_destroy(&sched->cond);
	free(sched);
}