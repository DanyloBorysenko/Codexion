#include "codexion.h"

pthread_mutex_t	*init_mutexes(int count)
{
	int	i;
	pthread_mutex_t *ptr;

	ptr = malloc(sizeof(pthread_mutex_t) * count);
	if (!ptr)
		return (NULL);
	i = 0;
	while (i < count)
	{
		pthread_mutex_init(&ptr[i], NULL);
		i++;
	}
	return (ptr);
}

locks_t	*create_locks(int coders_count)
{
	locks_t *locks;

	locks = malloc(sizeof(locks_t));
	locks->coder_state_locks = init_mutexes(coders_count);
	locks->print_lock = init_mutexes(1);
	locks->simul_state_lock = init_mutexes(1);
	return (locks);
}

void	destroy_locks(locks_t *ptr, int coders_count)
{
	int	i;
	pthread_mutex_t *coder_state_locks;

	coder_state_locks = ptr->coder_state_locks;
	i = 0;
	while (i < coders_count)
	{
		pthread_mutex_destroy(&coder_state_locks[i]);
		i++;
	}
	pthread_mutex_destroy(ptr->print_lock);
	pthread_mutex_destroy(ptr->simul_state_lock);
	free(ptr->coder_state_locks);
	free(ptr->print_lock);
	free(ptr->simul_state_lock);
}
