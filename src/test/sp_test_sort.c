#include "sp_test_sort.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <sp_util.h>

static int
sp_util_uint32p_cmp_inv(const uint32_t *f, const uint32_t *s)
{
  assert(f);
  assert(s);
  return sp_util_uint32_cmp(*s, *f);
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

static void
test_quicksort(sp_cb_cmp cmp)
{
  const size_t max = 1024;
  uint32_t arr[max];
  uint32_t i;
  for (i = 0; i < max; ++i) {
    arr[i] = i;
  }
  shuffle(arr, max);

  sp_util_sort(arr, max, sizeof(arr[0]), cmp);

  assert(sp_util_is_sorted(arr, max, sizeof(arr[0]), cmp));

  for (i = 0; i < max; ++i) {
    uint32_t *res;
    res = sp_util_bin_search(arr, max, &i, sizeof(i), cmp);
    assert(res);
    assert(*res == i);
  }
}

static void
test_quicksort_min(void)
{
  const size_t max = 1024;
  uint32_t arr[max];
  for (uint32_t i = 0; i < max; ++i) {
    arr[i] = i;
  }
  shuffle(arr, max);

  sp_util_sort(arr, max, sizeof(arr[0]), (sp_cb_cmp)sp_util_uint32_min_cmp);
  for (uint32_t i = 0; i < max; ++i) {
    assert(arr[i] == i);
  }
}

static void
test_quicksort_max(void)
{
  const uint32_t max = 1024;
  uint32_t arr[max];
  for (uint32_t i = 0; i < max; ++i) {
    arr[i] = i;
  }
  shuffle(arr, max);

  sp_util_sort(arr, max, sizeof(arr[0]), (sp_cb_cmp)sp_util_uint32_max_cmp);
  for (uint32_t i = 0; i < max; ++i) {
    assert(arr[i] == max - 1 - i);
  }
}

static void
test_bin_insert(sp_cb_cmp cmp)
{
  const size_t max = 1024;
  uint32_t arr[max];
  uint32_t sorted[max];
  size_t l_sorted = 0;
  uint32_t i;
  for (i = 0; i < max; ++i) {
    arr[i] = i;
  }
  shuffle(arr, max);
  for (i = 0; i < max; ++i) {
    size_t a;
    /* printf(".%d: %d\n", i, arr[i]); */
    /* for (a = 0; a < i; ++a) { */
    /*   printf("%d,", sorted[a]); */
    /* } */
    /* if (i > 0) { */
    /*   printf("\n"); */
    /* } */

    for (a = 0; a < i; ++a) {
      int *res;
      res = sp_util_bin_search(sorted, i, &arr[a], sizeof(arr[a]), cmp);
      assert(res);
      assert(*res == arr[a]);
    }
    for (a = i; a < max; ++a) {
      int *res;
      res = sp_util_bin_search(sorted, i, &arr[a], sizeof(arr[a]), cmp);
      assert(!res);
    }
    size_t res =
      sp_util_bin_insert_uniq(sorted, &l_sorted, &arr[i], sizeof(arr[i]), cmp);
    assert(l_sorted == i + 1);
    assert(sorted[res] == arr[i]);

    assert(sp_util_is_sorted(sorted, l_sorted, sizeof(arr[0]), cmp));
  }
  /* size_t a; */
  /* for (a = 0; a < i; ++a) { */
  /*   printf("%d,", sorted[a]); */
  /* } */
  /* printf("\n"); */
}

void
sp_test_sort(void)
{
  printf("BEGIN %s\n", __func__);
  /* while(1){ */
  test_quicksort((sp_cb_cmp)sp_util_uint32p_cmp);
  test_quicksort((sp_cb_cmp)sp_util_uint32p_cmp_inv);
  test_bin_insert((sp_cb_cmp)sp_util_uint32p_cmp);
  test_bin_insert((sp_cb_cmp)sp_util_uint32p_cmp_inv);
  test_quicksort_min();
  test_quicksort_max();
  /* printf("==\n"); */
  /* } */
  printf("END %s\n", __func__);
}
