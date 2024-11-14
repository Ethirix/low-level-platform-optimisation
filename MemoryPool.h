#pragma once

struct MemoryPool
{
	char* Pool;

	size_t Size;
	MemoryFooter* LastAllocation;
};
