/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 14:14:20 by danborys          #+#    #+#             */
/*   Updated: 2026/04/03 14:46:06 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void log_event(pthread_mutex_t *mut, int id, char *msg, int time)
{
	
	pthread_mutex_lock(mut);
	printf("%d %d %s\n", time, id, msg);
	pthread_mutex_unlock(mut);
}

void *monitor_routine(void *arg)
{
	monitor_arg_t *m_arg;
	struct timeval t;
	long long current_time;
	long long last_compl_time;
	int	stop;
	int	i;

	m_arg = (monitor_arg_t *)arg;
	stop = 0;
	while (stop == 0)
	{
		pthread_mutex_lock(m_arg->simul_lock);
		if (*(m_arg->simul->finished_coders) == m_arg->config->number_of_coders)
		{
			*(m_arg->simul->is_simul_alive) = 0;
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
			last_compl_time = m_arg->coders[i].last_compile_time;
			pthread_mutex_unlock(m_arg->coders[i].coder_lock);
			if (current_time - last_compl_time > m_arg->config->time_to_burnout)
			{
				stop = 1;
				log_event(m_arg->coders->print_lock, (m_arg->coders)[i].id, "borned out", current_time - m_arg->config->start);
				pthread_mutex_lock(m_arg->simul_lock);
				*(m_arg->simul->is_simul_alive) = 0;
				pthread_mutex_unlock(m_arg->simul_lock);
				break;
			}
			if (stop == 1)
				break;
			i++;
		}
		usleep(1000);
	}
	return (NULL);
}

void *coders_routine(void* arg)
{
	coder_t 		*coder;
	struct timeval	tv;
	int	stop;
	coder = (coder_t *)arg;
	long long		current_time;
	

	stop = 0;
	while (stop == 0)
	{
		current_time = get_current_time(&tv);
		log_event(coder->print_lock, coder->id, "is compiling", current_time - coder->config->start);
		pthread_mutex_lock(coder->coder_lock);
		coder->last_compile_time = current_time;
		pthread_mutex_unlock(coder->coder_lock);
		usleep((coder->config->time_to_compile) * 1000);
		coder->compiles_done++;
		if (coder->compiles_done == coder->config->number_of_compiles_required)
		{
			pthread_mutex_lock(coder->simul_state_lock);
			*(coder->simul_state->finished_coders) = *(coder->simul_state->finished_coders) + 1;
			pthread_mutex_unlock(coder->simul_state_lock);
		}
		log_event(coder->print_lock, coder->id, "is debugging", get_current_time(&tv) - coder->config->start);
		usleep((coder->config->time_to_debug) * 1000);
		log_event(coder->print_lock, coder->id, "is refactoring", get_current_time(&tv) - coder->config->start);
		usleep((coder->config->time_to_refactor) * 1000);
		pthread_mutex_lock(coder->simul_state_lock);
		if (*(coder->simul_state->is_simul_alive) == 0)
			stop = 1;
		pthread_mutex_unlock(coder->simul_state_lock);
	}
	return(NULL);
}

coder_t	*init_coders(t_config *config, locks_t *locks, simul_state_t *simul_state)
{
	coder_t		*coders;
	int			i;

	coders = malloc(sizeof(coder_t) * config->number_of_coders);
	if (!coders)
		return (NULL);
	i = 0;
	while (i < config->number_of_coders)
	{
		coders[i].id = i + 1;
		coders[i].config = config;
		coders[i].compiles_done = 0;
		coders[i].last_compile_time = config->start;
		coders[i].print_lock = locks->print_lock;
		coders[i].simul_state_lock = locks->simul_state_lock;
		coders[i].coder_lock = &(locks->coder_state_locks[i]);
		coders[i].simul_state = simul_state;
		i++;
	}
	return (coders);
}

void start_to_work(t_config *config, locks_t *locks, simul_state_t *simul_state)
{
	coder_t			*coders;
	pthread_t		monitor;
	monitor_arg_t	*m_arg;
	int	i;

	coders = init_coders(config, locks, simul_state);
	m_arg = malloc(sizeof(monitor_arg_t));
	if (!m_arg)
		return;
	m_arg->config = config;
	m_arg->coders = coders;
	m_arg->simul = simul_state;
	m_arg->simul_lock = locks->simul_state_lock;
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
	free(coders);
	free(m_arg);
}
