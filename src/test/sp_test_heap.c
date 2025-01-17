#include "sp_test_heap.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "sp_heap.h"
#include "sp_vec.h"
#include "sp_util.h"

typedef struct {
  sp_heap_T base;
  size_t value;
} THNode;

static int
sp_test_size_MIN_cmp(THNode *f, THNode *s)
{
  assert(f);
  assert(s);
  return -sp_util_size_t_cmp(f->value, s->value);
}

static int
sp_test_size_MAX_cmp(THNode *f, THNode *s)
{
  return sp_test_size_MIN_cmp(s, f);
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
  const size_t max = 1024;
  struct sp_vec /*THNode*/ *pool;
  struct sp_heap /*THNode*/ *heap;

  pool = sp_vec_new();
  heap = sp_heap_init((sp_heap_cmp_cb)sp_test_size_MIN_cmp);

  {
    size_t i;
    for (i = 0; i < max; ++i) {
      THNode **res;
      THNode *in;

      in        = calloc(1, sizeof(*in));
      in->value = i;
      /* printf("calloc(%p)\n", (void *)in); */

      res = sp_vec_append(pool, in);
      assert(res);
      assert(*res == in);
      assert((*res)->value == i);
    }
  }

  {
    THNode **it;
    size_t i = 0;
    assert(sp_vec_length(pool) == max);
    sp_vec_for_each2 (it, pool) {
      assert((*it)->value == i);
      ++i;
    }
  }

  shuffle(pool);
  assert(sp_heap_is_empty(heap));

  {
    THNode **it;
    sp_vec_for_each2 (it, pool) {
      sp_heap_enqueue(heap, *it);
    }
  }

  assert(sp_heap_length(heap) == max);

  {
    THNode *out;
    size_t i = 0;

    while (sp_heap_dequeue(heap, &out)) {
      assert(out);
      /* printf("- %zu[%p]\n", out->value, (void *)out); */
      assert(i == out->value);
      i++;
      free(out);
    }
    assert(i == max);
    assert(sp_heap_is_empty(heap));
  }

  sp_vec_free(&pool);
  sp_heap_free(&heap);

  return 0;
}

static int
sp_test_heap_update(void)
{
  size_t i;
  THNode *out;
  struct sp_heap /*THNode*/ *heap;
  const size_t min = 256;
  heap             = sp_heap_init((sp_heap_cmp_cb)sp_test_size_MIN_cmp);

  for (i = 512; i-- > min;) {
    THNode *in;
    THNode *res;

    in        = calloc(1, sizeof(*in));
    in->value = i;

    res = sp_heap_enqueue(heap, in);
    assert(res);
    assert(res == in);
    assert(res->value == i);
  }

  while (!sp_heap_is_empty(heap)) {
    size_t idx       = (size_t)rand() % sp_heap_length(heap);
    THNode **subject = (THNode **)sp_heap_array(heap) + idx;

    (*subject)->value = 0;

    sp_heap_update_key(heap, *subject);

    assert(sp_heap_dequeue(heap, &out));
    assert(out);
    assert(out->value == 0);
    free(out);
  }

  sp_heap_free(&heap);
  return 0;
}

static int
sp_test_heap_update2(void)
{
  size_t i;
  THNode **it;
  THNode *cur;
  struct sp_heap /*THNode*/ *heap;
  struct sp_vec /*THNode*/ *vec;
  const size_t max = 1024;
  heap             = sp_heap_init((sp_heap_cmp_cb)sp_test_size_MAX_cmp);
  vec              = sp_vec_new();

  for (i = 0; i < max; ++i) {
    cur        = calloc(1, sizeof(*cur));
    cur->value = i;
    sp_heap_enqueue(heap, cur);
    sp_vec_append(vec, cur);
  }

  assert(sp_heap_length(heap) == max);
  assert(sp_vec_length(vec) == max);
  shuffle(vec);

  sp_vec_for_each2 (it, vec) {
    /* printf("it->index[%zu]value[%zu]", (*it)->base.idx, (*it)->value); */
    (*it)->value = i++;
    sp_heap_update_key(heap, *it);
    /* printf("it->index[%zu]value[%zu]\n", (*it)->base.idx, (*it)->value); */
  }

  cur = NULL;
  --i;
  while (sp_heap_dequeue(heap, &cur)) {
    assert(cur);
    /* printf("- cur->value[%zu], i[%zu]\n", cur->value, i); */
    assert(cur->value == i);
    i--;
    free(cur);
  }

  assert(sp_heap_is_empty(heap));

  sp_vec_free(&vec);
  sp_heap_free(&heap);
  return 0;
}

int
sp_test_heap(void)
{
  sp_test_heap_sequence();
  sp_test_heap_update();
  sp_test_heap_update2();

  return 0;
}
