#pragma once
#include <assert.h>
#include <cstdlib>

#include "globals.h"
#include "MemoryFooter.h"
#include "MemoryHeader.h"
#include "MemoryPool.h"
#include "MemoryTracker.h"

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
		bool ranMalloc = false;

		if (!lastAlloc)
		{
			//Allocates immediately at start of pool if empty.
			addressToAllocateFrom = _pool.Pool;
		}

		if (!addressToAllocateFrom)
		{
			//Attempts to backtrack pool to find space - doing this first as to optimize slack space
			lastAlloc = lastAlloc->Header->Previous;
			while (lastAlloc)
			{
				if (MemoryHeader* header = lastAlloc->Header;
					lastAlloc->Next->Offset - (header->Offset + header->Size)>= wrappedSize)
				{
					addressToAllocateFrom = _pool.Pool + header->Offset + header->Size;
				}

				lastAlloc = lastAlloc->Header->Previous;
			}
		}

		if (!addressToAllocateFrom)
		{
			lastAlloc = _pool.LastAllocation;
			if (MemoryHeader* header = lastAlloc->Header;
			lastAlloc && _pool.Pool + _pool.Size 
				- (_pool.Pool + header->Offset + header->Size) >= static_cast<int>(wrappedSize))
			{
				//Allocates at top of pool
				addressToAllocateFrom = _pool.Pool + header->Offset + header->Size;
			}
		}

		if (!addressToAllocateFrom)
		{
			addressToAllocateFrom = static_cast<char*>(malloc(wrappedSize));
			ranMalloc = true;
		}

		MemoryTracker::Get().AddAllocation(wrappedSize);

		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(addressToAllocateFrom);
		MemoryFooter* footer = reinterpret_cast<MemoryFooter*>(addressToAllocateFrom + sizeof(MemoryHeader) + size);
		assert(header);
		assert(footer);

		//Placement new operator, allowing construction of already-allocated objects.
		new(header) MemoryHeader();
		new(footer) MemoryFooter();

		header->Size = wrappedSize;
		header->Offset = lastAlloc ? lastAlloc->Header->Offset + lastAlloc->Header->Size : 0;
		header->GlobalPrevious = MemoryTracker::LastTracked;
		header->Footer = footer;
		
		footer->Header = header;

		//Pool Specific Insertion
		if (!ranMalloc)
		{
			header->Previous = lastAlloc;
			footer->Next = lastAlloc ? lastAlloc->Next : nullptr;
			if (lastAlloc)
				lastAlloc->Next = header;
			if (footer->Next)
				footer->Next->Previous = footer;
			if (_pool.LastAllocation)
				_pool.LastAllocation->Next = header;
			_pool.LastAllocation = footer;
		}

		if (MemoryTracker::LastTracked)
			MemoryTracker::LastTracked->GlobalNext = header;

		MemoryTracker::LastTracked = footer;

		void* pStartMemoryBlock = addressToAllocateFrom + sizeof(MemoryHeader);
		return pStartMemoryBlock;
	}

	static void Free(void* memoryBlock)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(static_cast<char*>(memoryBlock) - sizeof(MemoryHeader));
		MemoryFooter* footer = header->Footer;

		MemoryTracker::Get().RemoveAllocation(header->Size);

		//What should happen if the header does get overwritten as the size is lost.
		if (header->UnderflowTest != UNDERFLOW_TEST)
			std::cout << "Err: Header Overwritten!" << '\n';

		if (footer->OverflowTest != OVERFLOW_TEST)
			std::cout << "Err: Footer Overwritten!" << '\n';

		if (footer->Next)
		{
			header->Previous->Next = footer->Next;
			footer->Next->Previous = header->Previous;
		}
		else if (header->Previous)
		{
			_pool.LastAllocation = header->Previous;
			header->Previous->Next = nullptr;
		}

		if (footer->GlobalNext)
		{
			header->GlobalPrevious->GlobalNext = footer->GlobalNext;
			footer->GlobalNext->GlobalPrevious = header->GlobalPrevious;
		}
		else
		{
			MemoryTracker::LastTracked = header->GlobalPrevious;
			header->GlobalPrevious->GlobalNext = nullptr;
		}

		if (!footer->Next && !header->Previous)
		{
			// Both being false implies that the current memory alloc was not allocated to a pool
			free(header);
		}
	}

private:
	static inline MemoryPool _pool;
	static inline unsigned _poolCount;
	static inline bool _poolInitialised = false;
};
