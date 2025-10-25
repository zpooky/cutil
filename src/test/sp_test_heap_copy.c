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
                            (sp_cb_cmp)sp_test_size_MIN_cmp);

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

struct sp_bt_BEP09_Heap {
  /* int64_t piece; */
  bool have;
  size_t requested;
};
static inline const char *
sp_debug_sp_bt_BEP09_Heap(const struct sp_bt_BEP09_Heap *in)
{
  static char buf[1024] = {'\0'};
  if (!in)
    return "sp_bt_BEP09_Heap(NULL)";
  snprintf(buf, sizeof(buf), "{h[%s]req[%zu]}", in->have ? "TRUE" : "FALSE",
           in->requested);
  return buf;
}

static int
sp_bt_BEP09_Heap_have_cmp(const struct sp_bt_BEP09_Heap *f,
                          const struct sp_bt_BEP09_Heap *s)
{
  /* TODO secondary sort order requested must work */
  /* TODO mabye redisgn with individual heaps */
  assertx(f);
  assertx(s);
  if (f->have == s->have) {
    return 0;
  }
  if (!s->have) {
    /* return sp_util_size_t_cmp(f->requested, s->requested); */
    return -1;
  }
  return 1;
}

static int
sp_bt_BEP09_Heap_cmp(const struct sp_bt_BEP09_Heap *f,
                     const struct sp_bt_BEP09_Heap *s)
{
  int ret = sp_bt_BEP09_Heap_have_cmp(f, s);
  if (ret == 0) {
    return sp_util_size_t_cmp(s->requested, f->requested);
  }
  return ret;
}

static void
shuffle(uint32_t *arr, size_t len)
{
  size_t i;
  for (i = 0; i < len; ++i) {
    size_t idx = (size_t)rand() % len;
    sp_util_swap_uint32_t(&arr[i], &arr[idx]);
  }
}

static int
test_update2()
{
  struct sp_bt_BEP09_Heap tmp = {0};
  const size_t max            = 1024;
  uint32_t arr[max];
  uint32_t i;
  struct sp_heap_copy *heap = sp_heap_copy_init2(
    alignof(struct sp_bt_BEP09_Heap), sizeof(struct sp_bt_BEP09_Heap),
    (sp_cb_cmp)sp_bt_BEP09_Heap_cmp);
  struct sp_bt_BEP09_Heap tmp_head = {0};

  for (i = 0; i < max; ++i) {
    arr[i] = i;
  }
  shuffle(arr, max);
  for (i = 0; i < max; ++i) {
    struct sp_bt_BEP09_Heap tmp = {
      .requested = arr[i],
      .have      = rand() < (RAND_MAX / 2),
    };
    sp_heap_copy_enqueue(heap, &tmp);
  }
  assert(sp_heap_copy_length(heap) == max);
  i = 0;
  sp_heap_copy_dequeue(heap, &tmp_head);
  ++i;
  while (sp_heap_copy_dequeue(heap, &tmp)) {
    fprintf(stderr, "tmp[%s]\n", sp_debug_sp_bt_BEP09_Heap(&tmp));
    ++i;
    assertx(tmp_head.have ? tmp.have == true : true);
    if (tmp_head.have == tmp.have) {
      assertx(tmp_head.requested <= tmp.requested);
    }
    tmp_head = tmp;
  }
  assert(i == max);
  sp_heap_copy_free(&heap);
  return 0;
}

int
sp_test_heap_copy(void)
{
  printf("%s: BEGIN\n", __func__);
  test_update();
  test_update2();
  printf("%s: END\n", __func__);
  return 0;
}
