/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danborys <borysenkodanyl@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/01 15:08:38 by danborys          #+#    #+#             */
/*   Updated: 2026/05/01 16:52:48 by danborys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	req_cmp(t_req parent, t_req child, char *sched)
{
	if (strcmp(sched, "fifo") == 0)
	{
		if (parent.arr_t > child.arr_t)
			return (1);
		else if (parent.arr_t < child.arr_t)
			return (-1);
	}
	else
	{
		if (parent.deadl > child.deadl)
			return (1);
		else if (parent.deadl < child.deadl)
			return (-1);
	}
	return (0);
}

void	swap_req(t_req *a, t_req *b)
{
	t_req	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

void	heapify_up(t_heap *heap, int index)
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

void	heapify_down(t_heap *heap, int index)
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
		if (right < heap->size
			&& req_cmp(heap->reqs[min], heap->reqs[right], heap->sched) > 0)
			min = right;
		if (min == index)
			break ;
		swap_req(&heap->reqs[index], &heap->reqs[min]);
		index = min;
	}
}
