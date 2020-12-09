#include "sp_vec_copy.h"

#include "sp_vec_copy_internal.h"

#include <stdlib.h>
#include <memory.h>
#include <assert.h>

#include "sp_util.h"

//==============================
static void
sp_vec_copy_memcopy(sp_vec_copy_T *dest, const sp_vec_copy_T *src, size_t sz)
{
  memcpy(dest, src, sz);
}

struct sp_vec_copy *
sp_vec_copy_init(size_t align, size_t sz, sp_vec_copy_copy_cb copy)
{
  struct sp_vec_copy *result;
  if ((result = calloc(1, sizeof(*result)))) {
    sp_vec_copy_internal_init(result, 0, align, sz, copy);
  }
  return result;
}

struct sp_vec_copy *
sp_vec_copy_init0(size_t align, size_t sz)
{
  return sp_vec_copy_init(align, sz, sp_vec_copy_memcopy);
}

struct sp_vec_copy *
sp_vec_copy_init_cap(size_t capacity,
                     size_t align,
                     size_t sz,
                     sp_vec_copy_copy_cb copy)
{
  struct sp_vec_copy *result;

  if ((result = sp_vec_copy_init(align, sz, copy))) {
    sp_vec_copy_internal_init(result, capacity, align, sz, copy);
  }
  return result;
}

int
sp_vec_copy_free(struct sp_vec_copy **pself)
{
  assert(pself);
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
sp_vec_copy_T *
sp_vec_copy_get(struct sp_vec_copy *self, size_t idx)
{
  if (idx < self->length) {
    return self->raw + (idx * self->sz);
  }
  return NULL;
}

const sp_vec_copy_T *
sp_vec_copy_get_c(const struct sp_vec_copy *self, size_t idx)
{
  if (idx < self->length) {
    return self->raw + (idx * self->sz);
  }
  return NULL;
}

sp_vec_copy_T *
sp_vec_copy_get_last(struct sp_vec_copy *self);

const sp_vec_copy_T *
sp_vec_copy_get_last_c(const struct sp_vec_copy *self);

//==============================
static void
sp_vec_copy_copy(struct sp_vec_copy *dest, const struct sp_vec_copy *src)
{
  size_t i;
  assert(dest);
  assert(src);
  assert(src->length <= (dest->capacity - dest->length));
  assert(src->sz == dest->sz);
  assert(src->align == dest->align);

  for (i = 0; i < src->length; ++i) {
    sp_vec_copy_T *s, *d;

    s = src->raw + (i * src->sz);
    d = dest->raw + (dest->length++ * dest->sz);

    dest->copy(d, s, dest->sz);
  }
}

static void
sp_vec_copy_swap_self(struct sp_vec_copy *f, struct sp_vec_copy *s)
{
  sp_util_swap_voidp(&f->raw, &s->raw);

  sp_util_swap_size_t(&f->length, &s->length);
  sp_util_swap_size_t(&f->capacity, &s->capacity);

  sp_util_swap_size_t(&f->align, &s->align);
  sp_util_swap_size_t(&f->sz, &s->sz);

  sp_util_swap_voidp(&f->copy, &s->copy);
}

sp_vec_copy_T *
sp_vec_copy_append_impl(struct sp_vec_copy *self, const sp_vec_copy_T *in)
{
  size_t idx;
  sp_vec_copy_T *result;

  assert(self);
  assert(in);

  assert(self->length <= self->capacity);

  if (self->length == self->capacity) {
    struct sp_vec_copy *tmp;

    tmp = sp_vec_copy_init_cap(sp_util_max(16, self->capacity * 2), self->align,
                               self->sz, self->copy);
    if (!tmp) {
      return NULL;
    }

    sp_vec_copy_copy(/*dest*/ tmp, self);
    sp_vec_copy_swap_self(self, tmp);
    sp_vec_copy_free(&tmp);
  }

  idx    = self->length++;
  result = self->raw + (idx * self->sz);
  self->copy(result, in, self->sz);

  return result;
}

int
sp_vec_copy_append_vec(struct sp_vec_copy *self, struct sp_vec_copy *);

//==============================
void
sp_vec_copy_remove(struct sp_vec_copy *self, size_t idx)
{
  assert(self);
  assert(idx < self->length);

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
  assert(f < self->length);
  assert(s < self->length);

  if (f < self->length && s < self->length) {
    //TODO improve (maybe use a free slot in self->raw instead of $tmp)
    void *tmp = aligned_alloc(self->align, self->sz);
    void *fp  = self->raw + (f * self->sz);
    void *sp  = self->raw + (s * self->sz);

    self->copy(tmp, fp, self->sz);
    self->copy(fp, sp, self->sz);
    self->copy(sp, tmp, self->sz);

    free(tmp);
    return true;
  }

  assert(false);
  return false;
}

//==============================
size_t
sp_vec_copy_index_of(const struct sp_vec_copy *self, sp_vec_copy_T *n)
{
  uintptr_t nidx     = (uintptr_t)n;
  uintptr_t ridx     = (uintptr_t)self->raw;
  uintptr_t ridx_end = ridx + (self->sz * self->capacity);
  assert(nidx >= ridx);
  assert(nidx < ridx_end);
  assert(nidx % self->align == 0);

  return (ridx - nidx) / self->sz;
}

//==============================
typedef int (*sp_vec_copy_cmp_cb)(sp_vec_copy_T *, sp_vec_copy_T *);

int
sp_vec_copy_sort(struct sp_vec_copy *self, sp_vec_copy_cmp_cb);

//==============================
typedef int (*sp_vec_copy_it_cb)(sp_vec_copy_T *, void *closure);

int
sp_vec_copy_for_each(struct sp_vec_copy *self,
                     void *closure,
                     sp_vec_copy_it_cb);

//==============================
int
sp_vec_copy_clear(struct sp_vec_copy *self)
{
  assert(self);
  self->length = 0;
  return 0;
}

//==============================
sp_vec_copy_T *
sp_vec_copy_array(struct sp_vec_copy *self)
{
  return self->raw;
}

//==============================
sp_vec_copy_T *
sp_vec_copy_begin(struct sp_vec_copy *self)
{
  return self->raw;
}

sp_vec_copy_T *
sp_vec_copy_end(struct sp_vec_copy *self)
{
  return self->raw + (self->length * self->sz);
}

sp_vec_copy_T *
sp_vec_copy_next(struct sp_vec_copy *self, sp_vec_copy_T *it)
{
  return it + self->sz;
}

//==============================
