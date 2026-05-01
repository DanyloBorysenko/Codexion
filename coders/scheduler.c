/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/01 16:05:48 by danborys          #+#    #+#             */
/*   Updated: 2026/05/01 16:25:02 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	get_state(t_dongle *d1, t_dongle *d2, t_coder *c)
{
	if (
		d1->heap->reqs[0].cod_id == c->id
		&& d2->heap->reqs[0].cod_id == c->id
		&& !d1->in_use && !d2->in_use
		&& get_cur_time() > d1->release
		&& get_cur_time() > d2->release
	)
		return (2);
	if (
		d1->heap->reqs[0].cod_id == c->id
		&& d2->heap->reqs[0].cod_id == c->id
		&& !d1->in_use && !d2->in_use
	)
		return (1);
	return (0);
}

static int	do_take(t_dongle *d1, t_dongle *d2, t_coder *c)
{
	d1->in_use = 1;
	d2->in_use = 1;
	heap_extract(d1->heap, 0);
	heap_extract(d2->heap, 0);
	pthread_mutex_unlock(&d2->lock);
	pthread_mutex_unlock(&d1->lock);
	pthread_mutex_unlock(&c->simul->sched_lock);
	return (1);
}

static void	wait_cooldown(t_dongle *d1, t_dongle *d2, t_coder *c)
{
	long long		max_release;
	struct timespec	wake_up;

	if (d1->release > d2->release)
		max_release = d1->release;
	else
		max_release = d2->release;
	pthread_mutex_unlock(&d2->lock);
	pthread_mutex_unlock(&d1->lock);
	wake_up = get_abs_time(max_release);
	pthread_cond_timedwait(&c->simul->sched_cond,
		&c->simul->sched_lock, &wake_up);
}

static void	wait_signal(t_dongle *d1, t_dongle *d2, t_coder *c)
{
	pthread_mutex_unlock(&d2->lock);
	pthread_mutex_unlock(&d1->lock);
	pthread_cond_wait(&c->simul->sched_cond, &c->simul->sched_lock);
}

int	take_dongles(t_dongle *d1, t_dongle *d2, t_coder *coder)
{
	int	state;

	while (!coder->simul->is_finished)
	{
		pthread_mutex_lock(&coder->simul->sched_lock);
		pthread_mutex_lock(&d1->lock);
		pthread_mutex_lock(&d2->lock);
		state = get_state(d1, d2, coder);
		if (state == 2)
			return (do_take(d1, d2, coder));
		if (state == 1)
			wait_cooldown(d1, d2, coder);
		else
			wait_signal(d1, d2, coder);
		pthread_mutex_unlock(&coder->simul->sched_lock);
	}
	return (0);
}
