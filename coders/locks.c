#include "codexion.h"

void destroy_locks(locks_t *locks)
{
	pthread_mutex_destroy(&locks->print_lock);
	pthread_mutex_destroy(&locks->simul_state_lock);
	pthread_cond_destroy(&locks->sched_cond);
	free(locks);
}

locks_t	*init_locks(void)
{
	locks_t *locks;

	locks = malloc(sizeof(locks_t));
	if (!locks)
		return (NULL);
	pthread_mutex_init(&locks->print_lock, NULL);
	pthread_mutex_init(&locks->simul_state_lock, NULL);
	pthread_cond_init(&locks->sched_cond, NULL);
	return (locks);
}
