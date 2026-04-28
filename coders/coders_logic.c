/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders_logic.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 12:27:48 by danborys          #+#    #+#             */
/*   Updated: 2026/04/28 15:03:02 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int work(coder_t *coder, long long end_time, struct timespec *ts);
int refact(coder_t *coder);
int debug(coder_t *coder);
int compile(coder_t *coder);

static void get_dongle_order(coder_t *coder, dongle_t **first, dongle_t **sec)
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

static void insert_req(coder_t *coder, req_t req)
{
	dongle_t *first;
	dongle_t *second;

	get_dongle_order(coder, &first, &second);
	pthread_mutex_lock(&first->lock);
	pthread_mutex_lock(&second->lock);
	heap_insert(first->heap, req);
	// printf("INSERTed req, dongle %d, coder id %d, deadline %llu, arrivaltime %llu\n", first->num, req.coder_id, req.deadline, req.arr_time);
	heap_insert(second->heap, req);
	// printf("INSERTed req, dongle %d, coder id %d, deadline %llu, arrivaltime %llu\n", second->num, req.coder_id, req.deadline, req.arr_time);
	pthread_mutex_unlock(&second->lock);
	pthread_mutex_unlock(&first->lock);
}

int take_dongle(coder_t *coder, dongle_t *don)
{
	struct timespec ts;

	pthread_mutex_lock(&don->lock);
	while (!is_simul_finished(coder->simul))
	{
		if (don->heap->size == 0)
		{
			pthread_cond_wait(&don->cond, &don->lock);
			continue;
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
	dongle_t *first;
	dongle_t *second;

	get_dongle_order(coder, &first, &second);
	if (!take_dongle(coder, first))
		return (0);
	if (!take_dongle(coder, second))
		return (0);
	return (1);
}

void *coder_rout(void *arg)
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
