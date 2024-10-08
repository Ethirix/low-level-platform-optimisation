#include "Overloads.h"

void* operator new(const size_t size)
{
	char* pMemory = static_cast<char*>(malloc(size));
	void* pStartMemoryBlock = pMemory;
	return pStartMemoryBlock;
}

void operator delete(void* pMemory)
{
	free(pMemory);
}