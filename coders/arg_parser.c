/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arg_parser.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 15:38:48 by danborys          #+#    #+#             */
/*   Updated: 2026/04/26 18:09:43 by danborys         ###   ########.fr       */
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

void	parse_arg(int argc, char **argv, char **sched_vals, t_config *conf)
{
	if (argc != ARG_COUNT)
	{
		printf("Args count: expected - %d, actual - %d\n", ARG_COUNT, argc);
		exit(EXIT_FAILURE);
	}
	if (!is_sched_val_correct(argv[ARG_COUNT - 1], sched_vals))
		terminate_program("Not correct scheduler");
	conf->number_of_coders = strict_atoi(argv[ARGV_CODERS]);
    conf->time_to_burnout = strict_atoi(argv[ARGV_BURNOUT]);
    conf->time_to_compile = strict_atoi(argv[ARGV_COMPILE]);
    conf->time_to_debug = strict_atoi(argv[ARGV_DEBUG]);
    conf->time_to_refactor = strict_atoi(argv[ARGV_REFACTOR]);
    conf->number_of_compiles_required = strict_atoi(argv[ARGV_COMP_REQ]);
    conf->dongle_cooldown = strict_atoi(argv[ARGV_COOLDOWN]);
    conf->scheduler = argv[ARGV_SCHED];
}
