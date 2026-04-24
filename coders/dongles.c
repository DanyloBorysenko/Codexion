/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/10 19:25:10 by danborys          #+#    #+#             */
/*   Updated: 2026/04/24 12:42:30 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

dongle_t	*init_dongles(int coders_count)
{
	dongle_t	*dongles;
	int			i;

	dongles = malloc(sizeof(dongle_t) * coders_count);
	if (!dongles)
		return (NULL);
	i = 0;
	while (i < coders_count)
	{
		dongles[i].num = i + 1;
		dongles[i].owner_id = 0;
		dongles[i].release = 0;
		pthread_mutex_init(&dongles[i].lock, NULL);
		pthread_cond_init(&dongles[i].cond, NULL);
		i++;
	}
	return (dongles);
}

void destroy_dongles(dongle_t *dongles, int coders_count)
{
	int	i;

	if (!dongles)
		return ;
	i = 0;
	while (i < coders_count)
	{
		pthread_mutex_destroy(&dongles[i].lock);
		pthread_cond_destroy(&dongles[i].cond);
		i++;
	}
	free(dongles);
}
