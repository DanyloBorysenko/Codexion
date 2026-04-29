/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders_logic.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 12:27:48 by danborys          #+#    #+#             */
/*   Updated: 2026/04/29 21:39:28 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	work(coder_t *coder, long long end_time, struct timespec *ts);
int	refact(coder_t *coder);
int	debug(coder_t *coder);
int	compile(coder_t *coder);

static void	get_dongle_order(coder_t *coder, dongle_t **first, dongle_t **sec)
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

static void	insert_req(req_t req, dongle_t *first, dongle_t *sec)
{
	pthread_mutex_lock(&first->lock);
	pthread_mutex_lock(&sec->lock);
	heap_insert(first->heap, req);
	// printf("INSERTed req, dongle %d, coder id %d, deadline %llu, arrivaltime %llu\n", first->num, req.coder_id, req.deadline, req.arr_time);
	heap_insert(sec->heap, req);
	// printf("INSERTed req, dongle %d, coder id %d, deadline %llu, arrivaltime %llu\n", second->num, req.coder_id, req.deadline, req.arr_time);
	pthread_mutex_unlock(&sec->lock);
	pthread_mutex_unlock(&first->lock);
}

int	take_dongles(dongle_t *d1, dongle_t *d2, coder_t *coder)
{
	long long	max_release;
	struct timespec	wake_up;

	while (!coder->simul->is_finished)
	{
		pthread_mutex_lock(&d1->lock);
		pthread_mutex_lock(&d2->lock);
		if (d1->heap->reqs[0].coder_id == coder->id &&
			d2->heap->reqs[0].coder_id == coder->id &&
			!d1->in_use && !d2->in_use &&
			get_cur_time() > d1->release && get_cur_time() > d2->release)
		{
			d1->in_use = 1;
			d2->in_use = 1;
			pthread_mutex_unlock(&d2->lock);
			pthread_mutex_unlock(&d1->lock);
			return (1);
		}
		if (d1->heap->reqs[0].coder_id == coder->id &&
			d2->heap->reqs[0].coder_id == coder->id &&
			!d1->in_use && !d2->in_use)
		{
			if (d1->release > d2->release)
				max_release = d1->release;
			else
				max_release = d2->release;
			wake_up = get_abs_time(max_release);
			pthread_mutex_lock(&coder->lock);
			pthread_mutex_unlock(&d2->lock);
			pthread_mutex_unlock(&d1->lock);
			pthread_cond_timedwait(&coder->cond, &coder->lock, &wake_up);
			pthread_mutex_unlock(&coder->lock);
		}
		else
		{
			pthread_mutex_lock(&coder->lock);
			pthread_mutex_unlock(&d2->lock);
			pthread_mutex_unlock(&d1->lock);
			pthread_cond_wait(&coder->cond, &coder->lock);
			pthread_mutex_unlock(&coder->lock);
		}
	}
	return (0);
}

void	*coder_rout(void *arg)
{
	coder_t		*coder;
	req_t		request;
	dongle_t	*first;
	dongle_t	*second;

	coder = (coder_t *)arg;
	get_dongle_order(coder, &first, &second);
	if (coder->left_dng == coder->right_dng)
		return (NULL);
	while (1)
	{
		request.coder_id = coder->id;
		request.arr_time = get_cur_time();
		request.deadline = coder->last_compile_time + coder->time_to_burnout;
		request.coder = coder;
		insert_req(request, first, second);
		if (!take_dongles(first, second, coder))
			break;
		if (!compile(coder) || !debug(coder) || !refact(coder))
			break ;
	}
	return (NULL);
}
