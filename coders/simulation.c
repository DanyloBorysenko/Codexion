#include "codexion.h"

simul_state_t	*init_simul(void)
{
	simul_state_t	*ptr;

	ptr = malloc(sizeof(simul_state_t));
	if (!ptr)
	return (NULL);
	ptr->finished_coders = malloc(sizeof(int));
	ptr->is_simul_alive = malloc(sizeof(int));
	if (!ptr->finished_coders || !ptr->is_simul_alive)
	{
		free_simul(ptr);
		return (NULL);
	}
	*(ptr->finished_coders) = 0;
	*(ptr->is_simul_alive) = 1;
	return (ptr);
}

void free_simul(simul_state_t *simul)
{
	free(simul->finished_coders);
	free(simul->is_simul_alive);
	free(simul);
}