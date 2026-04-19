/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   work.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 14:14:20 by danborys          #+#    #+#             */
/*   Updated: 2026/04/19 10:51:52 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void wake_up_all(int count, coder_t	*coders, scheduler_t *sched)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_mutex_lock(&coders[i].coder_lock);
		coders[i].alive = 0;
		pthread_cond_signal(&coders[i].cond);
		pthread_mutex_unlock(&coders[i].coder_lock);
		pthread_mutex_lock(&sched->lock);
		sched->alive = 0;
		pthread_cond_signal(&sched->cond);
		pthread_mutex_unlock(&sched->lock);
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
			wake_up_all(mon->config->number_of_coders, mon->coders, mon->sched);
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
				log_event(mon->simul, (mon->coders)[i].id, "burned out", now - mon->simul->start);
				wake_up_all(mon->config->number_of_coders, mon->coders, mon->sched);
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

int work(coder_t *coder, long long end_time,struct timespec *ts)
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

int compile(coder_t *coder)
{
	struct timespec ts;
	long long current_time;
	long long end_time;

	current_time = get_current_time();
	end_time = current_time + coder->config->time_to_compile;
	ts = get_abs_time(end_time);
	log_event(coder->simul, coder->id, "is compiling", current_time);
	pthread_mutex_lock(&coder->coder_lock);
	coder->last_compile_time = current_time;
	pthread_mutex_unlock(&coder->coder_lock);
	if (!work(coder, end_time, &ts))
		return (0);
	coder->compiles_done++;
	if (coder->compiles_done == coder->config->number_of_compiles_required)
	{
		pthread_mutex_lock(&coder->simul->sim_lock);
		coder->simul->finished_coders = coder->simul->finished_coders + 1;
		pthread_mutex_unlock(&coder->simul->sim_lock);
	}
	return (1);
}

// void print_req(req_t *req)
// {
// 	if (req == NULL)
// 		printf("NULL");
// 	else
// 		printf("'coder_id=%d, arr_time=%llu, deadline=%llu'", req->coder->id, req->arr_time, req->deadline);
// }

// void print_heap(heap_t *heap)
// {
// 	int i;

// 	i = 0;
// 	while (i < heap->size)
// 	{
// 		print_req(&(heap->reqs)[i]);
// 		i++;
// 	}
// 	printf("\n");
// }

void *coders_routine(void *arg)
{
	coder_t *coder;
	coder = (coder_t *)arg;
	req_t request;
	long long now;
	while (1)
	{
		now = get_current_time();
		request.coder = coder;
		request.arr_time = now;
		request.deadline = now + coder->config->time_to_burnout;
		// pthread_mutex_lock(&coder->simul->print_lock);
		// printf("Request Created: coder id %d, arr time %llu, deadline %llu\n", coder->id, request.arr_time, request.deadline);
		// pthread_mutex_unlock(&coder->simul->print_lock);
		heap_insert(coder->heap, request);
		pthread_mutex_lock(&coder->sched->lock);
		coder->sched->called = 1;
		pthread_cond_signal(&coder->sched->cond);
		pthread_mutex_unlock(&coder->sched->lock);
		pthread_mutex_lock(&coder->coder_lock);
		while (coder->perm == 0 && coder->alive)
			pthread_cond_wait(&coder->cond, &coder->coder_lock);
		if (!coder->alive)
		{
			pthread_mutex_unlock(&coder->coder_lock);
			break;
		}
		coder->perm = 0;
		pthread_mutex_unlock(&coder->coder_lock);
		if (!compile(coder))
			break;
		if (!debug(coder))
			break;
		if (!refact(coder))
			break;
	}
	return (NULL);
}

void *sched_routine(void *arg)
{
	scheduler_t *sched;
	int			i;
	coder_t		*coder;

	sched = (scheduler_t *)arg;
	while (1)
	{
		pthread_mutex_lock(&sched->lock);
		while (sched->alive && sched->called == 0)
			pthread_cond_wait(&sched->cond, &sched->lock);
		if (!sched->alive)
		{
			pthread_mutex_unlock(&sched->lock);
			break;
		}
		i = 0;
		pthread_mutex_lock(&sched->heap->lock);
		while (i < sched->heap->size)
		{
			coder = sched->heap->reqs[i].coder;
			pthread_mutex_lock(&coder->coder_lock);
			coder->perm = 1;
			pthread_cond_signal(&coder->cond);
			pthread_mutex_unlock(&coder->coder_lock);
			i++;
		}
		pthread_mutex_unlock(&sched->heap->lock);
		sched->called = 0;
		pthread_mutex_unlock(&sched->lock);
	}
	return (NULL);
}

void start_to_work(t_config *config, simul_t *simul)
{
	heap_t *heap;
	dongle_t *dongles;
	coder_t *coders;
	shared_arg_t shared_arg;
	scheduler_t	*sched;
	monitor_t *mon;
	int i;

	heap = init_heap(config);
	if (!heap)
		return;
	dongles = init_dongles(config->number_of_coders);
	sched = init_sched(heap);
	shared_arg.conf = config;
	shared_arg.dngls = dongles;
	shared_arg.heap = heap;
	shared_arg.sim = simul;
	shared_arg.sched = sched;
	coders = init_coders(shared_arg);
	mon = init_monitor(config, simul, coders, sched);
	i = 0;
	while (i < config->number_of_coders)
	{
		pthread_create(&coders[i].thread_id, NULL, coders_routine, &coders[i]);
		i++;
	}
	pthread_create(&mon->thread_id, NULL, monitor_routine, mon);
	pthread_create(&sched->thread_id, NULL, sched_routine, sched);
	pthread_join(mon->thread_id, NULL);
	pthread_join(sched->thread_id, NULL);
	i = 0;
	while (i < config->number_of_coders)
	{
		pthread_join(coders[i].thread_id, NULL);
		i++;
	}
	destroy_coders(coders, config->number_of_coders);
	destroy_dongles(dongles, config->number_of_coders);
	free(mon);
	destroy_heap(heap);
	destroy_sched(sched);
}
