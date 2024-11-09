#pragma once

struct MemoryFooter;

struct MemoryHeader
{
	unsigned UnderflowTest = UNDERFLOW_TEST;
	size_t Size;
	unsigned Offset;

	MemoryFooter* Footer = nullptr;
	MemoryFooter* Previous = nullptr;
	MemoryFooter* GlobalPrevious = nullptr;
};