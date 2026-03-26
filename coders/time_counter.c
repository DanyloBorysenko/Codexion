#include "codexion.h"

long long get_current_time(struct timeval* tv)
{
	gettimeofday(tv, NULL);
	return ((long long) (tv->tv_sec * 1000) + (tv->tv_usec / 1000));
}
