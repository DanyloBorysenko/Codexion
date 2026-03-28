/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 14:14:20 by danborys          #+#    #+#             */
/*   Updated: 2026/03/28 21:26:13 by danborys         ###   ########.fr       */
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
	int	iteration_count;
	monitor_arg_t *m_arg;

	m_arg = (monitor_arg_t *)arg;
	iteration_count = 0;
	while (iteration_count < 200)
	{
		i = 0;
		while (i < m_arg->config->number_of_coders)
		{
			if (m_arg->coders[i].compiles_done >= m_arg->config->number_of_compiles_required)
			{
				printf("Coder id: %d reached number of compiles\n", m_arg->coders[i].id);
			}
			i++;
		}
		iteration_count++;
		usleep(2000);
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

	coders = init_coders(config, print_lock);
	m_arg = malloc(sizeof(monitor_arg_t));
	m_arg->coders = coders;
	m_arg->config = config;
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
