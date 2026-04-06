/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 15:39:06 by danborys          #+#    #+#             */
/*   Updated: 2026/04/06 17:47:43 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char	**argv)
{
	t_config	*config;
	char		*scheduler_values[3];
	locks_t			*locks;
	simul_state_t *sim_state;

	scheduler_values[0] = "fifo";
	scheduler_values[1] = "edf";
	scheduler_values[2] = NULL;
	config = parse_arg(argc, argv, scheduler_values);
	locks = create_locks(config->number_of_coders);
	sim_state = init_simul();
	start_to_work(config, locks, sim_state);
	destroy_locks(locks, config->number_of_coders);
	free_locks(locks);
	free(config);
	free(sim_state);
	return (0);
}
