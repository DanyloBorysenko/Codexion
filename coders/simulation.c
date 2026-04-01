#include "codexion.h"

simul_state_t	*init_simul(void)
{
	simul_state_t	*ptr;

	ptr = malloc(sizeof(simul_state_t));
	if (!ptr)
	return (NULL);
	ptr->burn_out_coders = malloc(sizeof(int));
	ptr->finished_coders = malloc(sizeof(int));
	ptr->is_simul_alive = malloc(sizeof(int));
	if (!ptr->burn_out_coders || !ptr->finished_coders || !ptr->is_simul_alive)
	{
		free(ptr->burn_out_coders);
		free(ptr->finished_coders);
		free(ptr->is_simul_alive);
		free(ptr);
		return (NULL);
	}
	*(ptr->burn_out_coders) = 0;
	*(ptr->finished_coders) = 0;
	*(ptr->is_simul_alive) = 1;
	return (ptr);
}

void free_simul(simul_state_t *simul)
{
	free(simul->burn_out_coders);
	free(simul->finished_coders);
	free(simul->is_simul_alive);
	free(simul);
}