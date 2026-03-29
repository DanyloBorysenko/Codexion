/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 14:14:20 by danborys          #+#    #+#             */
/*   Updated: 2026/03/29 18:03:19 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void log_event(coder_t *coder, char *msg, long long timestamp)
{
	pthread_mutex_lock(coder->print_lock);
	printf("%lld %d %s\n", timestamp, coder->id, msg);
	pthread_mutex_unlock(coder->print_lock);
}

void *monitor_routine(void *arg)
{
	int	i;
	monitor_arg_t *m_arg;
	int	coders_finished;

	m_arg = (monitor_arg_t *)arg;
	while (*(m_arg->is_simul_alive))
	{
		i = 0;
		coders_finished = 0;
		while (i < m_arg->config->number_of_coders)
		{
			// pthread_mutex_lock(m_arg->print_lock);
			printf("Monitoring...\n");
			// pthread_mutex_unlock(m_arg->print_lock);
			if (m_arg->coders[i].compiles_done < m_arg->config->number_of_compiles_required)
				break;
			if (i == m_arg->config->number_of_coders - 1)
				*(m_arg->is_simul_alive) = 0;
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

	coder = (coder_t *)arg;
	while (coder->compiles_done < coder->config->number_of_compiles_required * 2)
	{
		coder->last_compile_start_time = get_current_time(&tv) - coder->config->start;
		log_event(coder, "is compiling", coder->last_compile_start_time);
		usleep((coder->config->time_to_compile) * 1000);
		coder->compiles_done++;
		log_event(coder, "is debugging", get_current_time(&tv) - coder->config->start);
		usleep((coder->config->time_to_debug) * 1000);
		log_event(coder, "is refactoring", get_current_time(&tv) - coder->config->start);
		usleep((coder->config->time_to_refactor) * 1000);
	}
	return(NULL);
}

coder_t	*init_coders(t_config *config, pthread_mutex_t *print_lock)
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
		i++;
	}
	return (coders);
}

void start_to_work(t_config *config, pthread_mutex_t *print_lock)
{
	coder_t			*coders;
	pthread_t		monitor;
	monitor_arg_t	*m_arg;
	int	i;
	int	is_sim_alive;

	is_sim_alive = 1;
	coders = init_coders(config, print_lock);
	m_arg = malloc(sizeof(monitor_arg_t));
	m_arg->coders = coders;
	m_arg->config = config;
	m_arg->is_simul_alive = &is_sim_alive;
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
