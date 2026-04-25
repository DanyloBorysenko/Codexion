/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   work.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 14:14:20 by danborys          #+#    #+#             */
/*   Updated: 2026/04/25 20:14:29 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void wake_up_all(int count, coder_t	*coders, dongle_t *don)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_mutex_lock(&coders[i].coder_lock);
		coders[i].alive = 0;
		pthread_cond_signal(&coders[i].cond);
		pthread_mutex_unlock(&coders[i].coder_lock);
		pthread_mutex_lock(&don[i].lock);
		pthread_cond_broadcast(&don[i].cond);
		pthread_mutex_unlock(&don[i].lock);
		i++;
	}
}

void *monitor_routine(void *arg)
{
	monitor_t *mon;
	long long now;
	long long last;
	int stop;
	int i;

	mon = (monitor_t*)arg;
	stop = 0;
	while (stop == 0)
	{
		pthread_mutex_lock(&mon->simul->sim_lock);
		if (mon->simul->finished_coders == mon->config->number_of_coders)
		{
			pthread_mutex_unlock(&mon->simul->sim_lock);
			wake_up_all(mon->config->number_of_coders, mon->coders, mon->dongles);
			printf("Finished = all\n");
			break;
		}
		pthread_mutex_unlock(&mon->simul->sim_lock);
		i = 0;
		while (i < mon->config->number_of_coders)
		{

			now = get_current_time();
			pthread_mutex_lock(&mon->coders[i].coder_lock);
			last = mon->coders[i].last_compile_time;
			pthread_mutex_unlock(&mon->coders[i].coder_lock);
			if (now - last > mon->config->time_to_burnout)
			{
				stop = 1;
				log_event(mon->simul, (mon->coders)[i].id, "burned out", now);
				wake_up_all(mon->config->number_of_coders, mon->coders, mon->dongles);
				break;
			}
			i++;
		}
		if (stop == 1)
			break;
		usleep(1000);
	}
	return (NULL);
}

int work(coder_t *coder, long long end_time, struct timespec *ts)
{
	long long	current_time;

	pthread_mutex_lock(&coder->coder_lock);
	while (coder->alive)
	{
		current_time = get_current_time();
		if (current_time >= end_time)
			break;
		pthread_cond_timedwait(&coder->cond, &coder->coder_lock, ts);
	}
	if (!coder->alive)
	{
		pthread_mutex_unlock(&coder->coder_lock);
		return (0);
	}
	pthread_mutex_unlock(&coder->coder_lock);
	return (1);
}

int refact(coder_t *coder)
{
	struct timespec ts;
	long long current_time;
	long long end_time;

	current_time = get_current_time();
	end_time = current_time + coder->config->time_to_refactor;
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
	end_time = current_time + coder->config->time_to_debug;
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
	end_time = current_time + coder->config->time_to_compile;
	ts = get_abs_time(end_time);
	pthread_mutex_lock(&coder->coder_lock);
	coder->last_compile_time = current_time;
	pthread_mutex_unlock(&coder->coder_lock);
	log_event(coder->simul, coder->id, "is compiling", current_time);
	if (!work(coder, end_time, &ts))
		return (0);
	release_dongles(coder->left_dng, coder->right_dng);
	coder->compiles_done++;
	if (coder->compiles_done == coder->config->number_of_compiles_required)
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
	pthread_mutex_lock(&second->lock);
	heap_insert(first->heap, req);
	heap_insert(second->heap, req);
	pthread_mutex_unlock(&second->lock);
	pthread_mutex_unlock(&first->lock);
}

int	acquire_dongles(coder_t *coder)
{
	dongle_t	*first;
	dongle_t	*second;
	struct timespec ts;

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
	while (coder->alive)
	{
		if (first->heap->reqs[0].coder_id == coder->id && !first->in_use && get_current_time() >= first->release)
			break;
		if (first->heap->reqs[0].coder_id == coder->id && !first->in_use)
		{
			ts = get_abs_time(first->release);
			pthread_cond_timedwait(&first->cond, &first->lock, &ts);
		}
		else
			pthread_cond_wait(&first->cond, &first->lock);
	}
	if (!coder->alive)
	{
		pthread_mutex_unlock(&first->lock);
		return (0);
	}
	first->in_use = 1;
	log_event(coder->simul, coder->id, "has taken a dongle", get_current_time());
	pthread_mutex_unlock(&first->lock);

	pthread_mutex_lock(&second->lock);
	while (coder->alive)
	{
		if (second->heap->reqs[0].coder_id == coder->id && !second->in_use && get_current_time() >= second->release)
			break;
		if (second->heap->reqs[0].coder_id == coder->id && !second->in_use)
		{
			ts = get_abs_time(second->release);
			pthread_cond_timedwait(&second->cond, &second->lock, &ts);
		}
		else
			pthread_cond_wait(&second->cond, &second->lock);
	}
	if (!coder->alive)
	{
		pthread_mutex_unlock(&second->lock);
		return (0);
	}
	second->in_use = 1;
	log_event(coder->simul, coder->id, "has taken a dongle", get_current_time());
	pthread_mutex_unlock(&second->lock);
	return (1);
}

void *coders_routine(void *arg)
{
	coder_t *coder;
	req_t request;
	long long now;

	coder = (coder_t *)arg;
	while (1)
	{
		now = get_current_time();
		request.coder_id = coder->id;
		request.arr_time = now;
		request.deadline = coder->last_compile_time + coder->config->time_to_burnout;
		insert_req(coder, request);
		if (!acquire_dongles(coder))
			break;
		if (!compile(coder))
			break;
		if (!debug(coder))
			break;
		if (!refact(coder))
			break;
	}
	return (NULL);
}

int	check_dongles(dongle_t *d1, dongle_t *d2)
{
	long long now;
	int			res;

	if (d1 == d2)
		return (-1);
	if (d1->in_use == 0 && d2->in_use == 0)
	{
		now = get_current_time();
		res = now >= d1->release && now >= d2->release;
		return (res);
	}
	return (0);
}

void start_to_work(t_config *config, simul_t *simul)
{
	dongle_t *dongles;
	coder_t *coders;
	shared_arg_t shared_arg;
	monitor_t *mon;
	int i;

	dongles = init_dongles(config->number_of_coders, config->dongle_cooldown, config->scheduler);
	shared_arg.conf = config;
	shared_arg.dngls = dongles;
	shared_arg.sim = simul;
	coders = init_coders(shared_arg);
	mon = init_monitor(config, simul, coders, dongles);
	i = 0;
	while (i < config->number_of_coders)
	{
		pthread_create(&coders[i].thread_id, NULL, coders_routine, &coders[i]);
		i++;
	}
	pthread_create(&mon->thread_id, NULL, monitor_routine, mon);
	pthread_join(mon->thread_id, NULL);
	i = 0;
	while (i < config->number_of_coders)
	{
		pthread_join(coders[i].thread_id, NULL);
		i++;
	}
	destroy_coders(coders, config->number_of_coders);
	destroy_dongles(dongles, config->number_of_coders);
	free(mon);
}
