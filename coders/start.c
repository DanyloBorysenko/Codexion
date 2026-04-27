/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   work.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 14:14:20 by danborys          #+#    #+#             */
/*   Updated: 2026/04/27 15:39:44 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	join_threads(monitor_t *mon, coder_t *coders, int count)
{
	int	i;

	pthread_join(mon->thread_id, NULL);
	i = 0;
	while (i < count)
	{
		pthread_join(coders[i].thread_id, NULL);
		i++;
	}
}

static int	init_components(t_config *config, t_components *c)
{
	c->sim = init_simul();
	c->dongles = init_dongles(config);
	if (!c->sim || !c->dongles)
		return (0);
	c->coders = init_coders(config, c->sim, c->dongles);
	if (!c->coders)
		return (0);
	c->mon = init_monitor(config->num_of_cod, c->sim, c->coders, c->dongles);
	if (!c->mon)
		return (0);
	return (1);
}

static void	destroy_components(t_components *comp, int coders_count)
{
	destroy_coders(comp->coders, coders_count);
	free(comp->mon);
	destroy_dongles(comp->dongles, coders_count);
	destroy_simul(comp->sim);
}

void	start_simul(t_config *config)
{
	t_components	compnts;

	memset(&compnts, 0, sizeof(t_components));
	if (!init_components(config, &compnts))
	{
		destroy_components(&compnts, config->num_of_cod);
		return ;
	}
	if (!launch_coders(compnts.coders, config->num_of_cod))
	{
		destroy_components(&compnts, config->num_of_cod);
		return ;
	}
	if (pthread_create(&compnts.mon->thread_id, NULL, mon_routine, compnts.mon))
	{
		destroy_components(&compnts, config->num_of_cod);
		return ;
	}
	join_threads(compnts.mon, compnts.coders, config->num_of_cod);
	destroy_components(&compnts, config->num_of_cod);
}
