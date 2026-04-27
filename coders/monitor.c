/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/17 11:10:51 by danborys          #+#    #+#             */
/*   Updated: 2026/04/27 15:33:38 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	wake_up_all(int count, dongle_t *don, simul_t *sim)
{
	int	i;

	pthread_mutex_lock(&sim->sim_lock);
	pthread_cond_broadcast(&sim->cond);
	pthread_mutex_unlock(&sim->sim_lock);
	i = 0;
	while (i < count)
	{
		pthread_mutex_lock(&don[i].lock);
		pthread_cond_broadcast(&don[i].cond);
		pthread_mutex_unlock(&don[i].lock);
		i++;
	}
}

void	*mon_routine(void *arg)
{
	monitor_t	*mon;
	long long	now;
	long long	last;
	int i;

	mon = (monitor_t*)arg;
	while (1)
	{
		// printf("%llu: Monitor is cheking\n", get_current_time() - mon->simul->start);
		pthread_mutex_lock(&mon->simul->sim_lock);
		if (mon->simul->finished_coders == mon->coders_count)
		{
			mon->simul->is_finished = 1;
			pthread_mutex_unlock(&mon->simul->sim_lock);
			wake_up_all(mon->coders_count, mon->dongles, mon->simul);
			printf("Finished = all\n");
			return (NULL);
		}
		pthread_mutex_unlock(&mon->simul->sim_lock);
		i = 0;
		while (i < mon->coders_count)
		{
			now = get_current_time();
			pthread_mutex_lock(&mon->coders[i].coder_lock);
			last = mon->coders[i].last_compile_time;
			pthread_mutex_unlock(&mon->coders[i].coder_lock);
			if (now - last > mon->coders[i].time_to_burnout)
			{
				pthread_mutex_lock(&mon->simul->sim_lock);
				mon->simul->is_finished = 1;
				pthread_mutex_unlock(&mon->simul->sim_lock);
				log_event(mon->simul, (mon->coders)[i].id, "burned out", now);
				wake_up_all(mon->coders_count, mon->dongles, mon->simul);
				return (NULL);
			}
			i++;
		}
		usleep(1000);
	}
	return (NULL);
}

monitor_t	*init_monitor(
	int coders_count,
	simul_t *simul,
	coder_t *coders,
	dongle_t *dongles)
{
	monitor_t	*mon;

	mon = malloc(sizeof(monitor_t));
	if (!mon)
		return (NULL);
	mon->coders_count = coders_count;
	mon->coders = coders;
	mon->simul = simul;
	mon->dongles = dongles;
	return (mon);
}
