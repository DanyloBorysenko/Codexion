/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders_logic.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 12:27:48 by danborys          #+#    #+#             */
/*   Updated: 2026/04/28 10:50:49 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int work(coder_t *coder, long long end_time, struct timespec *ts)
{
	long long	current_time;

	pthread_mutex_lock(&coder->simul->sim_lock);
	while (!coder->simul->is_finished)
	{
		current_time = get_current_time();
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

	current_time = get_current_time();
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

	current_time = get_current_time();
	end_time = current_time + coder->time_to_debug;
	ts = get_abs_time(end_time);
	log_event(coder->simul, coder->id, "is debugging", current_time);
	return (work(coder, end_time, &ts));
}

void	release_dongles(dongle_t *left, dongle_t *right)
{
	pthread_mutex_lock(&left->lock);
	left->in_use = 0;
	left->release = get_current_time() + left->cooldown;
	heap_extract(left->heap, 0);
	pthread_cond_broadcast(&left->cond);
	pthread_mutex_unlock(&left->lock);
	pthread_mutex_lock(&right->lock);
	right->in_use = 0;
	right->release = get_current_time() + right->cooldown;
	heap_extract(right->heap, 0);
	pthread_cond_broadcast(&right->cond);
	pthread_mutex_unlock(&right->lock);
}

int compile(coder_t *coder)
{
	struct timespec ts;
	long long current_time;
	long long end_time;

	current_time = get_current_time();
	end_time = current_time + coder->time_to_compile;
	ts = get_abs_time(end_time);
	pthread_mutex_lock(&coder->coder_lock);
	coder->last_compile_time = current_time;
	pthread_mutex_unlock(&coder->coder_lock);
	log_event(coder->simul, coder->id, "is compiling", current_time);
	if (!work(coder, end_time, &ts))
		return (0);
	release_dongles(coder->left_dng, coder->right_dng);
	coder->compiles_done++;
	if (coder->compiles_done == coder->num_of_comp_req)
	{
		pthread_mutex_lock(&coder->simul->sim_lock);
		coder->simul->finished_coders = coder->simul->finished_coders + 1;
		pthread_mutex_unlock(&coder->simul->sim_lock);
	}
	return (1);
}

void insert_req(coder_t *coder, req_t req)
{
	dongle_t *first;
	dongle_t *second;

	if (coder->id % 2 == 0)
	{
		first = coder->right_dng;
		second = coder->left_dng;
	}
	else
	{
		first = coder->left_dng;
		second = coder->right_dng;
	}
	pthread_mutex_lock(&first->lock);
	heap_insert(first->heap, req);
	pthread_mutex_unlock(&first->lock);
	pthread_mutex_lock(&second->lock);
	// printf("INSERTed req, dongle %d, coder id %d, deadline %llu, arrivaltime %llu\n", first->num, req.coder_id, req.deadline, req.arr_time);
	heap_insert(second->heap, req);
	// printf("INSERTed req, dongle %d, coder id %d, deadline %llu, arrivaltime %llu\n", second->num, req.coder_id, req.deadline, req.arr_time);
	pthread_mutex_unlock(&second->lock);
}

int take_dongle(coder_t *coder, dongle_t *don)
{
	struct timespec ts;

	pthread_mutex_lock(&don->lock);
	while (!is_simul_finished(coder->simul))
	{
		if (don->heap->size == 0)
		{
			pthread_mutex_unlock(&don->lock);
			return (0);
		}
		if (don->heap->reqs[0].coder_id == coder->id && !don->in_use && get_current_time() >= don->release)
			break;
		if (don->heap->reqs[0].coder_id == coder->id && !don->in_use)
		{
			ts = get_abs_time(don->release);
			pthread_cond_timedwait(&don->cond, &don->lock, &ts);
		}
		else
			pthread_cond_wait(&don->cond, &don->lock);
	}
	if (is_simul_finished(coder->simul))
	{
		pthread_mutex_unlock(&don->lock);
		return (0);
	}
	don->in_use = 1;
	log_event(coder->simul, coder->id, "has taken a dongle", get_current_time());
	pthread_mutex_unlock(&don->lock);
	return (1);
}

int	acquire_dongles(coder_t *coder)
{
	dongle_t	*first;
	dongle_t	*second;

	if (coder->id % 2 == 0)
	{
		first = coder->right_dng;
		second = coder->left_dng;
	}
	else
	{
		first = coder->left_dng;
		second = coder->right_dng;
	}
	if (!take_dongle(coder, first))
		return (0);
	if (!take_dongle(coder, second))
		return (0);
	return (1);
}

void *coder_routine(void *arg)
{
	coder_t *coder;
	req_t request;

	coder = (coder_t *)arg;
	if (coder->left_dng == coder->right_dng)
			return (NULL);
	while (1)
	{
		request.coder_id = coder->id;
		request.arr_time = get_current_time();
		request.deadline = coder->last_compile_time + coder->time_to_burnout;
		insert_req(coder, request);
		if (!acquire_dongles(coder))
			break;
		if (!compile(coder) || !debug(coder) || !refact(coder))
			break;
	}
	return (NULL);
}

int	launch_coders(coder_t *coders, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		if (pthread_create(&coders[i].thread_id, NULL, coder_routine, &coders[i]))
			return (i);
		i++;
	}
	return (count);
}
