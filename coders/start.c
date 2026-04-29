/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 14:14:20 by danborys          #+#    #+#             */
/*   Updated: 2026/04/29 18:10:29 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_components(t_config *config, t_components *c)
{
	c->sim = init_simul();
	c->dongles = init_dongles(config);
	if (!c->sim || !c->dongles)
		return (0);
	c->coders = init_coders(config, c->sim, c->dongles);
	if (!c->coders)
		return (0);
	c->mon = init_monitor(config->num_of_cod, c->sim, c->coders);
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

int	launch_coders(coder_t *coders, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		if (pthread_create(&coders[i].thread_id, NULL, coder_rout, &coders[i]))
			return (i);
		i++;
	}
	return (count);
}

void	start_simul(t_config *config)
{
	t_components	c;
	int				mon_ok;
	int				launched_coders;

	memset(&c, 0, sizeof(t_components));
	mon_ok = 0;
	launched_coders = 0;
	if (init_components(config, &c))
	{
		launched_coders = launch_coders(c.coders, config->num_of_cod);
		if (launched_coders > 0)
			mon_ok = !pthread_create(&c.mon->thread_id, NULL, mon_rout, c.mon);
	}
	if (launched_coders > 0 && !mon_ok)
		wake_up_all(config->num_of_cod, c.coders, c.sim);
	if (mon_ok)
		pthread_join(c.mon->thread_id, NULL);
	while (launched_coders > 0)
		pthread_join(c.coders[--launched_coders].thread_id, NULL);
	destroy_components(&c, config->num_of_cod);
}
