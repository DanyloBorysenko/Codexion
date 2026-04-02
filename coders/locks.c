#include "codexion.h"

pthread_mutex_t	*init_mutex(void)
{
	pthread_mutex_t *ptr;

	ptr = malloc(sizeof(pthread_mutex_t));
	if (!ptr)
		return (NULL);
	pthread_mutex_init(ptr, NULL);
	return (ptr);
}

locks_t	*create_locks(void)
{
	locks_t *locks;

	locks = malloc(sizeof(locks_t));
	locks->print_lock = init_mutex();
	locks->simul_state_lock = init_mutex();
	if (!locks->print_lock || !locks->simul_state_lock)
	{
		free(locks->print_lock);
		free(locks->simul_state_lock);
		free(locks);
		return (NULL);
	}
	return (locks);
}

void	destroy_and_free_locks(locks_t *ptr)
{
	pthread_mutex_destroy(ptr->print_lock);
	pthread_mutex_destroy(ptr->simul_state_lock);
	free(ptr->print_lock);
	free(ptr->simul_state_lock);
	free(ptr);
}
