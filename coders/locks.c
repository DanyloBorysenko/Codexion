#include "codexion.h"

void destroy_locks(locks_t *locks)
{
	pthread_mutex_destroy(&locks->print_lock);
	pthread_mutex_destroy(&locks->simul_state_lock);
	free(locks);
}

locks_t	*create_locks(void)
{
	locks_t *locks;

	locks = malloc(sizeof(locks_t));
	if (!locks)
		return (NULL);
	pthread_mutex_init(&locks->print_lock, NULL);
	pthread_mutex_init(&locks->simul_state_lock, NULL);
	return (locks);
}
