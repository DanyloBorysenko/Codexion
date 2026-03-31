/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 14:14:20 by danborys          #+#    #+#             */
/*   Updated: 2026/03/31 17:50:59 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void log_event(coder_t *coder, char *msg, long long timestamp)
{
	pthread_mutex_lock(coder->print_lock);
	printf("%lld %d %s\n", timestamp, coder->id, msg);
	pthread_mutex_unlock(coder->print_lock);
}

// function return 1 if simulation must be stoped
int	stop_simul(coder_t *coder)
{
	int	i;
	int coders_finished;

	i = 0;
	coders_finished = 0;
	while (i < coder->config->number_of_coders)
	{
		pthread_mutex_lock(coder[i].simul_lock);
		if (coder[i].compiles_done >= coder[i].config->number_of_compiles_required)
			coders_finished++;
		pthread_mutex_unlock(coder[i].simul_lock);
		i++;
	}
	return (coders_finished == coder->config->number_of_coders);
}

void *monitor_routine(void *arg)
{
	int	i;
	monitor_arg_t *m_arg;
	int	stop;
	
	m_arg = (monitor_arg_t *)arg;
	stop = 0;
	while (stop == 0)
	{
		i = 0;
		if (stop_simul(m_arg->coders) == 1)
		{
			pthread_mutex_lock(m_arg->simul_lock);
			*(m_arg->is_simul_alive) = 0;
			stop = 1;
			pthread_mutex_unlock(m_arg->simul_lock);
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
	
	stop = 0;
	while (stop == 0)
	{
		coder->last_compile_start_time = get_current_time(&tv) - coder->config->start;
		log_event(coder, "is compiling", coder->last_compile_start_time);
		usleep((coder->config->time_to_compile) * 1000);
		pthread_mutex_lock(coder->simul_lock);
		coder->compiles_done++;
		pthread_mutex_unlock(coder->simul_lock);
		log_event(coder, "is debugging", get_current_time(&tv) - coder->config->start);
		usleep((coder->config->time_to_debug) * 1000);
		log_event(coder, "is refactoring", get_current_time(&tv) - coder->config->start);
		usleep((coder->config->time_to_refactor) * 1000);
		pthread_mutex_lock(coder->simul_lock);
		if (*(coder->is_simul_alive) == 0)
			stop = 1;
		pthread_mutex_unlock(coder->simul_lock);
	}
	return(NULL);
}

coder_t	*init_coders(t_config *config, pthread_mutex_t *print_lock, int *is_simul_alive, pthread_mutex_t *simul_lock)
{
	coder_t		*coders;
	int			i;

	coders = malloc(sizeof(coder_t) * config->number_of_coders);
	if (!coders)
	{
		printf("Coders were not created\n");
		exit(EXIT_FAILURE);
	}
	i = 0;
	while (i < config->number_of_coders)
	{
		coders[i].id = i + 1;
		coders[i].config = config;
		coders[i].compiles_done = 0;
		coders[i].print_lock = print_lock;
		coders[i].is_simul_alive = is_simul_alive;
		coders[i].simul_lock = simul_lock;
		i++;
	}
	return (coders);
}

void start_to_work(t_config *config, pthread_mutex_t *print_lock, pthread_mutex_t *simul_lock)
{
	coder_t			*coders;
	pthread_t		monitor;
	monitor_arg_t	*m_arg;
	int	i;
	int	is_sim_alive;

	is_sim_alive = 1;
	coders = init_coders(config, print_lock, &is_sim_alive, simul_lock);
	m_arg = malloc(sizeof(monitor_arg_t));
	m_arg->coders = coders;
	m_arg->config = config;
	m_arg->is_simul_alive = &is_sim_alive;
	m_arg->simul_lock = simul_lock;

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
