/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time_counter.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 15:26:40 by danborys          #+#    #+#             */
/*   Updated: 2026/03/28 15:27:24 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	get_current_time(struct timeval *tv)
{
	gettimeofday(tv, NULL);
	return ((long long)(tv->tv_sec * 1000) + (tv->tv_usec / 1000));
}
