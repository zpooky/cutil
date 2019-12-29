#include "sp_svec.h"
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdalign.h>
#include <string.h>

#include "sp_util.h"

//==============================
struct sp_svec {
  sp_svec_dtor dtor;
  sp_svec_move move;
  void *buf;
  size_t length;
  size_t capacity;
  uintptr_t align;
  size_t szof;
};

//==============================
static size_t dummy_default_sz = 0;
static int
sp_svec_default_move(sp_svec_T *from, sp_svec_T *to)
{
  assert(from);
  assert(to);
  assert(dummy_default_sz > 0);

  memcpy(to, from, dummy_default_sz);
  return 0;
}

//==============================
struct sp_svec *
sp_svec_init_impl_fp(size_t a,
                     size_t sz,
                     size_t cap,
                     sp_svec_dtor dtor,
                     sp_svec_move move)
{
  struct sp_svec *result;

  /* require both or none */
  assert((dtor && move) || (!dtor && !move));

  if ((result = calloc(1, sizeof(*result)))) {
    result->dtor  = dtor;
    result->move  = move ? move : sp_svec_default_move;
    result->align = a;
    result->szof  = sz;

    posix_memalign(&result->buf, a, sz * cap);
    if (result->buf) {
      result->capacity = cap;
    }
  }

  return result;
}

struct sp_svec *
sp_svec_init_impl(size_t a, size_t sz)
{
  return sp_svec_init_impl_fp(a, sz, 0, NULL, NULL);
}

struct sp_svec *
sp_svec_init_impl_cap(size_t a, size_t sz, size_t capacity)
{
  return sp_svec_init_impl_fp(a, sz, capacity, NULL, NULL);
}

int
sp_svec_free(struct sp_svec **pself)
{
  assert(pself);

  struct sp_svec *self = *pself;
  if (self) {
    if (self->buf) {
      sp_svec_clear(self->buf);
      free(self->buf);
    }

    free(self);
    *pself = NULL;
  }

  return 0;
}

//==============================
/* int
 * sp_svec_swap_self(struct sp_svec *f, struct sp_svec *s)
 * {
 *   #<{(| sp_svec_dtor dtor; |)}>#
 *   #<{(| sp_svec_move move; |)}>#
 * 
 *   #<{(| sp_util_swap_voidp(&f->dtor, &s->dtor); |)}>#
 *   #<{(| sp_util_swap_voidp(&f->move, &s->move); |)}>#
 *   #<{(| TODO |)}>#
 * 
 *   sp_util_swap_voidp(&f->buf, &s->buf);
 * 
 *   sp_util_swap_size_t(&f->length, &s->length);
 *   sp_util_swap_size_t(&f->capacity, &s->capacity);
 *   sp_util_swap_size_t(&f->align, &s->align);
 *   sp_util_swap_size_t(&f->szof, &s->szof);
 * 
 *   return 0;
 * }
 */

//==============================
int
sp_svec_clear(struct sp_svec *self)
{
  assert(self);

  if (self->dtor) {
    size_t i;
    for (i = 0; i < self->length; ++i) {
      sp_svec_T *cur;
      cur = sp_svec_get(self, i);
      self->dtor(cur);
    }
  }
  self->length = 0;

  return 0;
}

//==============================
size_t
sp_svec_length(const struct sp_svec *self)
{
  assert(self);

  return self->length;
}

size_t
sp_svec_capacity(const struct sp_svec *self)
{
  assert(self);

  return self->capacity;
}

//==============================
bool
sp_svec_is_empty(const struct sp_svec *self)
{
  return sp_svec_length(self) == 0;
}

//==============================
sp_svec_T *
sp_svec_get(struct sp_svec *self, size_t idx)
{
  assert(self);

  size_t off = idx * self->szof;

  return (void *)(((uintptr_t)self->buf) + off);
}

//==============================
static int
sp_svec_move_all(struct sp_svec *dest, struct sp_svec *src)
{
  size_t i;
  for (i = 0; i < sp_svec_length(src); ++i) {
    sp_svec_T *cur;
    cur = sp_svec_get(src, i);

    sp_svec_append_impl(dest, cur);
  }

  src->length = 0;

  return 0;
}

sp_svec_T *
sp_svec_append_impl(struct sp_svec *self, sp_svec_T *in)
{
  size_t idx;
  sp_svec_T *result = NULL;

  assert(self);
  assert(in);
  assert(self->length <= self->capacity);

  if (self->length == self->capacity) {
    struct sp_svec *tmp;
    size_t cap = sp_util_max(16, self->capacity * 2);

    tmp = sp_svec_init_impl_cap(self->align, self->szof, cap);
    if (!tmp) {
      return NULL;
    }
    tmp->move = self->move;

    sp_svec_move_all(/*dest*/ tmp, /*src*/ self);

    sp_util_swap_voidp(&self->buf, &tmp->buf);
    sp_util_swap_size_t(&self->length, &tmp->length);
    sp_util_swap_size_t(&self->capacity, &tmp->capacity);

    sp_svec_free(&tmp);
  }

  idx    = self->length++;
  result = sp_svec_get(self, idx);

  dummy_default_sz = self->szof;
  self->move(in, result);
  dummy_default_sz = 0;

  return result;
}

//==============================
int
sp_svec_remove(struct sp_svec *self, size_t idx)
{
  int res = -EFAULT;

  assert(self);

  if (idx < self->length) {
    size_t last = self->length - 1;
    if (last != idx) {
      sp_svec_swap(self, idx, last);
    }

    if (self->dtor) {
      sp_svec_T *l;
      l = sp_svec_get(self, last);
      self->dtor(l);
    }
    self->length--;

    res = 0;
  }

  return res;
}

//==============================
#define alignto(p, bits) (((p) >> bits) << bits)
#define aligntonext(p, bits)                                                   \
  alignto(((p) + (((uintptr_t)1) << bits) - ((uintptr_t)1)), bits)

bool
sp_svec_swap(struct sp_svec *self, size_t f, size_t s)
{
  assert(self);

  if (f < self->length && s < self->length) {
    sp_svec_T *first, *second;
    const size_t len = self->szof + self->align;
    char tmp[len];
    void *atmp = (void *)aligntonext((uintptr_t)tmp, self->align);

    first  = sp_svec_get(self, f);
    second = sp_svec_get(self, s);

    assert(first);
    assert(second);

    dummy_default_sz = self->szof;
    self->move(first, atmp);
    self->move(second, first);
    self->move(tmp, second);
    dummy_default_sz = 0;

    return true;
  }

  return false;
}

//==============================
