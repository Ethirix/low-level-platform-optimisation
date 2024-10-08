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
	void AddAllocation()
	{
		_allocatedMap[typeid(T).hash_code()] += sizeof(T);
	}

	void AddAllocation(size_t size)
	{
		_genericAllocations += size;
	}

	template <typename T>
	void RemoveAllocation()
	{
		_allocatedMap[typeid(T).hash_code()] -= sizeof(T);
	}

	void RemoveAllocation(size_t size)
	{
		_genericAllocations -= size;
	}

	template <typename T>
	unsigned GetCurrentAllocation()
	{
		return _allocatedMap[typeid(T).hash_code()];
	}

	unsigned GetCurrentAllocation() const
	{
		return _genericAllocations;
	}

	unsigned GetTotalAllocation() const
	{
		unsigned total = 0;
		for (const std::pair<const unsigned, unsigned> element : _allocatedMap)
		{
			total += element.second;
		}

		return total + _genericAllocations;
	}

private:
	MemoryTracker() = default;
	~MemoryTracker() = default;

	std::unordered_map<size_t, unsigned> _allocatedMap{};
	unsigned _genericAllocations = 0;
};
