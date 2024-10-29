#pragma once
#include <cstdlib>

struct MemoryPool
{
	char* Pool;

	size_t Size;
	size_t Allocated;
};

class MemoryPoolManager
{
public:
	static bool CreateMemoryPool(size_t size)
	{
		if (_poolCount >= MAX_POOL_COUNT)
			return false;

		auto& [Pool, Size, Allocated] = _pools[++_poolCount];
		Pool = (char*)malloc(size);
		Size = size;

		return true;
	}

	static void* Allocate(size_t size)
	{
		for (auto& [Pool, Size, Allocated] : _pools)
		{
			if (Size - Allocated <= size)
			{
				//TODO: Allocate
			}
		}

		//TODO: Default Allocation
	}

	static void Free(void* memoryBlock, size_t size)
	{
		char* mBlock = (char*)memoryBlock;
		for (auto& [Pool, Size, Allocated] : _pools)
		{
			for (unsigned i = 0; i < Size; i++)
			{
				if (&Pool[i] == mBlock)
				{
					for (unsigned offset = 0; offset < size; offset++)
					{
						Pool[i + offset] = -127;
					}
				}
			}
		}
	}

private:
	static inline MemoryPool _pools[MAX_POOL_COUNT];
	static inline unsigned _poolCount;
};
