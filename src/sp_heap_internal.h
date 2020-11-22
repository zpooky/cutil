#ifndef SP_HEAP_INTERNAL_H
#define SP_HEAP_INTERNAL_H

#include <stddef.h>

static inline size_t
sp_heap_util_parent(size_t idx)
{
  return (idx - 1) / 2;
}

static inline size_t
sp_heap_util_left_child(size_t idx)
{
  return (2 * idx) + 1;
}

static inline size_t
sp_heap_util_right_child(size_t idx)
{
  return (2 * idx) + 2;
}

#endif
