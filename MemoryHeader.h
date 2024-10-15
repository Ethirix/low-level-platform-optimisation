#pragma once

struct MemoryFooter;

struct MemoryHeader
{
	unsigned UnderflowTest = 0x5E7F100D;
	size_t Size;
};