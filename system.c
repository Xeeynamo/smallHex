#include <stdlib.h>
#include "system.h"

void *MemoryAlloc(unsigned int size)
{
	return malloc(size);
}
void *MemoryFree(void *mem)
{
	free(mem);
	return NULL;
}