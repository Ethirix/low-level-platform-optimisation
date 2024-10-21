#pragma once

#include <iostream>

#include "ColliderObject.h"
#include "TypedMemoryTracker.h"

class Sphere : public ColliderObject
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

    void DrawMesh() override { glutSolidSphere(0.5, 5, 5); }

	inline static TypedMemoryTracker<Sphere> MemoryTracker;
}; 

