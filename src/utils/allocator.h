#pragma once

#include "log.h"

struct MemoryAllocator {
  size_t capacity;
  size_t used;
  char *memory;
};

MemoryAllocator makeAllocator(size_t size);
char *allocate(MemoryAllocator *allocator, size_t size);