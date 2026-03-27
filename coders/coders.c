/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 14:14:20 by danborys          #+#    #+#             */
/*   Updated: 2026/03/27 17:43:51 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void *routine(void* arg)
{
	struct timeval tv;
	t_coder *coder;

	coder = (t_coder *)arg;

	printf("time: %llu, Thread id: %d\n", get_current_time(&tv) - coder->config->start, coder->id);
	return(NULL);
}

t_coder	*init_coders(t_config *config)
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
		i++;
	}
	return (coders);
}

void start_to_work(t_config *config)
{
	t_coder *coders;
	int	i;

	coders = init_coders(config);
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
