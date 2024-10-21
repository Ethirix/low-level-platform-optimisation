#include "Overloads.h"

#include <iostream>

#include "globals.h"
#include "MemoryFooter.h"
#include "MemoryHeader.h"
#include "MemoryTracker.h"

void* operator new(const size_t size)
{
	const size_t wrappedSize = sizeof(MemoryHeader) + size + sizeof(MemoryFooter);

	MemoryTracker::Get().AddAllocation(wrappedSize);
	char* pMemory = (char*)malloc(wrappedSize);

	MemoryHeader* header = (MemoryHeader*)pMemory;
	header->UnderflowTest = UNDERFLOW_TEST;
	header->Size = wrappedSize;
	header->Previous = MemoryHeader::Last;

	MemoryFooter* footer = (MemoryFooter*)(pMemory + sizeof(MemoryHeader) + size);
	footer->OverflowTest = OVERFLOW_TEST;
	footer->Header = header;
	footer->Next = nullptr;
	header->Footer = footer;

	if (MemoryHeader::Last)
		MemoryHeader::Last->Next = header;

	MemoryHeader::Last = footer;

	void* pStartMemoryBlock = pMemory + sizeof(MemoryHeader);
	return pStartMemoryBlock;
}

void operator delete(void* pMemory)
{
	MemoryHeader* header = (MemoryHeader*)((char*)pMemory - sizeof(MemoryHeader));

	//What should happen if the header does get overwritten as the size is lost.
	if (header->UnderflowTest != UNDERFLOW_TEST)
	{
		std::cout << "Err: Header Overwritten!" << '\n';
		//This? (rejig headers and footers as well)
		free(header);
		return;
	}

	MemoryTracker::Get().RemoveAllocation(header->Size);

	MemoryFooter* footer = header->Footer;
	
	if (footer->OverflowTest != OVERFLOW_TEST)
		std::cout << "Err: Footer Overwritten!" << '\n';

	if (footer->Next)
	{
		header->Previous->Next = footer->Next;
		footer->Next->Previous = header->Previous;
	}
	else
	{
		MemoryHeader::Last = header->Previous;
		header->Previous->Next = nullptr;
	}
	
	free(header);
}