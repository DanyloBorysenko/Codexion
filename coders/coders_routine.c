/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders_routine.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 12:27:48 by danborys          #+#    #+#             */
/*   Updated: 2026/05/01 16:25:25 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	work(t_coder *coder, long long end_time, struct timespec *ts);
int	refact(t_coder *coder);
int	debug(t_coder *coder);
int	compile(t_coder *coder);

static void	get_dongle_order(t_coder *coder, t_dongle **first, t_dongle **sec)
{
	if (coder->id % 2 == 0)
	{
		*first = coder->right_dng;
		*sec = coder->left_dng;
	}
	else
	{
		*first = coder->left_dng;
		*sec = coder->right_dng;
	}
}

static void	insert_req(t_req req, t_dongle *first, t_dongle *sec)
{
	pthread_mutex_lock(&first->lock);
	pthread_mutex_lock(&sec->lock);
	heap_insert(first->heap, req);
	heap_insert(sec->heap, req);
	pthread_mutex_unlock(&sec->lock);
	pthread_mutex_unlock(&first->lock);
}

void	*coder_rout(void *arg)
{
	t_coder		*coder;
	t_req		request;
	t_dongle	*first;
	t_dongle	*second;

	coder = (t_coder *)arg;
	get_dongle_order(coder, &first, &second);
	if (coder->left_dng == coder->right_dng)
		return (NULL);
	while (1)
	{
		request.cod_id = coder->id;
		request.arr_t = get_cur_time();
		request.deadl = coder->last_compile_time + coder->time_to_burnout;
		request.coder = coder;
		insert_req(request, first, second);
		if (!take_dongles(first, second, coder))
			break ;
		if (!compile(coder) || !debug(coder) || !refact(coder))
			break ;
	}
	return (NULL);
}
