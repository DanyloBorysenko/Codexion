/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initialisation.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/04 16:05:57 by danborys          #+#    #+#             */
/*   Updated: 2026/04/23 14:41:06 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

coder_t	*init_coders(shared_arg_t arg)
{
	coder_t		*coders;
	int			i;

	coders = malloc(sizeof(coder_t) * arg.conf->number_of_coders);
	if (!coders)
		return (NULL);
	i = 0;
	while (i < arg.conf->number_of_coders)
	{
		coders[i].id = i + 1;
		coders[i].config = arg.conf;
		coders[i].perm = 0;
		coders[i].alive = 1;
		coders[i].left_dng = &arg.dngls[i];
		coders[i].right_dng = &arg.dngls[((i + 1) % arg.conf->number_of_coders)];
		coders[i].compiles_done = 0;
		coders[i].last_compile_time = arg.sim->start;
		coders[i].simul = arg.sim;
		coders[i].heap = arg.heap;
		coders[i].sched = arg.sched;
		pthread_mutex_init(&coders[i].coder_lock, NULL);
		pthread_cond_init(&coders[i].cond, NULL);
		i++;
	}
	return (coders);
}

void	destroy_coders(coder_t *coders, int count)
{
	int	i;

	if (!coders)
		return ;
	i = 0;
	while (i < count)
	{
		pthread_mutex_destroy(&coders[i].coder_lock);
		pthread_cond_destroy(&coders[i].cond);
		i++;
	}
	free(coders);
}
