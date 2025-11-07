#ifndef _SP_VECTOR_COPY_INTERNAL_H
#define _SP_VECTOR_COPY_INTERNAL_H

#define _GNU_SOURCE

#include <stdint.h>
#include <stdlib.h>
#include <stdalign.h>
#include <stddef.h>
#include <assert.h>

#include "sp_util.h"

//==============================
struct sp_vec_copy {
  uint8_t *raw;
  size_t length;
  size_t capacity;

  size_t element_align;
  size_t element_sz;

  void (*copy)(void *dest, const void *src, size_t);
};

static inline void
sp_vec_copy_internal_init(struct sp_vec_copy *self,
                          size_t capacity,
                          size_t element_align,
                          size_t element_sz,
                          void (*copy)(void *dest, const void *src, size_t))
{
  assert(self);
  self->length = self->capacity = 0;
  self->raw                     = NULL;

  if (capacity) {
    self->raw      = aligned_alloc(element_align, capacity * element_sz);
    self->capacity = capacity;
  }
  self->element_sz    = element_sz;
  self->element_align = element_align;
  self->copy  = copy;
}

//==============================
static inline void
sp_vec_copy_internal_free(struct sp_vec_copy *self)
{
  if (self->raw) {
    free(self->raw);
  }
  self->raw    = NULL;
  self->length = self->element_sz = 0;
}

//==============================
static inline void
sp_vec_copy_swap_self(struct sp_vec_copy *f, struct sp_vec_copy *s)
{
  sp_util_swap_voidp(&f->raw, &s->raw);

  sp_util_swap_size_t(&f->length, &s->length);
  sp_util_swap_size_t(&f->capacity, &s->capacity);

  sp_util_swap_size_t(&f->element_align, &s->element_align);
  sp_util_swap_size_t(&f->element_sz, &s->element_sz);

  sp_util_swap_voidp(&f->copy, &s->copy);
}

//==============================

#endif
