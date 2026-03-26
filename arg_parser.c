/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arg_parser.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 15:38:48 by danborys          #+#    #+#             */
/*   Updated: 2026/03/25 18:17:26 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_sched_val_correct(char *scheduler_value, char **possible_schedul_val)
{
	int	i;
	int	is_correct;
	
	i = 0;
	while (possible_schedul_val[i])
	{
		is_correct = strcmp(possible_schedul_val[i], scheduler_value);
		if (is_correct == 0)
			return (0);
		i++;
	}
	return (-1);
}

t_config	*create_config(int *args, char *scheduler_value)
{
	t_config	*config;
	
	config = malloc(sizeof(t_config));
	if (!config)
		return (NULL);
	config->number_of_coders = args[1];
	config->time_to_burnout = args[2];
	config->time_to_compile = args[3];
	config->time_to_debug = args[4];
	config->time_to_refactor = args[5];
	config->number_of_compiles_required = args[6];
	config->dongle_cooldown = args[7];
	config->scheduler = scheduler_value;
	return (config);
}

t_config	*parse_arg(int argc, char **argv, char **possible_schedul_val)
{
	int	i;
	int	arg;
	int	args[7];

	if (argc != 9)
	{
		printf("Expected count of argc - 9, actual - %d\n", argc);
		exit(EXIT_FAILURE);
	}
	if (is_sched_val_correct(argv[8], possible_schedul_val) == -1)
	{
		printf("Not correct scheduler");
		exit(EXIT_FAILURE);
	}
	i = 0;
	while (i < 7)
	{
		arg = atoi(argv[i + 1]);
		if (arg < 0)
		{
			printf("Argument can not be negative or bigger than 2147483647");
			exit(EXIT_FAILURE);
		}
		args[i] = arg;
		i++;
	}
	return (create_config(args, argv[8]));
}
