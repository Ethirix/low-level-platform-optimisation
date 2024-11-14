#pragma once

class MemoryTracker
{
public:
	static MemoryTracker& Get()
	{
		static MemoryTracker instance;
		return instance;
	}

	void AddAllocation(const size_t size)
	{
		_globalAllocation += size;
	}

	void RemoveAllocation(const size_t size)
	{
		_globalAllocation -= size;
	}

	[[nodiscard]] unsigned GetAllocation() const
	{
		return _globalAllocation;
	}
	
	MemoryTracker(MemoryTracker const&) = delete;
	MemoryTracker(MemoryTracker&&) = delete;
	MemoryTracker& operator=(MemoryTracker const&) = delete;
	MemoryTracker& operator=(MemoryTracker&&) = delete;

	static inline MemoryFooter* LastTracked = nullptr;

private:
	MemoryTracker() = default;
	~MemoryTracker() = default;

	unsigned _globalAllocation;
};