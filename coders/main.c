/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 15:39:06 by danborys          #+#    #+#             */
/*   Updated: 2026/04/02 09:05:54 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char	**argv)
{
	t_config	*configuration;
	char		*scheduler_values[3];
	struct timeval tv;
	locks_t			*locks;
	simul_state_t *sim_state;
	long long start;

	start = get_current_time(&tv);
	scheduler_values[0] = "fifo";
	scheduler_values[1] = "edf";
	scheduler_values[2] = NULL;
	configuration = parse_arg(argc, argv, scheduler_values);
	locks = create_locks();
	configuration->start = start;
	sim_state = init_simul();
	start_to_work(configuration, locks, sim_state);
	destroy_and_free_locks(locks);
	free(configuration);
	free_simul(sim_state);
	return (0);
}
