#define _GNU_SOURCE

#include "sp_vec_copy.h"
#include "sp_vec_copy_internal.h"

#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

static void
__sp_vec_copy_copy(struct sp_vec_copy *dest, const struct sp_vec_copy *src);

static bool
__sp_vec_copy_ensure_capacity(struct sp_vec_copy *self, size_t capacity);

//==============================
struct sp_vec_copy *
sp_vec_copy_init(size_t element_align, size_t element_sz, sp_cb_copy copy)
{
  struct sp_vec_copy *result;
  if ((result = calloc(1, sizeof(*result)))) {
    sp_vec_copy_internal_init(result, 0, element_align, element_sz, copy);
  }
  return result;
}

struct sp_vec_copy *
sp_vec_copy_init_copy(const struct sp_vec_copy *o)
{
  struct sp_vec_copy *result;
  if ((result = sp_vec_copy_init(o->element_align, o->element_sz, o->copy))) {
    assertxs(result->element_sz == o->element_sz, "%zu:%zu", result->element_sz,
             o->element_sz);
    assertxs(result->element_align == o->element_align, "%zu:%zu",
             result->element_align, o->element_align);
    /* assertxs(result->capacity == o->capacity, "%zu:%zu",result->capacity, o->capacity); */
    assertx(result->copy == o->copy);

    __sp_vec_copy_ensure_capacity(result, o->capacity);

    __sp_vec_copy_copy(result, o);
    assertxs(result->length == o->length, "%zu:%zu", result->length, o->length);
  }
  return result;
}

struct sp_vec_copy *
sp_vec_copy_init0(size_t element_align, size_t element_sz)
{
  return sp_vec_copy_init(element_align, element_sz, sp_util_memcopy);
}

struct sp_vec_copy *
sp_vec_copy_init0_cap(size_t capacity, size_t element_align, size_t element_sz)
{

  return sp_vec_copy_init_cap(capacity, element_align, element_sz,
                              sp_util_memcopy);
}

struct sp_vec_copy *
sp_vec_copy_init_cap(size_t capacity,
                     size_t element_align,
                     size_t element_sz,
                     sp_cb_copy copy)
{
  struct sp_vec_copy *result;

  if ((result = sp_vec_copy_init(element_align, element_sz, copy))) {
    sp_vec_copy_internal_init(result, capacity, element_align, element_sz,
                              copy);
  }
  return result;
}

int
sp_vec_copy_free(struct sp_vec_copy **pself)
{
  assertx(pself);
  if (*pself) {
    struct sp_vec_copy *self = *pself;
    sp_vec_copy_internal_free(self);
    free(self);
    *pself = NULL;
  }
  return 0;
}

//==============================
size_t
sp_vec_copy_length(const struct sp_vec_copy *self)
{
  return self->length;
}

size_t
sp_vec_copy_capacity(const struct sp_vec_copy *self)
{
  return self->capacity;
}

//==============================
bool
sp_vec_copy_is_empty(const struct sp_vec_copy *self)
{
  return self->length == 0;
}

//==============================
sp_T *
sp_vec_copy_get(struct sp_vec_copy *self, size_t idx)
{
  if (idx < self->length) {
    return self->raw + (idx * self->element_sz);
  }
  return NULL;
}

const sp_T *
sp_vec_copy_get_c(const struct sp_vec_copy *self, size_t idx)
{
  if (idx < self->length) {
    return self->raw + (idx * self->element_sz);
  }
  return NULL;
}

sp_T *
sp_vec_copy_get_last(struct sp_vec_copy *self);

const sp_T *
sp_vec_copy_get_last_c(const struct sp_vec_copy *self);

//==============================
static bool
__sp_vec_copy_ensure_capacity(struct sp_vec_copy *self, size_t capacity)
{
  if (capacity > self->capacity) {
    struct sp_vec_copy *tmp;
    size_t new_capacity = sp_max(sp_max(16, self->capacity * 2), capacity);

    tmp = sp_vec_copy_init_cap(new_capacity, self->element_align,
                               self->element_sz, self->copy);
    if (!tmp) {
      return false;
    }

    __sp_vec_copy_copy(/*dest*/ tmp, self);
    sp_vec_copy_swap_self(self, tmp);
    sp_vec_copy_free(&tmp);
  }
  return true;
}

static void
__sp_vec_copy_copy(struct sp_vec_copy *dest, const struct sp_vec_copy *src)
{
  size_t i;
  assertx(dest);
  assertx(src);
  assertx(src->copy == dest->copy);
  assertx(src->length <= (dest->capacity - dest->length));
  assertxs(src->element_sz == dest->element_sz, "%zu:%zu", src->element_sz,
           dest->element_sz);
  assertxs(src->element_align == dest->element_align, "%zu:%zu",
           src->element_align, dest->element_align);

  for (i = 0; i < src->length; ++i) {
    sp_T *s, *d;

    s = src->raw + (i * src->element_sz);
    d = dest->raw + (dest->length++ * dest->element_sz);

    dest->copy(d, s, dest->element_sz);
  }
}

sp_T *
sp_vec_copy_append_impl(struct sp_vec_copy *self, const sp_T *in)
{
  sp_T *result;
  size_t idx;

  assertx(self);
  assertx(in);

  assertx(self->length <= self->capacity);

  if (!__sp_vec_copy_ensure_capacity(self, self->length + 1)) {
    return NULL;
  }

  idx    = self->length++;
  result = self->raw + (idx * self->element_sz);
  self->copy(result, in, self->element_sz);
  assertx(idx == sp_vec_copy_index_of(self, result));

  return result;
}

int
sp_vec_copy_append_vec(struct sp_vec_copy *self, struct sp_vec_copy *);

//==============================
void
sp_vec_copy_remove(struct sp_vec_copy *self, size_t idx)
{
  assertx(self);
  assertx(idx < self->length);

  if (idx < self->length) {
    const size_t last = self->length - 1;

    if (idx != last) {
      sp_vec_copy_swap(self, idx, last);
    }

    --self->length;
  }
}

//==============================
bool
sp_vec_copy_swap(struct sp_vec_copy *self, size_t f, size_t s)
{
  assertx(f < self->length);
  assertx(s < self->length);

  if (f < self->length && s < self->length) {
    //TODO improve (maybe use a free slot in self->raw instead of $tmp)
    void *tmp = aligned_alloc(self->element_align, self->element_sz);
    void *fp  = self->raw + (f * self->element_sz);
    void *sp  = self->raw + (s * self->element_sz);

    self->copy(tmp, fp, self->element_sz);
    self->copy(fp, sp, self->element_sz);
    self->copy(sp, tmp, self->element_sz);

    free(tmp);
    return true;
  }

  assertx(false);
  return false;
}

//==============================
size_t
sp_vec_copy_index_of(const struct sp_vec_copy *self, sp_T *n)
{
  uintptr_t n_idx     = (uintptr_t)n;
  uintptr_t r_idx     = (uintptr_t)self->raw;
  uintptr_t r_idx_end = r_idx + (self->element_sz * self->capacity);
  assertx(n_idx >= r_idx);
  assertx(n_idx < r_idx_end);
  assertx(n_idx % self->element_align == 0);
  assertx((n_idx - r_idx) % self->element_sz == 0);

  return (n_idx - r_idx) / self->element_sz;
}

//==============================
int
sp_vec_copy_sort(struct sp_vec_copy *self, sp_cb_cmp);

//==============================
int
sp_vec_copy_for_each(struct sp_vec_copy *self, void *closure, sp_cb_it);

//==============================
int
sp_vec_copy_clear(struct sp_vec_copy *self)
{
  assertx(self);
  self->length = 0;
  return 0;
}

//==============================
sp_T *
sp_vec_copy_array(struct sp_vec_copy *self)
{
  return self->raw;
}

//==============================
sp_T *
sp_vec_copy_begin(struct sp_vec_copy *self)
{
  return self->raw;
}

sp_T *
sp_vec_copy_end(struct sp_vec_copy *self)
{
  return self->raw + (self->length * self->element_sz);
}

sp_T *
sp_vec_copy_next(struct sp_vec_copy *self, sp_T *it)
{
  return it + self->element_sz;
}

//==============================
