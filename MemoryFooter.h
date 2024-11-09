#pragma once

struct MemoryHeader;

struct MemoryFooter
{
	unsigned OverflowTest = OVERFLOW_TEST;

	MemoryHeader* Header = nullptr;
	MemoryHeader* Next = nullptr;
	MemoryHeader* GlobalNext = nullptr;
};
