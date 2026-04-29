/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/10 19:25:10 by danborys          #+#    #+#             */
/*   Updated: 2026/04/29 18:56:46 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	destroy_dongles(dongle_t *dongles, int count)
{
	int	i;

	if (!dongles)
		return ;
	i = 0;
	while (i < count)
	{
		destroy_heap(dongles[i].heap);
		pthread_mutex_destroy(&dongles[i].lock);
		i++;
	}
	free(dongles);
}

static int	init_dongle(dongle_t *d, int i, t_config *conf)
{
	if (pthread_mutex_init(&d->lock, NULL) != 0)
		return (0);
	d->num = i + 1;
	d->in_use = 0;
	d->release = 0;
	d->cooldown = conf->dongle_cooldown;
	d->heap = init_heap(HEAP_SIZE, conf->scheduler);
	if (!d->heap)
	{
		pthread_mutex_destroy(&d->lock);
		return (0);
	}
	return (1);
}

dongle_t	*init_dongles(t_config *conf)
{
	dongle_t	*dongles;
	int			i;

	dongles = malloc(sizeof(dongle_t) * conf->num_of_cod);
	if (!dongles)
		return (NULL);
	i = 0;
	while (i < conf->num_of_cod)
	{
		if (!init_dongle(&dongles[i], i, conf))
		{
			destroy_dongles(dongles, i);
			return (NULL);
		}
		i++;
	}
	return (dongles);
}
