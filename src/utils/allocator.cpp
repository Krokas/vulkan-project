#include "allocator.h"

#include <stdlib.h>
#include <string.h>

#include "log.h"

MemoryAllocator makeAllocator(size_t size) {
  MemoryAllocator alloc = {};
  alloc.memory = (char *)malloc(size);
  if (alloc.memory) {
    alloc.capacity = size;
    memset(alloc.memory, 0, size);
  } else {
    R_ASSERT(false, "Failed to allocate Memory!");
  }

  return alloc;
}

char *allocate(MemoryAllocator *allocator, size_t size) {
  char *result = nullptr;

  // This makes sure that first four bits are set to 0
  size_t allignedSize = (size + 7) & ~7;
  if (allocator->used + allignedSize <= allocator->capacity) {
    result = allocator->memory + allocator->used;
    allocator->used += allignedSize;
  } else {
    R_ASSERT(false, "Allocator is full!");
  }

  return result;
}