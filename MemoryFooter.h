#pragma once

struct MemoryHeader;

struct MemoryFooter
{
	unsigned OverflowTest = 0xF100D5E7;

	MemoryHeader* Header = nullptr;
	MemoryHeader* Next = nullptr;
};
