/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 15:39:06 by danborys          #+#    #+#             */
/*   Updated: 2026/04/17 13:01:15 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char	**argv)
{
	t_config	*config;
	char		*scheduler_values[3];
	simul_t		*simulation;

	scheduler_values[0] = "fifo";
	scheduler_values[1] = "edf";
	scheduler_values[2] = NULL;
	config = parse_arg(argc, argv, scheduler_values);
	simulation = init_simul();
	start_to_work(config, simulation);
	free(config);
	destroy_simul(simulation);
	return (0);
}
