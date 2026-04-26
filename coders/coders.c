/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/04 16:05:57 by danborys          #+#    #+#             */
/*   Updated: 2026/04/26 16:35:43 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	init_coder(coder_t *cod, int i, shared_arg_t arg)
{
	cod->id = i + 1;
	cod->left_dng = &arg.dngls[i];
	cod->right_dng = &arg.dngls[((i + 1) % arg.conf->number_of_coders)];
	cod->compiles_done = 0;
	cod->time_to_burnout = arg.conf->time_to_burnout;
	cod->time_to_compile = arg.conf->time_to_compile;
	cod->time_to_debug = arg.conf->time_to_debug;
	cod->time_to_refactor = arg.conf->time_to_refactor;
	cod->num_of_comp_req = arg.conf->number_of_compiles_required;
	cod->last_compile_time = arg.sim->start;
	cod->simul = arg.sim;
	pthread_mutex_init(&cod->coder_lock, NULL);
	pthread_cond_init(&cod->cond, NULL);
}

coder_t	*init_coders(shared_arg_t arg)
{
	coder_t	*coders;
	int		i;

	coders = malloc(sizeof(coder_t) * arg.conf->number_of_coders);
	if (!coders)
		return (NULL);
	i = 0;
	while (i < arg.conf->number_of_coders)
	{
		init_coder(&coders[i], i, arg);
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
