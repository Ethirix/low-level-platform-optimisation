#pragma once

struct MemoryPool
{
	~MemoryPool()
	{
		delete Pool;
	}

	char* Pool;

	size_t Size;
	MemoryFooter* LastAllocation;
};
