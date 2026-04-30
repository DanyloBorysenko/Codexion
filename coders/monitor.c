/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/17 11:10:51 by danborys          #+#    #+#             */
/*   Updated: 2026/04/30 15:48:22 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	wake_up_all(simul_t *sim)
{
	pthread_mutex_lock(&sim->sim_lock);
	sim->is_finished = 1;
	pthread_cond_broadcast(&sim->cond);
	pthread_mutex_unlock(&sim->sim_lock);
	pthread_mutex_lock(&sim->sched_lock);
	pthread_cond_broadcast(&sim->sched_cond);
	pthread_mutex_unlock(&sim->sched_lock);
}

static int	is_burn_out(int count, coder_t *cods, simul_t *s)
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
	monitor_t	*mon;
	int			finished;

	mon = (monitor_t *)arg;
	finished = 0;
	while (1)
	{
		pthread_mutex_lock(&mon->simul->sim_lock);
		finished = mon->simul->finished_coders;
		pthread_mutex_unlock(&mon->simul->sim_lock);
		if (finished == mon->cod_count)
		{
			wake_up_all(mon->simul);
			printf("Finished = all\n");
			return (NULL);
		}
		if (is_burn_out(mon->cod_count, mon->coders, mon->simul))
			return (NULL);
		usleep(1000);
	}
	return (NULL);
}

monitor_t	*init_monitor(
	int coders_count,
	simul_t *simul,
	coder_t *coders)
{
	monitor_t	*mon;

	mon = malloc(sizeof(monitor_t));
	if (!mon)
		return (NULL);
	mon->cod_count = coders_count;
	mon->coders = coders;
	mon->simul = simul;
	return (mon);
}
