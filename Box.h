#pragma once

#include "ColliderObject.h"
#include "MemoryTracker.h"

class Box : public ColliderObject
{
public:
	void* operator new(size_t size)
	{
		MemoryTracker::Get().AddAllocationOfType<Box>();
		return ::operator new(size);
	}

	void operator delete(void* pMemory)
	{
		MemoryTracker::Get().RemoveAllocationOfType<Box>();
		::operator delete(pMemory);
	}

	void drawMesh() override { glutSolidCube(1.0); }
};
