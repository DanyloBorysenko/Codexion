/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 15:39:06 by danborys          #+#    #+#             */
/*   Updated: 2026/03/29 17:49:18 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char	**argv)
{
	t_config	*configuration;
	char		*scheduler_values[3];
	struct timeval tv;
	pthread_mutex_t print_lock;
	long long start;

	start = get_current_time(&tv);

	scheduler_values[0] = "fifo";
	scheduler_values[1] = "edf";
	scheduler_values[2] = NULL;
	configuration = parse_arg(argc, argv, scheduler_values);
	configuration->start = start;
	pthread_mutex_init(&print_lock, NULL);
	start_to_work(configuration, &print_lock);
	pthread_mutex_destroy(&print_lock);
	free(configuration);
	return (0);
}
