#include "sp_vec.h"
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <memory.h>

#include "sp_util.h"

//==============================
struct sp_vec {
  sp_vec_T **entries;
  size_t length;
  size_t capacity;
};

//==============================
struct sp_vec *
sp_vec_init(void)
{
  return calloc(1, sizeof(struct sp_vec));
}

struct sp_vec *
sp_vec_init_cap(size_t capacity)
{
  struct sp_vec *result;

  if ((result = sp_vec_init())) {
    result->entries  = calloc(capacity, sizeof(sp_vec_T *));
    result->capacity = capacity;
  }

  return result;
}

int
sp_vec_free(struct sp_vec **pself)
{
  assert(pself);

  struct sp_vec *self = *pself;
  if (self) {
    if (self->entries) {
      free(self->entries);
    }

    free(self);
    *pself = NULL;
  }

  return 0;
}

//==============================
size_t
sp_vec_length(const struct sp_vec *self)
{
  if (!self) {
    fflush(stdout);
    fflush(stderr);
    assert(self);
  }
  return self->length;
}

size_t
sp_vec_capacity(const struct sp_vec *self)
{
  assert(self);

  return self->capacity;
}

//==============================
bool
sp_vec_is_empty(const struct sp_vec *self)
{
  return sp_vec_length(self) == 0;
}

//==============================
sp_vec_T *
sp_vec_get(struct sp_vec *self, size_t idx)
{
  assert(self);

  if (idx < self->length) {
    return self->entries[idx];
  }

  return NULL;
}

const sp_vec_T *
sp_vec_get_c(const struct sp_vec *self, size_t idx)
{
  assert(self);

  if (idx < self->length) {
    return self->entries[idx];
  }

  return NULL;
}

sp_vec_T *
sp_vec_get_last(struct sp_vec *self)
{
  assert(self);

  if (self->length > 0) {
    return self->entries[self->length - 1];
  }

  return NULL;
}

const sp_vec_T *
sp_vec_get_last_c(const struct sp_vec *self)
{
  assert(self);

  if (self->length > 0) {
    return self->entries[self->length - 1];
  }

  return NULL;
}

//==============================
static void
sp_vec_swap_self(struct sp_vec *f, struct sp_vec *s)
{
  assert(f);
  assert(s);

  void **tmp = f->entries;
  f->entries = s->entries;
  s->entries = tmp;

  sp_util_swap_size_t(&f->length, &s->length);
  sp_util_swap_size_t(&f->capacity, &s->capacity);
}

static void
sp_vec_copy(struct sp_vec *dest, struct sp_vec *src)
{
  sp_vec_T **raw_dest;

  assert(dest);
  assert(src);
  assert(src->length <= (dest->capacity - dest->length));

  raw_dest = &dest->entries[dest->length];
  memcpy(raw_dest, src->entries, sizeof(sp_vec_T *) * src->length);
  dest->length += src->length;
}

sp_vec_T **
sp_vec_append_impl(struct sp_vec *self, sp_vec_T *in)
{
  size_t idx;

  assert(self);
  assert(in);

  assert(self->length <= self->capacity);

  if (self->length == self->capacity) {
    struct sp_vec *tmp;

    tmp = sp_vec_init_cap(sp_util_max(16, self->capacity * 2));
    if (!tmp) {
      return NULL;
    }

    sp_vec_copy(/*dest*/ tmp, self);
    sp_vec_swap_self(self, tmp);
    sp_vec_free(&tmp);
  }

  idx                = self->length++;
  self->entries[idx] = in;

  return self->entries + idx;
}

int
sp_vec_append_vec(struct sp_vec *self, struct sp_vec *appends)
{
  sp_vec_T **it = NULL;

  assert(self);
  assert(appends);

  sp_vec_for_each2 (it, appends) {
    sp_vec_append(self, *it);
  }

  return 0;
}

//==============================
sp_vec_T *
sp_vec_remove(struct sp_vec *self, size_t idx)
{
  sp_vec_T *result = NULL;

  assert(self);

  if (idx < self->length) {
    result      = sp_vec_get(self, idx);
    size_t last = self->length - 1;

    if (idx != last) {
      sp_vec_swap(self, idx, last);
    }

    self->entries[last] = NULL;

    --self->length;
  }

  return result;
}

//==============================
bool
sp_vec_swap(struct sp_vec *self, size_t f, size_t s)
{
  assert(f < self->length);
  assert(s < self->length);

  if (f < self->length && s < self->length) {
    sp_util_swap_voidp(self->entries + f, self->entries + s);

    return true;
  }

  return false;
}

//==============================
size_t
sp_vec_index_of_impl(const struct sp_vec *self, sp_vec_T **data)
{
  uintptr_t dptr = (uintptr_t)data, sptr;
  size_t index;

  assert(self);
  assert(data);

  sptr = (uintptr_t)self->entries;
  assert(dptr >= sptr);

  dptr = dptr - sptr;
  assert(dptr % sizeof(data) == 0);

  index = dptr / sizeof(data);

  if (index >= self->length) {
    index = sp_vec_capacity(self);
    assert(false);
  }

  return index;
}

//==============================
int
sp_vec_sort(struct sp_vec *self, sp_vec_cmp_cb cmp)
{
  assert(self);
  sp_util_sort(self->entries, self->length, cmp);
  return 0;
}

//==============================
int
sp_vec_for_each(struct sp_vec *self, void *closure, sp_vec_it_cb it)
{
  if (self) {
    size_t i;

    for (i = 0; i < self->length; ++i) {
      int res;

      res = it(self->entries[i], closure);
      if (res != 0) {
        return res;
      }
    }
  }

  return 0;
}

//==============================
int
sp_vec_clear(struct sp_vec *self)
{
  assert(self);
  //TODO memset 0

  self->length = 0;
  return 0;
}

//==============================
sp_vec_T **
sp_vec_array(struct sp_vec *self)
{
  assert(self);

  return self->entries;
}

//==============================
sp_vec_T **
sp_vec_begin(struct sp_vec *self)
{
  if (self) {
    if (self->entries) {
      return self->entries + 0;
    }
  }

  return NULL;
}

sp_vec_T **
sp_vec_end(struct sp_vec *self)
{
  if (self) {
    if (self->entries) {
      return self->entries + self->length;
    }
  }

  return NULL;
}

//==============================
