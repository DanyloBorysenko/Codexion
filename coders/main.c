/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 15:39:06 by danborys          #+#    #+#             */
/*   Updated: 2026/04/01 23:14:45 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char	**argv)
{
	t_config	*configuration;
	char		*scheduler_values[3];
	struct timeval tv;
	pthread_mutex_t *coder_locks;
	locks_t			*locks;
	simul_state_t *sim_state;
	long long start;

	start = get_current_time(&tv);
	scheduler_values[0] = "fifo";
	scheduler_values[1] = "edf";
	scheduler_values[2] = NULL;
	configuration = parse_arg(argc, argv, scheduler_values);
	locks = create_locks(configuration->number_of_coders);
	configuration->start = start;
	coder_locks = init_mutexes(configuration->number_of_coders);
	sim_state = init_simul();
	start_to_work(configuration, locks, sim_state);
	destroy_locks(locks, configuration->number_of_coders);
	free(locks);
	free(configuration);
	free(coder_locks);
	free_simul(sim_state);
	return (0);
}
