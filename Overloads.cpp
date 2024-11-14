#include "Overloads.h"

#include <iostream>

#include "MemoryPoolManager.h"

void* operator new(const size_t size)
{
	return MemoryPoolManager::Allocate(size);
}

void operator delete(void* pMemory)
{
	MemoryPoolManager::Free(pMemory);
}