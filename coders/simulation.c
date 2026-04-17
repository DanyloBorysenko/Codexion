/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/17 12:56:26 by danborys          #+#    #+#             */
/*   Updated: 2026/04/17 23:30:17 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

simul_t	*init_simul(void)
{
	simul_t			*ptr;
	struct timeval	tv;
	long long		time;

	ptr = malloc(sizeof(simul_t));
	if (!ptr)
		return (NULL);
	pthread_mutex_init(&ptr->sim_lock, NULL);
	pthread_mutex_init(&ptr->print_lock, NULL);
	ptr->finished_coders = 0;
	gettimeofday(&tv, NULL);
	time = (long long)(tv.tv_sec * 1000) + (tv.tv_usec / 1000);
	ptr->start = time;
	return (ptr);
}

void	destroy_simul(simul_t *sim)
{
	pthread_mutex_destroy(&sim->sim_lock);
	pthread_mutex_destroy(&sim->print_lock);
	free(sim);
}
