#include "sp_heap.h"
#include "sp_vec.h"
#include "sp_util.h"
#include "sp_heap_internal.h"

#include <stdlib.h>
#include <assert.h>

//==============================
struct sp_heap {
  struct sp_vec *vec;
  sp_heap_cmp_cb cmp;
};

//==============================
struct sp_heap *
sp_heap_init(sp_heap_cmp_cb cmp)
{
  struct sp_heap *result;

  if ((result = calloc(1, sizeof(*result)))) {
    result->vec = sp_vec_new();
    result->cmp = cmp;
  }

  return result;
}

//==============================
static void
sp_heap_internal_swap(struct sp_heap *self, size_t idx0, size_t idx1)
{
  bool res;
  sp_heap_T *first;
  sp_heap_T *second;

  first = sp_vec_get(self->vec, idx0);
  assert(first);
  assert(first->idx == idx0);

  second = sp_vec_get(self->vec, idx1);
  assert(second);
  assert(second->idx == idx1);

  sp_util_swap_size_t(&first->idx, &second->idx);

  res = sp_vec_swap(self->vec, idx0, idx1);
  assert(res);
}

static size_t
sp_heap_shift_up(struct sp_heap *self, size_t idx)
{
  while (true) {
    sp_vec_T *parent;
    size_t par_idx;
    sp_vec_T *me;

    if (idx == 0) {
      /* we are root, we can not shift up further */
      return idx;
    }

    par_idx = sp_heap_util_parent(idx);
    me      = sp_vec_get(self->vec, idx);
    parent  = sp_vec_get(self->vec, par_idx);

    if (self->cmp(me, parent) > 0) {
      sp_heap_internal_swap(self, idx, par_idx);
      idx = par_idx;
    } else {
      break;
    }
  }

  return idx;
}

sp_heap_T *
sp_heap_enqueue_impl(struct sp_heap *self, sp_heap_T *in)
{
  sp_heap_T **res;

  if ((res = sp_vec_append(self->vec, in))) {
    size_t idx;

    in->idx = sp_vec_index_of(self->vec, res);
    assert(in->idx != sp_vec_capacity(self->vec));
    assert(*res == in);

    idx = sp_heap_shift_up(self, in->idx);
    assert(in->idx == idx);
    assert(in == sp_vec_array(self->vec)[idx]);

    return in;
  }

  return NULL;
}

//==============================
static size_t
sp_heap_extreme(struct sp_heap *self, size_t first, size_t second)
{
  sp_heap_T *f, *s;

  f = sp_vec_get(self->vec, first);
  s = sp_vec_get(self->vec, second);

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

  left_idx = sp_heap_util_left_child(idx);
  if (left_idx < sp_heap_length(self)) {
    extreme_idx = left_idx;

    right_idx = sp_heap_util_right_child(idx);
    if (right_idx < sp_heap_length(self)) {
      extreme_idx = sp_heap_extreme(self, right_idx, left_idx);
    }
  }

  if (extreme_idx < sp_heap_length(self)) {
    sp_heap_T *f, *s;

    f = sp_vec_get(self->vec, extreme_idx);
    s = sp_vec_get(self->vec, idx);

    if (self->cmp(f, s) > 0) {
      sp_heap_internal_swap(self, idx, extreme_idx);

      idx = extreme_idx;
      goto Lit;
    }
  }

  return idx;
}

bool
sp_heap_dequeue_impl(struct sp_heap *self, sp_heap_T **out, sp_heap_T *dummy)
{
  (void)dummy;
  assert(self);
  assert(out);

  if (!sp_heap_is_empty(self)) {
    sp_vec_T *res;
    const size_t head = 0;
    const size_t last = sp_vec_length(self->vec) - 1;

    *out = sp_vec_get(self->vec, head);
    sp_heap_internal_swap(self, head, last);

    res = sp_vec_remove(self->vec, last);
    assert(res);

    sp_heap_shift_down(self, head);

    return true;
  }

  *out = NULL;

  return false;
}

//==============================
void *
sp_heap_head(struct sp_heap *self)
{
  assert(self);

  return sp_vec_get(self->vec, 0);
}

//==============================
bool
sp_heap_remove_impl(struct sp_heap *self, sp_heap_T *in)
{
  /* 1. swap(in->idx, self->vec.last.idx);
   * 2. drop last
   * 3. update_key(in->idx) # adjust to position of the previously last item
   */
  assert(self);
  assert(in);
  size_t idx = in->idx;
  sp_heap_T *last;

  if ((last = sp_vec_get_last(self->vec))) {
    if (idx != last->idx) {
      size_t last_idx = last->idx;
      assert(idx < last_idx);
      assert(last_idx == sp_vec_length(self->vec) - 1);

      sp_heap_internal_swap(self, idx, last_idx);
      assert(last->idx == idx);
      assert(last == sp_vec_get(self->vec, idx));

      sp_heap_update_key_impl(self, last);
    } else {
      sp_vec_remove(self->vec, idx);
    }
  } else {
    /* $in was not in heap */
    assert(false);
    return false;
  }

  return true;
}

//==============================
void
sp_heap_update_key_impl(struct sp_heap *self, sp_heap_T *subject)
{
  size_t idx;
  assert(self);
  assert(subject);

  idx = sp_heap_shift_up(self, subject->idx);
  assert(idx == subject->idx);

  idx = sp_heap_shift_down(self, subject->idx);
  assert(idx == subject->idx);
}

//==============================
size_t
sp_heap_length(const struct sp_heap *self)
{
  return sp_vec_length(self->vec);
}

size_t
sp_heap_capacity(const struct sp_heap *self)
{
  return sp_vec_capacity(self->vec);
}

//==============================
bool
sp_heap_is_empty(const struct sp_heap *self)
{
  return sp_vec_is_empty(self->vec);
}

//==============================
sp_heap_T **
sp_heap_array(struct sp_heap *self)
{
  assert(self);

  return (sp_heap_T **)sp_vec_array(self->vec);
}

//==============================
int
sp_heap_clear(struct sp_heap *self)
{
  return sp_vec_clear(self->vec);
}

//==============================
int
sp_heap_free(struct sp_heap **pself)
{
  assert(pself);

  if (*pself) {
    struct sp_heap *self = *pself;

    sp_vec_free(&self->vec);
    free(self);

    *pself = NULL;
  }

  return 0;
}

//==============================
