#pragma once

#include "ColliderObject.h"
#include "TypedMemoryTracker.h"

class Box : public ColliderObject
{
public:
	void* operator new(size_t size)
	{
		MemoryTracker.AddAllocation();
		return ::operator new(size);
	}

	void operator delete(void* pMemory)
	{
		MemoryTracker.RemoveAllocation();
		::operator delete(pMemory);
	}

	void DrawMesh() override { glutSolidCube(1.0); }

	inline static TypedMemoryTracker<Box> MemoryTracker;
};
