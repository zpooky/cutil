#include "sp_heap.h"
#include "sp_vec.h"

#include <stdlib.h>
#include <assert.h>

//==============================
struct sp_heap {
  struct sp_vec *impl;
  sp_heap_cmp_cb cmp;
};

//==============================
struct sp_heap *
sp_heap_init(sp_heap_cmp_cb cmp)
{
  struct sp_heap *result;

  if ((result = calloc(1, sizeof(*result)))) {
    result->impl = sp_vec_init();
    result->cmp  = cmp;
  }

  return result;
}

//==============================
static size_t
sp_heap_parent(size_t idx)
{
  return (idx - 1) / 2;
}

static size_t
sp_heap_left_child(size_t idx)
{
  return (2 * idx) + 1;
}

static size_t
sp_heap_right_child(size_t idx)
{
  return (2 * idx) + 2;
}

static size_t
sp_heap_shift_up(struct sp_heap *self, size_t idx)
{
  size_t par_idx;

Lit:
  if (idx == 0) {
    /* we are root, we can not shift up further */
    return idx;
  }

  par_idx = sp_heap_parent(idx);
  if (self->cmp(sp_vec_get(self->impl, idx), sp_vec_get(self->impl, par_idx)) >
      0) {
    sp_vec_swap(self->impl, idx, par_idx);
    idx = par_idx;

    goto Lit;
  }

  return idx;
}

sp_heap_T **
sp_heap_enqueue_impl(struct sp_heap *self, sp_heap_T *in)
{
  sp_heap_T **res;

  if ((res = sp_vec_append(self->impl, in))) {
    size_t idx;

    idx = sp_vec_index_of(self->impl, res);
    assert(idx != sp_vec_capacity(self->impl));

    idx = sp_heap_shift_up(self, idx);
    res = sp_vec_array(self->impl) + idx;
  }

  return res;
}

//==============================
static size_t
sp_heap_extreme(struct sp_heap *self, size_t first, size_t second)
{
  sp_heap_T *f, *s;

  f = sp_vec_get(self->impl, first);
  s = sp_vec_get(self->impl, second);

  if (self->cmp(f, /*>*/ s) > 0) {
    return first;
  }

  return second;
}

static size_t
sp_heap_shift_down(struct sp_heap *self, size_t idx)
{
  size_t extreme_idx;
  size_t left_idx;
  size_t right_idx;

Lit:
  extreme_idx = sp_heap_length(self);

  left_idx = sp_heap_left_child(idx);
  if (left_idx < sp_heap_length(self)) {
    extreme_idx = left_idx;

    right_idx = sp_heap_right_child(idx);
    if (right_idx < sp_heap_length(self)) {
      extreme_idx = sp_heap_extreme(self, right_idx, left_idx);
    }
  }

  if (extreme_idx < sp_heap_length(self)) {
    sp_heap_T *f, *s;

    f = sp_vec_get(self->impl, extreme_idx);
    s = sp_vec_get(self->impl, idx);

    if (self->cmp(f, s) > 0) {
      sp_vec_swap(self->impl, idx, extreme_idx);

      idx = extreme_idx;
      goto Lit;
    }
  }

  return idx;
}

bool
sp_heap_dequeue_impl(struct sp_heap *self, sp_heap_T **out)
{
  assert(out);

  if (!sp_heap_is_empty(self)) {
    void *res;
    const size_t head = 0;
    const size_t last = sp_vec_length(self->impl) - 1;

    *out = sp_vec_get(self->impl, head);
    sp_vec_swap(self->impl, head, last);

    res = sp_vec_remove(self->impl, last);
    assert(res);

    sp_heap_shift_down(self, head);

    return true;
  }

  *out = NULL;

  return false;
}

//==============================
static sp_heap_T **
sp_heap_decrease_key(struct sp_heap *self, sp_heap_T **subject)
{
  size_t idx;

  idx = sp_vec_index_of(self->impl, subject);
  assert(idx != sp_vec_capacity(self->impl));

  idx = sp_heap_shift_up(self, idx);

  return sp_vec_array(self->impl) + idx;
}

static sp_heap_T *
sp_heap_increase_key(struct sp_heap *self, sp_heap_T *subject)
{
  size_t idx;

  idx = sp_vec_index_of(self->impl, subject);
  assert(idx != sp_vec_capacity(self->impl));

  idx = sp_heap_shift_down(self, idx);

  return sp_vec_array(self->impl) + idx;
}

sp_heap_T **
sp_heap_update_key_impl(struct sp_heap *self, sp_heap_T **subject)
{
  assert(self);
  assert(subject);

  subject = sp_heap_decrease_key(self, subject);
  subject = sp_heap_increase_key(self, subject);

  return subject;
}

//==============================
size_t
sp_heap_length(const struct sp_heap *self)
{
  return sp_vec_length(self->impl);
}

size_t
sp_heap_capacity(const struct sp_heap *self)
{
  return sp_vec_capacity(self->impl);
}

//==============================
bool
sp_heap_is_empty(const struct sp_heap *self)
{
  return sp_vec_is_empty(self->impl);
}

//==============================
sp_heap_T **
sp_heap_array(struct sp_heap *self)
{
  assert(self);

  return sp_vec_array(self->impl);
}

//==============================
int
sp_heap_clear(struct sp_heap *self)
{
  return sp_vec_clear(self->impl);
}

//==============================
int
sp_heap_free(struct sp_heap **pself)
{
  assert(pself);

  if (*pself) {
    struct sp_heap *self = *pself;

    sp_vec_free(&self->impl);
    free(self);

    *pself = NULL;
  }

  return 0;
}

//==============================
