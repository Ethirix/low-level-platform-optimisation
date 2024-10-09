#pragma once
#include <unordered_map>
#include <typeinfo>

class MemoryTracker
{
public:
	static MemoryTracker& Get()
	{
		static MemoryTracker instance;
		return instance;
	}

	MemoryTracker(MemoryTracker const&) = delete;
	MemoryTracker(MemoryTracker&&) = delete;
	MemoryTracker& operator=(MemoryTracker const&) = delete;
	MemoryTracker& operator=(MemoryTracker&&) = delete;

	template <typename T>
	void AddAllocationOfType()
	{
		_allocatedMap[typeid(T).hash_code()] += sizeof(T);
	}

	void AddAllocation(size_t size)
	{
		_totalAllocation += size;
	}

	template <typename T>
	void RemoveAllocationOfType()
	{
		_allocatedMap[typeid(T).hash_code()] -= sizeof(T);
	}

	void RemoveAllocation(size_t size)
	{
		_totalAllocation -= size;
	}

	template <typename T>
	unsigned GetCurrentAllocationOfType()
	{
		return _allocatedMap[typeid(T).hash_code()];
	}

	unsigned GetTotalAllocation() const
	{
		return _totalAllocation;
	}

private:
	MemoryTracker() = default;
	~MemoryTracker() = default;

	std::unordered_map<size_t, unsigned> _allocatedMap{};
	unsigned _totalAllocation = 0;
};
