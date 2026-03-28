/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 14:14:20 by danborys          #+#    #+#             */
/*   Updated: 2026/03/28 15:12:36 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void *routine(void* arg)
{
	struct timeval	tv;
	t_coder 		*coder;

	coder = (t_coder *)arg;
	while (coder->compiles_done < coder->config->number_of_compiles_required)
	{
		pthread_mutex_lock(coder->print_lock);
		printf("%lld %d is compiling\n", get_current_time(&tv) - coder->config->start, coder->id);
		pthread_mutex_unlock(coder->print_lock);
		usleep((coder->config->time_to_compile) * 1000);
		coder->compiles_done++;
		pthread_mutex_lock(coder->print_lock);
		printf("%lld %d is debugging\n", get_current_time(&tv) - coder->config->start, coder->id);
		pthread_mutex_unlock(coder->print_lock);
		usleep((coder->config->time_to_debug) * 1000);
		pthread_mutex_lock(coder->print_lock);
		printf("%lld %d is refuctoring\n", get_current_time(&tv) - coder->config->start, coder->id);
		pthread_mutex_unlock(coder->print_lock);
		usleep((coder->config->time_to_refactor) * 1000);
	}
	return(NULL);
}

t_coder	*init_coders(t_config *config, pthread_mutex_t *print_lock)
{
	t_coder		*coders;
	int			i;

	coders = malloc(sizeof(t_coder) * config->number_of_coders);
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
	t_coder *coders;
	int	i;

	coders = init_coders(config, print_lock);
	i = 0;
	while (i < config->number_of_coders)
	{
		pthread_create(&coders[i].thread, NULL, routine, &coders[i]);
		i++;
	}
	i = 0;
	while (i < config->number_of_coders)
	{
		pthread_join(coders[i].thread, NULL);
		i++;
	}
	free(coders);
}
