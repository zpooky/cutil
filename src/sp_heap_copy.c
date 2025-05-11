#include "sp_heap_copy.h"

#include <stdint.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>

#include "sp_vec_copy.h"
#include "sp_vec_copy_internal.h"
#include "sp_heap_internal.h"

//==============================
struct sp_heap_copy {
  struct sp_vec_copy *vec;

  sp_heap_copy_cmp_cb cmp;
  sp_heap_copy_copy_cb copy;
};

//==============================
static void
sp_heap_copy_copy_memcopy(sp_heap_copy_T *dest,
                          const sp_heap_copy_T *src,
                          size_t element_sz)
{
  memcpy(dest, src, element_sz);
}

struct sp_heap_copy *
sp_heap_copy_init(size_t element_align, size_t element_sz, sp_heap_copy_cmp_cb cmp, sp_heap_copy_copy_cb copy) {
  struct sp_heap_copy *result;
  if ((result = calloc(1, sizeof(*result)))) {
    result->vec  = sp_vec_copy_init(element_align, element_sz, (sp_vec_copy_copy_cb)copy);
    result->copy = copy;
    result->cmp  = cmp;
  }
  return result;
}

struct sp_heap_copy *
sp_heap_copy_init2(size_t element_align, size_t element_sz, sp_heap_copy_cmp_cb cmp)
{
  return sp_heap_copy_init(element_align, element_sz, cmp, sp_heap_copy_copy_memcopy);
}

struct sp_heap_copy *
sp_heap_copy_init_copy(const struct sp_heap_copy *o){
  struct sp_heap_copy *result;
  if ((result = calloc(1, sizeof(*result)))) {
    result->vec  = sp_vec_copy_init_copy(o->vec);
    result->copy = o->copy;
    result->cmp  = o->cmp;
  }
  return result;
}

int
sp_heap_copy_free(struct sp_heap_copy **pself)
{
  assert(pself);
  if (*pself) {
    struct sp_heap_copy *self = *pself;
    sp_vec_copy_free(&self->vec);
    self->copy = NULL;
    self->cmp  = NULL;

    free(self);
    *pself = NULL;
  }

  return 0;
}

//==============================
static size_t
sp_heap_copy_shift_up(struct sp_heap_copy *self, size_t idx)
{

  while (true) {
    sp_vec_copy_T *parent;
    sp_vec_copy_T *me;
    size_t par_idx;

    if (idx == 0) {
      /* we are root, we can not shift up further */
      return idx;
    }

    par_idx = sp_heap_util_parent(idx);
    me      = sp_vec_copy_get(self->vec, idx);
    parent  = sp_vec_copy_get(self->vec, par_idx);

    if (self->cmp(me, parent) > 0) {
      sp_vec_copy_swap(self->vec, idx, par_idx);
      idx = par_idx;
    } else {
      break;
    }
  }

  return idx;
}

sp_heap_copy_T *
sp_heap_copy_enqueue(struct sp_heap_copy *self, const sp_heap_copy_T *in)
{
  sp_heap_copy_T *res;

  if ((res = sp_vec_copy_append(self->vec, in))) {
    size_t idx;

    idx = sp_vec_copy_index_of(self->vec, res);
    idx = sp_heap_copy_shift_up(self, idx);

    return sp_vec_copy_get(self->vec, idx);
  }

  return NULL;
}

//==============================
static size_t
sp_heap_copy_extreme(struct sp_heap_copy *self, size_t first, size_t second)
{
  sp_heap_copy_T *f, *s;

  f = sp_vec_copy_get(self->vec, first);
  s = sp_vec_copy_get(self->vec, second);

  if (self->cmp(f, /*>*/ s) > 0) {
    return first;
  }

  return second;
}

static size_t
sp_heap_copy_shift_down(struct sp_heap_copy *self, size_t idx)
{
  size_t extreme_idx;
  size_t left_idx;
  size_t right_idx;

Lit:
  extreme_idx = sp_heap_copy_length(self);

  left_idx = sp_heap_util_left_child(idx);
  if (left_idx < sp_heap_copy_length(self)) {
    extreme_idx = left_idx;

    right_idx = sp_heap_util_right_child(idx);
    if (right_idx < sp_heap_copy_length(self)) {
      extreme_idx = sp_heap_copy_extreme(self, right_idx, left_idx);
    }
  }

  if (extreme_idx < sp_heap_copy_length(self)) {
    sp_heap_copy_T *f, *s;

    f = sp_vec_copy_get(self->vec, extreme_idx);
    s = sp_vec_copy_get(self->vec, idx);

    if (self->cmp(f, s) > 0) {
      sp_vec_copy_swap(self->vec, idx, extreme_idx);

      idx = extreme_idx;
      goto Lit;
    }
  }

  return idx;
}
bool
sp_heap_copy_dequeue(struct sp_heap_copy *self, sp_heap_copy_T *dest)
{
  assert(self);
  assert(dest);

  if (!sp_heap_copy_is_empty(self)) {
    sp_heap_copy_T *src;
    const size_t head = 0;
    const size_t last = sp_vec_copy_length(self->vec) - 1;

    src = sp_vec_copy_get(self->vec, head);
    assert(src);
    self->copy(dest, src, self->vec->element_sz);
    sp_vec_copy_swap(self->vec, head, last);

    sp_vec_copy_remove(self->vec, last);

    sp_heap_copy_shift_down(self, head);

    return true;
  }

  return false;
}

//==============================
sp_heap_copy_T *
sp_heap_copy_head(struct sp_heap_copy *self)
{
  assert(self);
  if (!sp_heap_copy_is_empty(self)) {
    return sp_vec_copy_get(self->vec, 0);
  }
  return NULL;
}

//==============================
bool
sp_heap_copy_drop_head(struct sp_heap_copy *self)
{
  assert(self);

  if (!sp_heap_copy_is_empty(self)) {
    const size_t head = 0;
    const size_t last = sp_vec_copy_length(self->vec) - 1;
    sp_vec_copy_swap(self->vec, head, last);
    sp_vec_copy_remove(self->vec, last);
    sp_heap_copy_shift_down(self, head);

    return true;
  }

  return false;
}

//==============================
bool
sp_heap_copy_remove_impl(struct sp_heap_copy *, sp_heap_copy_T *);

//==============================
sp_heap_copy_T *
sp_heap_copy_update_key(struct sp_heap_copy *self, sp_heap_copy_T *subject)
{
  size_t idx = sp_vec_copy_index_of(self->vec, subject);

  idx = sp_heap_copy_shift_down(self, idx);
  idx = sp_heap_copy_shift_up(self, idx);

  return sp_vec_copy_get(self->vec, idx);
}

//==============================
size_t
sp_heap_copy_length(const struct sp_heap_copy *self)
{
  assert(self);
  return sp_vec_copy_length(self->vec);
}

size_t
sp_heap_copy_capacity(const struct sp_heap_copy *);

//==============================
bool
sp_heap_copy_eq(const struct sp_heap_copy *f, const struct sp_heap_copy *s, sp_heap_copy_eq_cb eq) {
  assertx(f->cmp == s->cmp);
  size_t len = sp_heap_copy_length(f);
  if (len != sp_heap_copy_length(s)) {
    return false;
  }

  for (size_t i=0; i<len; ++i){
    const sp_vec_copy_T *f0 = sp_vec_copy_get_c(f->vec, i);
    const sp_vec_copy_T *s0 = sp_vec_copy_get_c(s->vec, i);
    if (!eq(f0,s0)){
      return false;
    }
  }

  return true;
}

//==============================
bool
sp_heap_copy_is_empty(const struct sp_heap_copy *self)
{
  return sp_vec_copy_is_empty(self->vec);
}

//==============================
sp_heap_copy_T *
sp_heap_copy_array(struct sp_heap_copy *self)
{
  return sp_vec_copy_array(self->vec);
}

//==============================
int
sp_heap_copy_clear(struct sp_heap_copy *self)
{
  return sp_vec_copy_clear(self->vec);
}

//==============================
