/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders_work.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 14:21:20 by danborys          #+#    #+#             */
/*   Updated: 2026/05/01 12:39:22 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int work(coder_t *coder, long long end_time, struct timespec *ts)
{
	long long	current_time;

	pthread_mutex_lock(&coder->simul->sim_lock);
	while (!coder->simul->is_finished)
	{
		current_time = get_cur_time();
		if (current_time >= end_time)
			break;
		pthread_cond_timedwait(&coder->simul->cond, &coder->simul->sim_lock, ts);
	}
	if (coder->simul->is_finished)
	{
		pthread_mutex_unlock(&coder->simul->sim_lock);
		return (0);
	}
	pthread_mutex_unlock(&coder->simul->sim_lock);
	return (1);
}

int refact(coder_t *coder)
{
	struct timespec ts;
	long long current_time;
	long long end_time;

	current_time = get_cur_time();
	end_time = current_time + coder->time_to_refactor;
	ts = get_abs_time(end_time);
	log_event(coder->simul, coder->id, "is refactoring", current_time);
	return (work(coder, end_time, &ts));
}

int debug(coder_t *coder)
{
	struct timespec ts;
	long long current_time;
	long long end_time;

	current_time = get_cur_time();
	end_time = current_time + coder->time_to_debug;
	ts = get_abs_time(end_time);
	log_event(coder->simul, coder->id, "is debugging", current_time);
	return (work(coder, end_time, &ts));
}

void	release_dongles(dongle_t *left, dongle_t *right, simul_t *sim)
{
	pthread_mutex_lock(&sim->sched_lock);
	pthread_mutex_lock(&left->lock);
	left->in_use = 0;
	left->release = get_cur_time() + left->cooldown;
	pthread_mutex_unlock(&left->lock);
	pthread_mutex_lock(&right->lock);
	right->in_use = 0;
	right->release = get_cur_time() + right->cooldown;
	pthread_mutex_unlock(&right->lock);
	pthread_cond_broadcast(&sim->sched_cond);
	pthread_mutex_unlock(&sim->sched_lock);
}

int compile(coder_t *coder)
{
	struct timespec ts;
	long long current_time;
	long long end_time;

	current_time = get_cur_time();
	end_time = current_time + coder->time_to_compile;
	ts = get_abs_time(end_time);
	pthread_mutex_lock(&coder->lock);
	coder->last_compile_time = current_time;
	pthread_mutex_unlock(&coder->lock);
	log_event(coder->simul, coder->id, "is compiling", current_time);
	if (!work(coder, end_time, &ts))
		return (0);
	release_dongles(coder->left_dng, coder->right_dng, coder->simul);
	coder->compiles_done++;
	if (coder->compiles_done == coder->num_of_comp_req)
	{
		pthread_mutex_lock(&coder->simul->sim_lock);
		coder->simul->finished_coders = coder->simul->finished_coders + 1;
		pthread_mutex_unlock(&coder->simul->sim_lock);
	}
	return (1);
}
