#pragma once
#include <cstdlib>

#include "MemoryPool.h"
#include "MemoryTracker.h"

/*
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
		MemoryTracker::LastTracked = header->Previous;
		header->Previous->Next = nullptr;
	}
	
	free(header);
*/

class MemoryPoolManager
{
public:
	[[nodiscard]] static void* Allocate(size_t size)
	{
		if (!_poolInitialised)
		{
			_pool = {
				static_cast<char*>(malloc(POOL_SIZE)),
				POOL_SIZE,
				nullptr
			};

			_poolInitialised = true;
		}

		const size_t wrappedSize = sizeof(MemoryHeader) + size + sizeof(MemoryFooter);
		MemoryFooter* lastAlloc = _pool.LastAllocation;
		char* addressToAllocateFrom = nullptr;

		if (!lastAlloc)
		{
			addressToAllocateFrom = _pool.Pool;
		}
		else if (lastAlloc 
			&& _pool.Pool + _pool.Size 
			- (_pool.Pool + lastAlloc->Header->Offset + lastAlloc->Header->Size) >= static_cast<int>(wrappedSize))
		{
			addressToAllocateFrom = _pool.Pool + lastAlloc->Header->Offset + lastAlloc->Header->Size;
		}
		else
		{
			lastAlloc = lastAlloc->Header->Previous;
			while (lastAlloc)
			{
				 if ((_pool.Pool + lastAlloc->Next->Offset + lastAlloc->Next->Size) 
					 - (_pool.Pool + lastAlloc->Header->Offset + lastAlloc->Header->Size) >= static_cast<int>(wrappedSize))
				 {
					 addressToAllocateFrom = _pool.Pool + lastAlloc->Header->Offset + lastAlloc->Header->Size;
				 }

				lastAlloc = lastAlloc->Header->Previous;
			}
		}

		if (!addressToAllocateFrom)
		{
			return GenericAllocation(size);
		}

		MemoryTracker::Get().AddAllocation(wrappedSize);

		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(addressToAllocateFrom);
		MemoryFooter* footer = reinterpret_cast<MemoryFooter*>(addressToAllocateFrom + sizeof(MemoryHeader) + size);
		header->UnderflowTest = UNDERFLOW_TEST;
		header->Size = wrappedSize;
		header->Offset = lastAlloc ? lastAlloc->Header->Offset + lastAlloc->Header->Size : 0;
		header->GlobalPrevious = MemoryTracker::LastTracked;
		header->Footer = footer;

		footer->OverflowTest = OVERFLOW_TEST;
		footer->Header = header;
		footer->GlobalNext = nullptr;

		//Pool Specific Insertion
		header->Previous = lastAlloc;
		footer->Next = lastAlloc ? lastAlloc->Next : nullptr;
		if (lastAlloc)
			lastAlloc->Next = header;
		if (footer->Next)
			footer->Next->Previous = footer;
			

		if (MemoryTracker::LastTracked)
			MemoryTracker::LastTracked->GlobalNext = header;

		if (_pool.LastAllocation)
			_pool.LastAllocation->Next = header;

		MemoryTracker::LastTracked = footer;
		_pool.LastAllocation = footer;

		void* pStartMemoryBlock = addressToAllocateFrom + sizeof(MemoryHeader);
		return pStartMemoryBlock;
	}

	static void Free(void* memoryBlock)
	{
		//TODO: Free
	}

private:
	static void* GenericAllocation(size_t size)
	{
		//TODO: Generic Allocation

		return nullptr;
	}

	static inline MemoryPool _pool;
	static inline unsigned _poolCount;
	static inline bool _poolInitialised = false;
};
