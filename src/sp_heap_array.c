#include "sp_heap_array.h"
#include "sp_heap_internal.h"

static sp_T *
sp_heap_array_get(void *array, size_t esize, size_t idx)
{
  return array + (esize * idx);
}

static void
sp_heap_array_swap(void *array, size_t esize, size_t idx0, size_t idx1)
{
}

static size_t
shift_up(void *array, size_t esize, sp_cb_cmp cmp, size_t idx)
{

  while (true) {
    sp_T *parent;
    sp_T *me;
    size_t par_idx;

    if (idx == 0) {
      /* we are root, we can not shift up further */
      return idx;
    }

    par_idx = sp_heap_util_parent(idx);
    me      = sp_heap_array_get(array, esize, idx);
    parent  = sp_heap_array_get(array, esize, par_idx);

    if (cmp(me, parent) > 0) {
      sp_heap_array_swap(array, esize, idx, par_idx);
      idx = par_idx;
    } else {
      break;
    }
  }

  return idx;
}

sp_T *
sp_heap_array_enqueue_move(void *array,
                           size_t capacity,
                           size_t *l_array,
                           size_t esize,
                           sp_cb_cmp cmp,
                           sp_T *in)
{
  sp_T *res;

  if (capacity == 0) {
    return NULL;
  }
#if 0
  if ((res = sp_vec_copy_append(self->vec, in))) {
    size_t idx;

    idx = sp_vec_copy_index_of(self->vec, res);
    idx = shift_up(array, esize, cmp, idx);

    return sp_heap_array_get(array, esize, idx);
  }
#endif

  return NULL;
}
