#include "random.h"


void setSeed(int *seed)
{
    srand(seed == NULL ? time(0): *seed);
}

int randomInRange(int min, int max)
{
	if (max + 1 - min == 0)
		return 0;
	return min + rand() % (max + 1 - min);
}
