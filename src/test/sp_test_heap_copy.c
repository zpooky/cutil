#include "sp_test_heap_copy.h"

#include "sp_heap_copy.h"
#include "sp_util.h"
#include <assert.h>
#include <stdalign.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
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

static int
test_update(void)
{
  size_t i;
  THNode *out;
  struct sp_heap_copy /*THNode*/ *heap;
  const size_t min = 1024;

  heap = sp_heap_copy_init2(alignof(THNode), sizeof(THNode),
                            (sp_heap_copy_cmp_cb)sp_test_size_MIN_cmp);

  for (i = min; i-- > min;) {
    THNode in = {.value = i};
    THNode *res;

    res = sp_heap_copy_enqueue(heap, &in);
    assert(res);
    assert(res->value == in.value);
    assert(res->value == i);
  }

  while (!sp_heap_copy_is_empty(heap)) {
    size_t idx      = (size_t)rand() % sp_heap_copy_length(heap);
    THNode *subject = (THNode *)sp_heap_copy_array(heap) + idx;

    subject->value = 0;

    sp_heap_copy_update_key(heap, subject);

    assert(sp_heap_copy_dequeue(heap, &out));
    assert(out);
    assert(out->value == 0);
  }

  sp_heap_copy_free(&heap);
  return 0;
}

int
sp_test_heap_copy(void)
{
  printf("%s: BEGIN\n", __func__);
  test_update();
  printf("%s: END\n", __func__);
  return 0;
}
