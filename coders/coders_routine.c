/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders_routine.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 12:27:48 by danborys          #+#    #+#             */
/*   Updated: 2026/05/02 16:11:32 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	work(t_coder *coder, long long end_time, struct timespec *ts);
int	refact(t_coder *coder);
int	debug(t_coder *coder);
int	compile(t_coder *coder);

static void	insert_req(t_coder *c)
{
	t_req	request;

	request.cod_id = c->id;
	request.arr_t = get_cur_time();
	request.deadl = c->last_compile_time + c->time_to_burnout;
	heap_insert(c->left_dng->heap, request);
	heap_insert(c->right_dng->heap, request);
}

void	*coder_rout(void *arg)
{
	t_coder		*coder;

	coder = (t_coder *)arg;
	if (coder->left_dng == coder->right_dng)
		return (NULL);
	while (1)
	{
		pthread_mutex_lock(&coder->sim->sched_lock);
		insert_req(coder);
		pthread_mutex_unlock(&coder->sim->sched_lock);
		if (!take_dongles(coder))
			break ;
		if (!compile(coder) || !debug(coder) || !refact(coder))
			break ;
	}
	return (NULL);
}
