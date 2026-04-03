#include "codexion.h"

pthread_mutex_t	*alloc_mutex(int count)
{
	pthread_mutex_t *ptr;

	ptr = malloc(sizeof(pthread_mutex_t) * count);
	if (!ptr)
		return (NULL);
	return (ptr);
}

void init_locks(locks_t *locks, int coders_count)
{
	int	i;

	i = 0;
	while (i < coders_count)
	{
		pthread_mutex_init(&(locks->coder_state_locks[i]), NULL);
		i++;
	}
	pthread_mutex_init(locks->print_lock, NULL);
	pthread_mutex_init(locks->simul_state_lock, NULL);
}

void destroy_locks(locks_t *locks, int coders_count)
{
	int	i;

	i = 0;
	while (i < coders_count)
	{
		pthread_mutex_destroy(&(locks->coder_state_locks[i]));
		i++;
	}
	pthread_mutex_destroy(locks->print_lock);
	pthread_mutex_destroy(locks->simul_state_lock);
}

void	free_locks(locks_t *ptr)
{
	free(ptr->print_lock);
	free(ptr->simul_state_lock);
	free(ptr->coder_state_locks);
	free(ptr);
}

locks_t	*create_locks(int coders_count)
{
	locks_t *locks;

	locks = malloc(sizeof(locks_t));
	if (!locks)
		return (NULL);
	locks->coder_state_locks = alloc_mutex(coders_count);
	locks->print_lock = alloc_mutex(1);
	locks->simul_state_lock = alloc_mutex(1);
	if (!locks->print_lock || !locks->simul_state_lock || !locks->coder_state_locks)
	{
		free_locks(locks);
		return (NULL);
	}
	init_locks(locks, coders_count);
	return (locks);
}
