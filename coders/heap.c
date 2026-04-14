/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 14:40:03 by danborys          #+#    #+#             */
/*   Updated: 2026/04/14 09:59:56 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

heap_t *init_heap(t_config *config)
{
	req_t *reqs;
	heap_t *heap;

	// reqs = malloc(sizeof(req_t) * config->number_of_coders);
	reqs = malloc(sizeof(req_t) * config->number_of_compiles_required * 2);
	if (!reqs)
		return (NULL);
	heap = malloc(sizeof(heap_t));
	if (!heap)
	{
		free(reqs);
		return (NULL);
	}
	// heap->capacity = config->number_of_coders;
	heap->capacity = config->number_of_coders * 2;
	heap->scheduler = config->scheduler;
	heap->reqs = reqs;
	heap->size = 0;
	pthread_mutex_init(&heap->lock, NULL);
	return (heap);
}

void destroy_heap(heap_t *heap)
{
	if (!heap)
		return;
	pthread_mutex_destroy(&heap->lock);
	free(heap->reqs);
	free(heap);
}

int req_cmp(req_t parent, req_t child, char *sched)
{
	if (strcmp(sched, "fifo") == 0)
	{
		if (parent.arr_time > child.arr_time)
			return (1);
		else if (parent.arr_time < child.arr_time)
			return (-1);
	}
	else
	{
		if (parent.deadline > child.deadline)
			return (1);
		else if (parent.deadline < child.deadline)
			return (-1);
	}
	return (0);
}

static void	swap_req(req_t *a, req_t *b)
{
	req_t tmp;
	
	tmp = *a;
	*a = *b;
	*b = tmp;
}

void	heapify_up(heap_t *heap, int child_ind)
{
	int	parent_ind;

	while (child_ind > 0)
	{
		parent_ind = (child_ind - 1) / 2;
		if (req_cmp(heap->reqs[parent_ind], heap->reqs[child_ind], heap->scheduler) <= 0)
			break;
		swap_req(&heap->reqs[parent_ind], &heap->reqs[child_ind]);
		child_ind = parent_ind;
	}
}

void heap_insert(heap_t *heap, req_t req)
{
	int child_ind;
	
	pthread_mutex_lock(&heap->lock);
	if (heap->size >= heap->capacity)
	{
		pthread_mutex_unlock(&heap->lock);
		return ;	
	}
	child_ind = heap->size;
	heap->reqs[child_ind] = req;
	heap->size++;
	heapify_up(heap, child_ind);
	pthread_mutex_unlock(&heap->lock);
}
