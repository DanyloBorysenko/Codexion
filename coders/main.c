/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 15:39:06 by danborys          #+#    #+#             */
/*   Updated: 2026/04/27 11:01:06 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char	**argv)
{
	t_config	config;
	char		*sched_values[SCHED_COUNT];
	
	sched_values[FIFO_SCHED_IND] = "fifo";
	sched_values[EDF_SCHED_IND] = "edf";
	parse_arg(argc, argv, sched_values, &config);
	start_simul(&config);
	return (0);
}
