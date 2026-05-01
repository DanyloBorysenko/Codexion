/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 14:40:03 by danborys          #+#    #+#             */
/*   Updated: 2026/05/01 16:25:42 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int		req_cmp(t_req parent, t_req child, char *sched);
void	swap_req(t_req *a, t_req *b);
void	heapify_up(t_heap *heap, int index);
void	heapify_down(t_heap *heap, int index);

t_heap	*init_heap(int count, char *sched)
{
	t_heap	*heap;
	t_req	*reqs;

	heap = malloc(sizeof(t_heap));
	if (!heap)
		return (NULL);
	reqs = malloc(sizeof(t_req) * count);
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

void	destroy_heap(t_heap *heap)
{
	if (!heap)
		return ;
	if (heap->reqs)
		free(heap->reqs);
	free(heap);
}

void	heap_insert(t_heap *heap, t_req req)
{
	int	child_ind;

	if (heap->size >= heap->capacity)
		return ;
	child_ind = heap->size;
	heap->reqs[child_ind] = req;
	heap->size++;
	heapify_up(heap, child_ind);
}

t_req	heap_extract(t_heap *heap, int index)
{
	t_req	removed;

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

void	print_heap(t_heap *heap, int don_id)
{
	int			i;
	int			cod_id;
	long long	arr_t;
	long long	deadl_t;
	t_req		req;

	i = 0;
	printf("Dongle %d\n", don_id);
	while (i < heap->size)
	{
		req = heap->reqs[i];
		cod_id = req.cod_id;
		arr_t = req.arr_t;
		deadl_t = req.deadl;
		printf("cod: %d, arr: %llu, deadl: %llu\n", cod_id, arr_t, deadl_t);
		i++;
	}
}
