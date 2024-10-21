#include "Overloads.h"

#include <iostream>

#include "MemoryFooter.h"
#include "MemoryHeader.h"
#include "MemoryTracker.h"

void* operator new(const size_t size)
{
	static MemoryFooter* lastFooter = nullptr;

	const size_t wrappedSize = sizeof(MemoryHeader) + size + sizeof(MemoryFooter);

	MemoryTracker::Get().AddAllocation(wrappedSize);
	char* pMemory = (char*)malloc(wrappedSize);

	MemoryHeader* header = (MemoryHeader*)pMemory;
	header->UnderflowTest = 0x5E7F100D;
	header->Size = wrappedSize;
	header->Last = lastFooter;

	MemoryFooter* footer = (MemoryFooter*)(pMemory + sizeof(MemoryHeader) + size);
	footer->OverflowTest = 0xF100D5E7;
	footer->Header = header;
	footer->Next = nullptr;
	header->Footer = footer;

	if (lastFooter)
		lastFooter->Next = header;

	lastFooter = footer;

	void* pStartMemoryBlock = pMemory + sizeof(MemoryHeader);
	return pStartMemoryBlock;
}

void operator delete(void* pMemory)
{
	MemoryHeader* header = (MemoryHeader*)((char*)pMemory - sizeof(MemoryHeader));

	//What should happen if the header does get overwritten as the size is lost.
	if (header->UnderflowTest != 0x5E7F100D)
	{
		std::cout << "Err: Header Overwritten!" << '\n';
		//This? (rejig headers and footers as well)
		free(header);
		return;
	}

	MemoryTracker::Get().RemoveAllocation(header->Size);

	MemoryFooter* footer = (MemoryFooter*)((char*)pMemory + header->Size);
	
	if (footer->OverflowTest != 0xF100D5E7)
		std::cout << "Err: Footer Overwritten!" << '\n';

	header->Last->Next = footer->Next;
	
	free(header);
}