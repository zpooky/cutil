#ifndef _SP_BITSET_INTERNAL_H
#define _SP_BITSET_INTERNAL_H

#include <stddef.h>
#include <stdlib.h>

#include "sp_util.h"

//==============================
struct sp_bitset {
  int *raw;
  size_t length;
  size_t bits;
};

//==============================
#define BS_BITS (sizeof(int) * 8)
static size_t
bits_to_capacity(size_t bits)
{
  return (bits / BS_BITS) + (bits % BS_BITS == 0 ? 0 : 1);
}

static inline int
sp_bitset_init_internal(struct sp_bitset *self, size_t bits)
{
  size_t capacity = sp_util_max(bits_to_capacity(bits), 1);
  self->raw       = calloc(capacity, sizeof(int));
  self->length    = capacity;
  self->bits      = bits;
  return 0;
}

//==============================
static inline void
sp_bitset_free_internal(struct sp_bitset *self)
{
  if (self->raw) {
    free(self->raw);
  }
}

#endif
