#include "sp_test_heap.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "sp_heap.h"
#include "sp_vec.h"
#include "sp_util.h"

static int
sp_test_size_MIN_cmp(size_t *f, size_t *s)
{
  assert(f);
  assert(s);
  return -sp_util_size_t_cmp(*f, *s);
}

static void
shuffle(struct sp_vec *arr)
{
  size_t i;
  for (i = 0; i < sp_vec_length(arr); ++i) {
    size_t idx = (size_t)rand() % sp_vec_length(arr);
    sp_vec_swap(arr, i, idx);
  }
}

static int
sp_test_heap_sequence(void)
{
  size_t i;
  size_t max = 1024;
  size_t **it;
  size_t *out;
  struct sp_vec /*size_t*/ *pool;
  struct sp_heap /*size_t*/ *heap;

  pool = sp_vec_init();
  heap = sp_heap_init((sp_heap_cmp_cb)sp_test_size_MIN_cmp);
  assert(heap);

  for (i = 0; i < max; ++i) {
    size_t **res;
    size_t *in;

    in = calloc(1, sizeof(*in));
    *in = i;
    assert(*in == i);

    res = sp_vec_append(pool, in);
    assert(res);
    assert(*res == in);
    assert(**res == i);
  }

  assert(sp_vec_length(pool) == max);
  i = 0;
  sp_vec_for_each2(it, pool)
  {
    /* printf("-%zu\n", **it); */
    assert(**it == i);
    ++i;
  }
  shuffle(pool);

  assert(sp_heap_is_empty(heap));

  sp_vec_for_each2(it, pool)
  {
    /* printf("-%zu\n", **it); */
    size_t **res;
    res = sp_heap_enqueue(heap, *it);
    assert(res);
    assert(*res);
    assert(*res == *it);
  }

  assert(sp_heap_length(heap) == max);

  i = 0;
  while (sp_heap_dequeue(heap, &out)) {
    assert(out);
    /* printf("- %zu\n", *out); */
    assert(i == *out);
    i++;
    free(out);
  }
  assert(i == max);
  assert(sp_heap_is_empty(heap));

  sp_vec_free(&pool);
  sp_heap_free(&heap);

  return 0;
}

static int
sp_test_heap_update(void)
{
  size_t i;
  size_t *out;
  struct sp_heap /*size_t*/ *heap;
  const size_t min = 256;
  heap = sp_heap_init((sp_heap_cmp_cb)sp_test_size_MIN_cmp);

  for (i = 512; i-- > min;) {
    size_t *in;
    size_t **res;

    in = calloc(1, sizeof(*in));
    *in = i;

    res = sp_heap_enqueue(heap, in);
    assert(res);
    assert(*res == in);
    assert(**res == i);
  }

  while (!sp_heap_is_empty(heap)) {
    size_t idx = (size_t)rand() % sp_heap_length(heap);
    size_t **subject = (size_t **)sp_heap_array(heap) + idx;
    size_t **res;

    **subject = 0;

    res = sp_heap_update_key(heap, subject);
    assert(res);
    assert(**res == 0);

    assert(sp_heap_dequeue(heap, &out));
    assert(out);
    assert(*out == 0);
    free(out);
  }

  sp_heap_free(&heap);
  return 0;
}

int
sp_test_heap(void)
{
  sp_test_heap_sequence();
  sp_test_heap_update();

  return 0;
}
