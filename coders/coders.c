/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/04 16:05:57 by danborys          #+#    #+#             */
/*   Updated: 2026/05/02 09:53:41 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_coder(t_coder *cod, t_dongle *don, t_config *conf, t_simul *s)
{
	int	coder_id;

	coder_id = cod->id;
	cod->left_dng = &don[coder_id - 1];
	cod->right_dng = &don[((coder_id) % conf->num_of_cod)];
	cod->compiles_done = 0;
	cod->time_to_burnout = conf->time_to_burnout;
	cod->time_to_compile = conf->time_to_compile;
	cod->time_to_debug = conf->time_to_debug;
	cod->time_to_refactor = conf->time_to_refactor;
	cod->num_of_comp_req = conf->num_of_comp_req;
	cod->last_compile_time = s->start;
	cod->sim = s;
	if (pthread_mutex_init(&cod->lock, NULL) != 0)
		return (0);
	return (1);
}

t_coder	*init_coders(t_config *conf, t_simul *sim, t_dongle *don)
{
	t_coder	*coders;
	int		i;

	coders = malloc(sizeof(t_coder) * conf->num_of_cod);
	if (!coders)
		return (NULL);
	i = 0;
	while (i < conf->num_of_cod)
	{
		coders[i].id = i + 1;
		if (!init_coder(&coders[i], don, conf, sim))
		{
			destroy_coders(coders, i);
			return (NULL);
		}
		i++;
	}
	return (coders);
}

void	destroy_coders(t_coder *coders, int count)
{
	int	i;

	if (!coders)
		return ;
	i = 0;
	while (i < count)
	{
		pthread_mutex_destroy(&coders[i].lock);
		i++;
	}
	free(coders);
}
