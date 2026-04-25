/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/10 19:25:10 by danborys          #+#    #+#             */
/*   Updated: 2026/04/25 19:41:42 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

dongle_t    *init_dongles(int coders_count, int cooldown, char *sched)
{
    dongle_t    *dongles;
    int         i;

    dongles = malloc(sizeof(dongle_t) * coders_count);
    if (!dongles)
        return (NULL);

    for (i = 0; i < coders_count; i++)
    {
        dongles[i].heap = NULL;
        pthread_mutex_init(&dongles[i].lock, NULL);
        pthread_cond_init(&dongles[i].cond, NULL);
        dongles[i].num = i + 1;
        dongles[i].in_use = 0;
        dongles[i].release = 0;
		dongles[i].cooldown = cooldown;
        dongles[i].heap = init_heap(HEAP_SIZE, sched);
        if (!dongles[i].heap)
        {
            destroy_dongles(dongles, i + 1);
            return (NULL);
        }
    }
    return (dongles);
}

void destroy_dongles(dongle_t *dongles, int count)
{
    if (!dongles)
        return;
    for (int i = 0; i < count; i++)
    {
		destroy_heap(dongles[i].heap);
        pthread_mutex_destroy(&dongles[i].lock);
        pthread_cond_destroy(&dongles[i].cond);
    }
    free(dongles);
}
