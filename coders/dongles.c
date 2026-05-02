/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/10 19:25:10 by danborys          #+#    #+#             */
/*   Updated: 2026/05/02 09:47:38 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	destroy_dongles(t_dongle *dongles, int count)
{
	int	i;

	if (!dongles)
		return ;
	i = 0;
	while (i < count)
	{
		destroy_heap(dongles[i].heap);
		i++;
	}
	free(dongles);
}

static int	init_dongle(t_dongle *d, int i, t_config *conf)
{
	d->num = i + 1;
	d->in_use = 0;
	d->release = 0;
	d->cooldown = conf->dongle_cooldown;
	d->heap = init_heap(HEAP_SIZE, conf->scheduler);
	if (!d->heap)
		return (0);
	return (1);
}

t_dongle	*init_dongles(t_config *conf)
{
	t_dongle	*dongles;
	int			i;

	dongles = malloc(sizeof(t_dongle) * conf->num_of_cod);
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
