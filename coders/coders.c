/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 14:14:20 by danborys          #+#    #+#             */
/*   Updated: 2026/04/09 13:50:55 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void *monitor_routine(void *arg)
{
	monitor_arg_t *m_arg;
	struct timeval t;
	long long current_time;
	long long burn_out_time;
	int	stop;
	int	i;

	m_arg = (monitor_arg_t *)arg;
	stop = 0;
	while (stop == 0)
	{
		pthread_mutex_lock(m_arg->simul_lock);
		if (m_arg->simul->finished_coders == m_arg->config->number_of_coders)
		{
			m_arg->simul->is_simul_alive = 0;
			pthread_mutex_unlock(m_arg->simul_lock);
			printf("Finished = all\n");
			break;
		}
		pthread_mutex_unlock(m_arg->simul_lock);
		i = 0;
		while (i < m_arg->config->number_of_coders)
		{

			current_time =  get_current_time(&t);
			pthread_mutex_lock(m_arg->coders[i].coder_lock);
			burn_out_time = m_arg->coders[i].burn_out_time;
			// printf("cur time %llu, burn out time %llu\n", current_time, burn_out_time);
			pthread_mutex_unlock(m_arg->coders[i].coder_lock);
			if (current_time > burn_out_time)
			{
				stop = 1;
				log_event(m_arg->coders->print_lock, (m_arg->coders)[i].id, "burned out", current_time - m_arg->simul->start);
				pthread_mutex_lock(m_arg->simul_lock);
				m_arg->simul->is_simul_alive = 0;
				pthread_mutex_unlock(m_arg->simul_lock);
				break;
			}
			i++;
		}
		if (stop == 1)
			break;
		usleep(1000);
	}
	return (NULL);
}

int compile(coder_t *coder)
{
	struct timeval tv;
	long long current_time;
	int is_simul_alive;

	pthread_mutex_lock(coder->simul_lock);
	is_simul_alive = coder->simul->is_simul_alive;
	pthread_mutex_unlock(coder->simul_lock);
	if (is_simul_alive == 0)
		return (0);
	current_time = get_current_time(&tv);
	log_event(coder->print_lock, coder->id, "is compiling", current_time - coder->simul->start);
	pthread_mutex_lock(coder->coder_lock);
	coder->last_compile_time = current_time;
	coder->burn_out_time = current_time + coder->config->time_to_burnout;
	pthread_mutex_unlock(coder->coder_lock);
	usleep((coder->config->time_to_compile) * 1000);
	coder->compiles_done++;
	if (coder->compiles_done == coder->config->number_of_compiles_required)
	{
		pthread_mutex_lock(coder->simul_lock);
		coder->simul->finished_coders = coder->simul->finished_coders + 1;
		pthread_mutex_unlock(coder->simul_lock);
	}
	return (1);
}

int debug(coder_t *coder)
{
	struct timeval tv;
	long long time;
	int is_simul_alive;

	pthread_mutex_lock(coder->simul_lock);
	is_simul_alive = coder->simul->is_simul_alive;
	pthread_mutex_unlock(coder->simul_lock);
	if (is_simul_alive == 0)
		return (0);
	time = get_current_time(&tv) - coder->simul->start;
	log_event(coder->print_lock, coder->id, "is debugging", time);
	usleep((coder->config->time_to_debug) * 1000);
	return (1);
}

int refact(coder_t *coder)
{
	struct timeval tv;
	long long time;
	int is_simul_alive;

	pthread_mutex_lock(coder->simul_lock);
	is_simul_alive = coder->simul->is_simul_alive;
	pthread_mutex_unlock(coder->simul_lock);
	if (is_simul_alive == 0)
		return (0);
	time = get_current_time(&tv) - coder->simul->start;
	log_event(coder->print_lock, coder->id, "is refactoring", time);
	usleep((coder->config->time_to_refactor) * 1000);
	return (1);
}

void *coders_routine(void* arg)
{
	coder_t 		*coder;
	coder = (coder_t *)arg;

	while (1)
	{
		if (!compile(coder))
            break;
        if (!debug(coder))
            break;
        if (!refact(coder))
            break;
	}
	return(NULL);
}

void start_to_work(t_config *config, locks_t *locks, simul_t *simul_state)
{
	coder_t			*coders;
	dongle_t		*dongles;
	pthread_t		monitor;
	monitor_arg_t	*m_arg;
	int	i;

	coders = init_coders(config, locks, simul_state);
	m_arg = init_monitor(config, locks, simul_state, coders);
	dongles = init_dongles(config->number_of_coders);
	i = 0;
	while (i < config->number_of_coders)
	{
		pthread_create(&coders[i].thread, NULL, coders_routine, &coders[i]);
		i++;
	}
	pthread_create(&monitor, NULL, monitor_routine, m_arg);
	i = 0;
	while (i < config->number_of_coders)
	{
		pthread_join(coders[i].thread, NULL);
		i++;
	}
	pthread_join(monitor, NULL);
	free(dongles);
	free(coders);
	free(m_arg);
}
