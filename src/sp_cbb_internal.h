#ifndef _SP_CIRCULAR_BYTE_BUFFER_INTERNAL_H
#define _SP_CIRCULAR_BYTE_BUFFER_INTERNAL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

//==============================
struct sp_cbb {
  uint8_t *buffer;
  size_t r;
  size_t w;
  size_t capacity;

  int read_only;
  struct sp_cbb *root;
  bool free_buffer;

  int mark_r;
  size_t original_r;

  int mark_w;
};

static inline void
sp_cbb_init_internal0(struct sp_cbb *self,
                     uint8_t *b,
                     size_t capacity,
                     size_t r,
                     size_t w)
{
  self->buffer   = b;
  self->r        = r;
  self->w        = w;
  self->capacity = capacity;
}

//==============================

#endif
