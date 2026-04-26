/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arg_parser.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 15:38:48 by danborys          #+#    #+#             */
/*   Updated: 2026/04/26 17:42:14 by danborys         ###   ########.fr       */
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

int	is_sched_val_correct(char *actual, char **sched_val)
{
	int	i;
	int	is_correct;

	i = 0;
	while (i < SCHED_COUNT)
	{
		if (!sched_val[i])
			return (0);
		is_correct = strcmp(sched_val[i], actual);
		if (is_correct == 0)
			return (1);
		i++;
	}
	return (0);
}

t_config	*create_config(int *args, char *sched_value)
{
	t_config		*config;

	config = malloc(sizeof(t_config));
	if (!config)
		return (NULL);
	config->number_of_coders = args[CONF_CODERS_IDX];
	config->time_to_burnout = args[CONF_BURNOUT_IDX];
	config->time_to_compile = args[CONF_COMPILE_IDX];
	config->time_to_debug = args[CONF_DEBUG_IDX];
	config->time_to_refactor = args[CONF_REFACTOR_IDX];
	config->number_of_compiles_required = args[CONF_COMP_REQ_IDX];
	config->dongle_cooldown = args[CONF_COOLDOWN_IDX];
	config->scheduler = sched_value;
	return (config);
}

t_config	*parse_arg(int argc, char **argv, char **sched_values)
{
	int	i;
	int	args[ARG_COUNT - 2];

	if (argc != ARG_COUNT)
	{
		printf("Args count: expected - %d, actual - %d\n", ARG_COUNT, argc);
		exit(EXIT_FAILURE);
	}
	if (!is_sched_val_correct(argv[ARG_COUNT - 1], sched_values))
		terminate_program("Not correct scheduler");
	i = 0;
	while (i < argc - 2)
	{
		args[i] = strict_atoi(argv[i + 1]);
		i++;
	}
	return (create_config(args, argv[ARG_COUNT - 1]));
}
