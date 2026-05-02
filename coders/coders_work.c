/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders_work.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 14:21:20 by danborys          #+#    #+#             */
/*   Updated: 2026/05/02 09:53:41 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	work(t_coder *coder, long long end_time, struct timespec *ts)
{
	long long	current_time;

	pthread_mutex_lock(&coder->sim->lock);
	while (!coder->sim->is_finished)
	{
		current_time = get_cur_time();
		if (current_time >= end_time)
			break ;
		pthread_cond_timedwait(&coder->sim->cond, &coder->sim->lock, ts);
	}
	if (coder->sim->is_finished)
	{
		pthread_mutex_unlock(&coder->sim->lock);
		return (0);
	}
	pthread_mutex_unlock(&coder->sim->lock);
	return (1);
}

int	refact(t_coder *coder)
{
	struct timespec	ts;
	long long		current_time;
	long long		end_time;

	current_time = get_cur_time();
	end_time = current_time + coder->time_to_refactor;
	ts = get_abs_time(end_time);
	log_event(coder->sim, coder->id, "is refactoring", current_time);
	return (work(coder, end_time, &ts));
}

int	debug(t_coder *coder)
{
	struct timespec	ts;
	long long		current_time;
	long long		end_time;

	current_time = get_cur_time();
	end_time = current_time + coder->time_to_debug;
	ts = get_abs_time(end_time);
	log_event(coder->sim, coder->id, "is debugging", current_time);
	return (work(coder, end_time, &ts));
}

void	release_dongles(t_dongle *left, t_dongle *right, t_simul *sim)
{
	pthread_mutex_lock(&sim->sched_lock);
	left->in_use = 0;
	left->release = get_cur_time() + left->cooldown;
	right->in_use = 0;
	right->release = get_cur_time() + right->cooldown;
	pthread_cond_broadcast(&sim->sched_cond);
	pthread_mutex_unlock(&sim->sched_lock);
}

int	compile(t_coder *coder)
{
	struct timespec	ts;
	long long		current_time;
	long long		end_time;

	current_time = get_cur_time();
	end_time = current_time + coder->time_to_compile;
	ts = get_abs_time(end_time);
	pthread_mutex_lock(&coder->lock);
	coder->last_compile_time = current_time;
	pthread_mutex_unlock(&coder->lock);
	log_event(coder->sim, coder->id, "is compiling", current_time);
	if (!work(coder, end_time, &ts))
		return (0);
	release_dongles(coder->left_dng, coder->right_dng, coder->sim);
	coder->compiles_done++;
	if (coder->compiles_done == coder->num_of_comp_req)
	{
		pthread_mutex_lock(&coder->sim->lock);
		coder->sim->finished_coders = coder->sim->finished_coders + 1;
		pthread_mutex_unlock(&coder->sim->lock);
	}
	return (1);
}
