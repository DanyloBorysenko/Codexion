/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/17 11:10:51 by danborys          #+#    #+#             */
/*   Updated: 2026/05/01 16:26:00 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	wake_up_all(t_simul *sim)
{
	pthread_mutex_lock(&sim->lock);
	sim->is_finished = 1;
	pthread_cond_broadcast(&sim->cond);
	pthread_mutex_unlock(&sim->lock);
	pthread_mutex_lock(&sim->sched_lock);
	pthread_cond_broadcast(&sim->sched_cond);
	pthread_mutex_unlock(&sim->sched_lock);
}

static int	is_burn_out(int count, t_coder *cods, t_simul *s)
{
	int			i;
	long long	now;
	long long	last;

	i = 0;
	while (i < count)
	{
		now = get_cur_time();
		pthread_mutex_lock(&cods[i].lock);
		last = cods[i].last_compile_time;
		pthread_mutex_unlock(&cods[i].lock);
		if (now - last > cods[i].time_to_burnout)
		{
			wake_up_all(s);
			log_event(s, cods[i].id, "burned out", now);
			return (1);
		}
		i++;
	}
	return (0);
}

void	*mon_rout(void *arg)
{
	t_monitor	*mon;
	int			finished;

	mon = (t_monitor *)arg;
	finished = 0;
	while (1)
	{
		pthread_mutex_lock(&mon->simul->lock);
		finished = mon->simul->finished_coders;
		pthread_mutex_unlock(&mon->simul->lock);
		if (finished == mon->cod_count)
		{
			wake_up_all(mon->simul);
			return (NULL);
		}
		if (is_burn_out(mon->cod_count, mon->coders, mon->simul))
			return (NULL);
		usleep(1000);
	}
	return (NULL);
}

t_monitor	*init_monitor(
	int coders_count,
	t_simul *simul,
	t_coder *coders)
{
	t_monitor	*mon;

	mon = malloc(sizeof(t_monitor));
	if (!mon)
		return (NULL);
	mon->cod_count = coders_count;
	mon->coders = coders;
	mon->simul = simul;
	return (mon);
}
