/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time_counter.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/18 00:05:33 by danborys          #+#    #+#             */
/*   Updated: 2026/04/18 00:17:05 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long get_current_time(void)
{
	struct timeval t;
	long long curr_time;

	gettimeofday(&t, NULL);
	curr_time = (long long)(t.tv_sec * 1000) + (t.tv_usec / 1000);
	return (curr_time);
}

struct timespec get_abs_time(long long wake_up_time)
{
	struct timespec ts;

	ts.tv_sec = wake_up_time / 1000;
	ts.tv_nsec = (wake_up_time % 1000) * 1000000;
	return	(ts);
}