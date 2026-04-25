/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 14:40:03 by danborys          #+#    #+#             */
/*   Updated: 2026/04/25 20:26:56 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

heap_t	*init_heap(int count, char *sched)
{
	heap_t	*heap;
	req_t	*reqs;

	heap = malloc(sizeof(heap_t));
	if (!heap)
		return (NULL);
	reqs = malloc(sizeof(req_t) * count);
	if (!reqs)
	{
		free(heap);
		return (NULL);
	}
	heap->capacity = count;
	heap->sched = sched;
	heap->reqs = reqs;
	heap->size = 0;
	return (heap);
}

void	destroy_heap(heap_t *heap)
{
	if (!heap)
		return ;
	if (heap->reqs)
		free(heap->reqs);
	free(heap);
}

int	req_cmp(req_t parent, req_t child, char *sched)
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
	req_t	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

void	heapify_up(heap_t *heap, int index)
{
	int	par_ind;

	while (index > 0)
	{
		par_ind = (index - 1) / 2;
		if (req_cmp(heap->reqs[par_ind], heap->reqs[index], heap->sched) <= 0)
			break ;
		swap_req(&heap->reqs[par_ind], &heap->reqs[index]);
		index = par_ind;
	}
}

void	heapify_down(heap_t *heap, int index)
{
	int	left;
	int	right;
	int	min;

	while (1)
	{
		min = index;
		left = (index * 2) + 1;
		right = (index * 2) + 2;
		if (left < heap->size
			&& req_cmp(heap->reqs[min], heap->reqs[left], heap->sched) > 0)
			min = left;
		if (right < heap->size)
		{
			if (req_cmp(heap->reqs[min], heap->reqs[right], heap->sched) > 0)
				min = right;
		}
		if (min == index)
			break ;
		swap_req(&heap->reqs[index], &heap->reqs[min]);
		index = min;
	}
}

void	heap_insert(heap_t *heap, req_t req)
{
	int	child_ind;

	if (heap->size >= heap->capacity)
		return ;
	child_ind = heap->size;
	heap->reqs[child_ind] = req;
	heap->size++;
	heapify_up(heap, child_ind);
}

req_t	heap_extract(heap_t *heap, int index)
{
	req_t	removed;

	removed = heap->reqs[index];
	if (index < heap->size - 1)
	{
		heap->reqs[index] = heap->reqs[heap->size - 1];
		heap->size--;
		heapify_down(heap, index);
		heapify_up(heap, index);
	}
	else
		heap->size--;
	return (removed);
}
