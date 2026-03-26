/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 15:39:06 by danborys          #+#    #+#             */
/*   Updated: 2026/03/26 17:15:32 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char	**argv)
{
	t_config	*configuration;
	char		*scheduler_values[3];
	struct timeval tv;
	long long start;
	
	start = get_current_time(&tv);
	
	scheduler_values[0] = "fifo";
	scheduler_values[1] = "edf";
	scheduler_values[2] = NULL;
	configuration = parse_arg(argc, argv, scheduler_values);
	printf("Number of coders: %d\n", configuration->number_of_coders);
	printf("Scheduler: %s\n", configuration->scheduler);
	free(configuration);
	printf("execution time - %lld\n", get_current_time(&tv) - start);

	return (0);
}
