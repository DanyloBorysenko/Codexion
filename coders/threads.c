#include "codexion.h"

void *routine(void* arg)
{
	printf("id: %d\n", *(int *)arg);
	return(NULL);
}

void start_to_work(t_config *config)
{
	pthread_t 	*threads;
	int			*ids;
	int			i;

	threads = malloc(sizeof(pthread_t) * config->number_of_coders);
	if (!threads)
	{
		printf("Threads were not created\n");
		exit(EXIT_FAILURE);
	}
	ids = malloc(sizeof(int) * config->number_of_coders);
	if (!ids)
	{
		printf("Thread ids were not created");
		exit(EXIT_FAILURE);
	}
	i = 0;
	while (i < config->number_of_coders)
	{
		ids[i] = i + 1;
		pthread_create(&threads[i], NULL, routine, &ids[i]);
		i++;
	}
	i = 0;
	while (i < config->number_of_coders)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
	free(threads);
	free(ids);
}