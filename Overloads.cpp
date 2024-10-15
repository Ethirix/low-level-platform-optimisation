#include "Overloads.h"

#include <iostream>

#include "MemoryFooter.h"
#include "MemoryHeader.h"
#include "MemoryTracker.h"

void* operator new(const size_t size)
{
	const size_t wrappedSize = sizeof(MemoryHeader) + size + sizeof(MemoryFooter);

	MemoryTracker::Get().AddAllocation(wrappedSize);
	char* pMemory = (char*)malloc(wrappedSize);

	MemoryHeader* header = (MemoryHeader*)pMemory;
	header->UnderflowTest = 0x5E7F100D;
	header->Size = wrappedSize;
	MemoryFooter* footer = (MemoryFooter*)(pMemory + sizeof(MemoryHeader) + size);
	footer->OverflowTest = 0xF100D5E7;

	std::cout << "Allocated: " << wrappedSize << '\n';
	void* pStartMemoryBlock = pMemory + sizeof(MemoryHeader);
	return pStartMemoryBlock;
}

void operator delete(void* pMemory)
{
	MemoryHeader* header = (MemoryHeader*)((char*)pMemory - sizeof(MemoryHeader));

	MemoryTracker::Get().RemoveAllocation(header->Size);
	free(header);
}