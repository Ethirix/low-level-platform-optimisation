#pragma once
#include <cstdlib>

void* operator new(size_t size);

void operator delete(void* pMemory);