/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   work.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 14:14:20 by danborys          #+#    #+#             */
/*   Updated: 2026/04/13 18:03:16 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void *monitor_routine(void *arg)
{
	monitor_arg_t *m_arg;
	struct timeval t;
	long long current_time;
	long long burn_out_time;
	int	stop;
	int	i;

	m_arg = (monitor_arg_t *)arg;
	stop = 0;
	while (stop == 0)
	{
		pthread_mutex_lock(m_arg->simul_lock);
		if (m_arg->simul->finished_coders == m_arg->config->number_of_coders)
		{
			m_arg->simul->is_simul_alive = 0;
			pthread_mutex_unlock(m_arg->simul_lock);
			printf("Finished = all\n");
			break;
		}
		pthread_mutex_unlock(m_arg->simul_lock);
		i = 0;
		while (i < m_arg->config->number_of_coders)
		{

			current_time =  get_current_time(&t);
			pthread_mutex_lock(&m_arg->coders[i].coder_lock);
			burn_out_time = m_arg->coders[i].burn_out_time;
			pthread_mutex_unlock(&m_arg->coders[i].coder_lock);
			if (current_time > burn_out_time)
			{
				stop = 1;
				log_event(&(m_arg->coders)[i].locks->print_lock, (m_arg->coders)[i].id, "burned out", current_time - m_arg->simul->start);
				pthread_mutex_lock(m_arg->simul_lock);
				m_arg->simul->is_simul_alive = 0;
				pthread_mutex_unlock(m_arg->simul_lock);
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

int compile(coder_t *coder)
{
	struct timeval tv;
	long long current_time;
	int is_simul_alive;

	pthread_mutex_lock(&coder->locks->simul_state_lock);
	is_simul_alive = coder->simul->is_simul_alive;
	pthread_mutex_unlock(&coder->locks->simul_state_lock);
	if (is_simul_alive == 0)
		return (0);
	current_time = get_current_time(&tv);
	log_event(&coder->locks->print_lock, coder->id, "is compiling", current_time - coder->simul->start);
	pthread_mutex_lock(&coder->coder_lock);
	coder->last_compile_time = current_time;
	coder->burn_out_time = current_time + coder->config->time_to_burnout;
	pthread_mutex_unlock(&coder->coder_lock);
	usleep((coder->config->time_to_compile) * 1000);
	coder->compiles_done++;
	if (coder->compiles_done == coder->config->number_of_compiles_required)
	{
		pthread_mutex_lock(&coder->locks->simul_state_lock);
		coder->simul->finished_coders = coder->simul->finished_coders + 1;
		pthread_mutex_unlock(&coder->locks->simul_state_lock);
	}
	return (1);
}

int debug(coder_t *coder)
{
	struct timeval tv;
	long long time;
	int is_simul_alive;

	pthread_mutex_lock(&coder->locks->simul_state_lock);
	is_simul_alive = coder->simul->is_simul_alive;
	pthread_mutex_unlock(&coder->locks->simul_state_lock);
	if (is_simul_alive == 0)
		return (0);
	time = get_current_time(&tv) - coder->simul->start;
	log_event(&coder->locks->print_lock, coder->id, "is debugging", time);
	usleep((coder->config->time_to_debug) * 1000);
	return (1);
}

int refact(coder_t *coder)
{
	struct timeval tv;
	long long time;
	int is_simul_alive;

	pthread_mutex_lock(&coder->locks->simul_state_lock);
	is_simul_alive = coder->simul->is_simul_alive;
	pthread_mutex_unlock(&coder->locks->simul_state_lock);
	if (is_simul_alive == 0)
		return (0);
	time = get_current_time(&tv) - coder->simul->start;
	log_event(&coder->locks->print_lock, coder->id, "is refactoring", time);
	usleep((coder->config->time_to_refactor) * 1000);
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
// 	int	i;
	
// 	i = 0;
// 	while (i < heap->size)
// 	{
// 		print_req(&(heap->reqs)[i]);
// 		i++;
// 	}
// 	printf("\n");
// }

void *coders_routine(void* arg)
{
	coder_t 		*coder;
	coder = (coder_t *)arg;
	req_t	request;
	struct timeval	tv;
	long long		now;
	while (1)
	{
		now = get_current_time(&tv);
		request.coder = coder;
		request.arr_time = now;
		request.deadline = now + coder->config->time_to_burnout;
		pthread_mutex_lock(&coder->locks->print_lock);
		printf("Request Created: coder id %d, arr time %llu, deadline %llu\n", coder->id, request.arr_time, request.deadline);
		pthread_mutex_unlock(&coder->locks->print_lock);
		pthread_mutex_lock(&coder->heap->lock);
		heap_insert(coder->heap, request);
		pthread_mutex_unlock(&coder->heap->lock);
		// print_heap(coder->heap);
		if (!compile(coder))
            break;
        if (!debug(coder))
            break;
        if (!refact(coder))
            break;
	}
	return(NULL);
}

void start_to_work(t_config *config, simul_t *simul_state)
{
	heap_t			*heap;
	coder_t			*coders;
	dongle_t		*dongles;
	pthread_t		monitor;
	monitor_arg_t	*m_arg;
	locks_t			*locks;
	int	i;
	
	heap = init_heap(config);
	if (!heap)
		return ;
	locks = create_locks();
	dongles = init_dongles(config->number_of_coders);
	coders = init_coders(config, locks, simul_state, dongles, heap);
	m_arg = init_monitor(config, locks, simul_state, coders);
	i = 0;
	while (i < config->number_of_coders)
	{
		pthread_create(&coders[i].thread_id, NULL, coders_routine, &coders[i]);
		i++;
	}
	pthread_create(&monitor, NULL, monitor_routine, m_arg);
	pthread_join(monitor, NULL);
	i = 0;
	while (i < config->number_of_coders)
	{
		pthread_join(coders[i].thread_id, NULL);
		i++;
	}
	destroy_coders(coders, config->number_of_coders);
	destroy_dongles(dongles, config->number_of_coders);
	destroy_locks(locks);
	free(m_arg);
	destroy_heap(heap);
}
