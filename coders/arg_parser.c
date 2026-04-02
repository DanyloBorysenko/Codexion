/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arg_parser.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 15:38:48 by danborys          #+#    #+#             */
/*   Updated: 2026/04/02 10:56:44 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	terminate_program(char *msg)
{
	printf("%s\n", msg);
	exit(EXIT_FAILURE);
}

int	strict_atoi(char *str)
{
	long	res;
	int		i;

	res = 0;
	i = 0;
	if (!str || str[i] == '\0')
		terminate_program("Empty string found");
	if (str[i] == '+')
		i++;
	else if (str[i] == '-')
		terminate_program("Negative argument found");
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			terminate_program("Not numeric digit found");
		res = res * 10 + (str[i] - '0');
		if (res > INT_MAX)
			terminate_program("Argument bigger that 2147483647 found");
		i++;
	}
	return ((int)res);
}

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
	t_config		*config;
	struct timeval	tv;

	config = malloc(sizeof(t_config));
	if (!config)
		return (NULL);
	config->number_of_coders = args[0];
	config->time_to_burnout = args[1];
	config->time_to_compile = args[2];
	config->time_to_debug = args[3];
	config->time_to_refactor = args[4];
	config->number_of_compiles_required = args[5];
	config->dongle_cooldown = args[6];
	config->scheduler = scheduler_value;
	config->start = get_current_time(&tv);
	return (config);
}

t_config	*parse_arg(int argc, char **argv, char **possible_schedul_val)
{
	int	i;
	int	args[7];

	if (argc != 9)
	{
		printf("Expected count of argc - 9, actual - %d\n", argc);
		exit(EXIT_FAILURE);
	}
	if (is_sched_val_correct(argv[8], possible_schedul_val) == -1)
		terminate_program("Not correct scheduler");
	i = 0;
	while (i < argc - 2)
	{
		args[i] = strict_atoi(argv[i + 1]);
		i++;
	}
	return (create_config(args, argv[8]));
}
