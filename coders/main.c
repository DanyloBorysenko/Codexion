/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 15:39:06 by danborys          #+#    #+#             */
/*   Updated: 2026/04/26 18:11:09 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char	**argv)
{
	t_config	config;
	char		*sched_values[SCHED_COUNT];
	simul_t		*simulation;

	sched_values[FIFO_SCHED_IND] = "fifo";
	sched_values[EDF_SCHED_IND] = "edf";
	parse_arg(argc, argv, sched_values, &config);
	simulation = init_simul();
	start_to_work(&config, simulation);
	destroy_simul(simulation);
	return (0);
}
