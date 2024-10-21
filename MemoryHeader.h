#pragma once

struct MemoryFooter;

struct MemoryHeader
{
	unsigned UnderflowTest = UNDERFLOW_TEST;
	size_t Size;

	MemoryFooter* Footer = nullptr;
	MemoryFooter* Previous = nullptr;

	static inline MemoryFooter* Last = nullptr;
};