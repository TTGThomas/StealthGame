#pragma once

#include <iostream>

#ifdef ENABLEMEMORYCOUNTER

static unsigned int allocated = 0, freed = 0;

void PrintStats()
{
	printf("Alloc: %u | freed: %u | leaked: %u\n", allocated, freed, allocated - freed);
}

void* operator new(size_t size)
{
	allocated += size;
	printf("Alloc: %u\n", size);
	PrintStats();
	return malloc(size);
}

void operator delete(void* block, size_t size)
{
	freed += size;
	printf("Freed: %u\n", size);
	PrintStats();
	free(block);
}

#endif