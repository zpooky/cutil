#include "sp_test_hashset.h"
#include "sp_hashset.h"

#include <stdalign.h>
#include <memory.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include <sp_util.h>

struct hashset_test {
  uint32_t hash;
  int value;
};

static uint32_t
sp_test_hash(const struct hashset_test *in)
{
  return in->hash;
}
static void
sp_test_copy(struct hashset_test *dest,
             const struct hashset_test *src,
             size_t sz)
{
  memcpy(dest, src, sz);
}
static bool
sp_test_eq(const struct hashset_test *f,
           const struct hashset_test *s,
           size_t sz)
{
  return f->value == s->value;
}

static void
shuffle_int(uint32_t *arr, size_t len)
{
  size_t i;
  for (i = 0; i < len; ++i) {
    size_t idx = (size_t)rand() % len;
    sp_util_swap_uint32_t(arr + i, arr + idx);
  }
}

static int
sp_do_test_simple(void)
{
  struct sp_hashset *set;
  const int MAX = 1024 * 1;
  /* const int MAX = 16 * 1; */
  int i;
  int a;
  uint32_t data[MAX];

  set = sp_hashset_init(
    alignof(struct hashset_test), sizeof(struct hashset_test),
    (sp_hashset_hash_cb)sp_test_hash, (sp_hashset_copy_cb)sp_test_copy,
    (sp_hashset_eq_cb)sp_test_eq);

  for (i = 0; i < MAX; ++i) {
    data[i] = i;
  }

  shuffle_int(data, (size_t)MAX);
  for (i = 0; i < MAX; ++i) {
    struct hashset_test *out = NULL;
    struct hashset_test tmp  = {0};

    for (a = 0; a < i; ++a) {
      tmp.hash = tmp.value = data[a];
      out                  = sp_hashset_lookup(set, &tmp);
      if (!out) {
        printf("i:%d, a:%d\n", i, data[a]);
        /* sp_hashset_dump(set);//TODO bug: duplicate in hashset */
        sp_hashset_lookup(set, &tmp);
      }

      assert(out);
      assert(out->value == data[a]);
    }

    assert(sp_hashset_length(set) == i);

    tmp.hash = tmp.value = data[i];
    out                  = sp_hashset_insert(set, &tmp);
    assert(out);
    assert(out->value == data[i]);
/* sp_hashset_dump(set); */

    for (a = i + 1; a < MAX; ++a) {
      tmp.hash = tmp.value = data[a];
      out                  = sp_hashset_lookup(set, &tmp);
      if (out) {
        out = sp_hashset_lookup(set, &tmp);
        printf("data[a]:%u\n", data[1]);
        printf("out:%u\n", out->hash);
        printf("out:%u\n", out->value);
      }
      assert(!out);
    }
  } //for

  assert(sp_hashset_length(set) == MAX);

  for (i = 0; i < MAX; ++i) {
    struct hashset_test *out = NULL;
    struct hashset_test tmp  = {.value = data[i], .hash = data[i]};
    out                      = sp_hashset_lookup(set, &tmp);
    assert(out);
    assert(out->value == data[i]);
  }

  shuffle_int(data, (size_t)MAX);
  for (i = 0; i < MAX; ++i) {
    struct hashset_test *out = NULL;
    struct hashset_test tmp  = {0};

    /* assert(sp_hashset_length(set) == MAX - i); */
    for (a = 0; a < i; ++a) {
      tmp.hash = tmp.value = data[a];
      out                  = sp_hashset_lookup(set, &tmp);
      assert(!out);
    }

    /* printf(".remove: %d\n", i); */
    tmp.hash = tmp.value = data[i];
    out                  = sp_hashset_lookup(set, &tmp);
    assert(out);
    assert(out->value == data[i]);
    sp_hashset_remove(set, out);

    for (a = i + 1; a < MAX; ++a) {
      tmp.hash = tmp.value = data[a];
      out                  = sp_hashset_lookup(set, &tmp);
      assert(out);
      assert(out->value == data[a]);
    }
  } //for
  for (i = 0; i < MAX; ++i) {
    struct hashset_test *out = NULL;
    struct hashset_test tmp  = {.value = data[i], .hash = data[i]};
    out                      = sp_hashset_lookup(set, &tmp);
    assert(!out);
  }
  assert(sp_hashset_length(set) == 0);
  assert(sp_hashset_is_empty(set));

  sp_hashset_free(&set);
  return 0;
}

int
sp_test_hashset(void)
{
  sp_do_test_simple();
  return 0;
}
