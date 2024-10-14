#pragma once
#include <typeinfo>

template <typename T>
class TypedMemoryTracker
{
public:
	TypedMemoryTracker() = default;
	~TypedMemoryTracker() = default;

	void AddAllocation()
	{
		_allocation += Size();
	}

	void RemoveAllocation()
	{
		_allocation -= Size();
	}

	[[nodiscard]] unsigned GetAllocation() const
	{
		return _allocation;
	}

private:
	constexpr size_t Size() { return sizeof(T); }

	unsigned _allocation = 0;
};
