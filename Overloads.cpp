#include "Overloads.h"

#include "MemoryTracker.h"

void* operator new(const size_t size)
{
	MemoryTracker::Get().AddAllocation(size);
	char* pMemory = static_cast<char*>(malloc(size));
	void* pStartMemoryBlock = pMemory;
	return pStartMemoryBlock;
}

void operator delete(void* pMemory)
{
	MemoryTracker::Get().RemoveAllocation(sizeof(pMemory));
	free(pMemory);
}