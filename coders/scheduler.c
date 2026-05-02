/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/01 16:05:48 by danborys          #+#    #+#             */
/*   Updated: 2026/05/02 14:07:55 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	get_state(t_coder *c)
{
	if (
		c->left_dng->heap->reqs[0].cod_id == c->id
		&& c->right_dng->heap->reqs[0].cod_id == c->id
		&& !c->left_dng->in_use && !c->right_dng->in_use
		&& get_cur_time() > c->left_dng->release
		&& get_cur_time() > c->right_dng->release
	)
		return (2);
	if (
		c->left_dng->heap->reqs[0].cod_id == c->id
		&& c->right_dng->heap->reqs[0].cod_id == c->id
		&& !c->left_dng->in_use && !c->right_dng->in_use
	)
		return (1);
	return (0);
}

static int	do_take(t_coder *c)
{
	c->left_dng->in_use = 1;
	log_event(c->sim, c->id, "has taken a dongle", get_cur_time());
	heap_extract(c->left_dng->heap, 0);
	c->right_dng->in_use = 1;
	log_event(c->sim, c->id, "has taken a dongle", get_cur_time());
	heap_extract(c->right_dng->heap, 0);
	pthread_mutex_unlock(&c->sim->sched_lock);
	return (1);
}

static void	wait_cooldown(t_coder *c)
{
	long long		max_release;
	struct timespec	wake_up;

	if (c->left_dng->release > c->right_dng->release)
		max_release = c->left_dng->release;
	else
		max_release = c->right_dng->release;
	wake_up = get_abs_time(max_release);
	pthread_cond_timedwait(&c->sim->sched_cond,
		&c->sim->sched_lock, &wake_up);
}

int	take_dongles(t_coder *coder)
{
	int	state;

	pthread_mutex_lock(&coder->sim->sched_lock);
	while (!is_simul_finished(coder->sim))
	{
		if (!coder->left_dng->heap->size
			|| !coder->right_dng->heap->size)
			return (0);
		state = get_state(coder);
		if (state == 2)
			return (do_take(coder));
		if (state == 1)
			wait_cooldown(coder);
		else
			pthread_cond_wait(&coder->sim->sched_cond, &coder->sim->sched_lock);
	}
	pthread_mutex_unlock(&coder->sim->sched_lock);
	return (0);
}
