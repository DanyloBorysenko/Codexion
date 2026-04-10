#include "codexion.h"

pthread_mutex_t	*alloc_mutex(int count)
{
	pthread_mutex_t *ptr;

	ptr = malloc(sizeof(pthread_mutex_t) * count);
	if (!ptr)
		return (NULL);
	return (ptr);
}

void init_locks(locks_t *locks)
{
	pthread_mutex_init(locks->print_lock, NULL);
	pthread_mutex_init(locks->simul_state_lock, NULL);
}

void destroy_locks(locks_t *locks)
{
	pthread_mutex_destroy(locks->print_lock);
	pthread_mutex_destroy(locks->simul_state_lock);
}

void	free_locks(locks_t *ptr)
{
	free(ptr->print_lock);
	free(ptr->simul_state_lock);
	free(ptr);
}

locks_t	*create_locks(void)
{
	locks_t *locks;

	locks = malloc(sizeof(locks_t));
	if (!locks)
		return (NULL);
	locks->print_lock = alloc_mutex(1);
	locks->simul_state_lock = alloc_mutex(1);
	if (!locks->print_lock || !locks->simul_state_lock)
	{
		free_locks(locks);
		return (NULL);
	}
	init_locks(locks);
	return (locks);
}
