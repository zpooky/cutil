#include "sp_test_hashset.h"
#include <sp_hashset.h>
#include <sp_djb.h>
#include <sp_vec_copy.h>

#include <stdalign.h>
#include <memory.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <sp_util.h>
static void
sp_vec_copy_shuffle(struct sp_vec_copy *vec)
{
  size_t length = sp_vec_copy_length(vec);
  for (size_t i = 0; i < length; ++i) {
    size_t next = rand() % length;
    if (i != next) {
      sp_vec_copy_swap(vec, i, next);
    }
  }
}

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
sp_test_move(struct hashset_test *dest, struct hashset_test *src, size_t sz)
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

  set = sp_hashset_new(alignof(struct hashset_test),
                       sizeof(struct hashset_test), (sp_cb_hash)sp_test_hash,
                       (sp_cb_move)sp_test_move, (sp_cb_eq)sp_test_eq);

  for (i = 0; i < MAX; ++i) {
    data[i] = i;
  }

  shuffle_int(data, (size_t)MAX);
  for (i = 0; i < MAX; ++i) {
    struct hashset_test *out = NULL;
    struct hashset_test tmp  = {0};

    for (a = 0; a < i; ++a) {
      tmp.hash = tmp.value = data[a];
      if (!(out = sp_hashset_lookup(set, &tmp))) {
        printf("i:%d, a:%d\n", i, data[a]);
        /* sp_hashset_dump(set);//TODO bug: duplicate in hashset */
        sp_hashset_lookup(set, &tmp);
      }

      assert(out);
      assert(out->value == data[a]);
      assert(out->hash == tmp.hash);
      assert(out->value == tmp.value);
    }

    assert(sp_hashset_length(set) == i);

    tmp.hash = tmp.value = data[i];
    out                  = sp_hashset_insert_move(set, &tmp);
    assert(out);
    assert(out->value == data[i]);
    /* sp_hashset_dump(set); */

    for (a = i + 1; a < MAX; ++a) {
      tmp.hash = tmp.value = data[a];
      if ((out = sp_hashset_lookup(set, &tmp))) {
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
    bool b;

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
    b = sp_hashset_remove(set, out);
    assert(b);

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

struct hashset_test_str_val {
  char key[128];
  int value;
};

static uint32_t
hashset_test_str_val_hash(const struct hashset_test_str_val *in)
{
  return sp_djb2_hash(in->key, strlen(in->key));
}
static bool
hashset_test_str_val_eq(const struct hashset_test_str_val *f,
                        const struct hashset_test_str_val *s,
                        size_t sz)
{
  size_t f_l = strlen(f->key);
  size_t s_l = strlen(s->key);

  return f_l == s_l && strncmp(f->key, s->key, f_l) == 0;
}

static void
sp_do_test_str_val(void)
{
  struct sp_hashset *set;
  struct sp_vec_copy /*int*/ *pool;
  const int max = 11 * 1024;
  int *it;

  pool = sp_vec_copy_init0_cap(max, alignof(int), sizeof(int));
  set  = sp_hashset_new(alignof(struct hashset_test_str_val), //
                        sizeof(struct hashset_test_str_val), //
                        (sp_cb_hash)hashset_test_str_val_hash, //
                        sp_cb_move_memcopy, (sp_cb_eq)hashset_test_str_val_eq);

  for (int i = 0; i < max; ++i) {
    assert(sp_vec_copy_length(pool) == i);
    sp_vec_copy_append(pool, &i);
  }
  assert(sp_vec_copy_length(pool) == max);
  sp_vec_copy_shuffle(pool);

  fprintf(stderr, "%s:1\n", __func__);
  sp_vec_copy_for_each2(pool, it)
  {
    struct hashset_test_str_val val  = {.value = *it};
    struct hashset_test_str_val *res = NULL;
    /* fprintf(stderr, "%s:i[%d]\n", __func__, *it); */
    sprintf(val.key, "%d", *it);
    res = sp_hashset_insert_move(set, &val);
    assert(res);
    assert(res->value == *it);
    assert(strcmp(res->key, val.key) == 0);
  }

  sp_vec_copy_shuffle(pool);
  assert(sp_hashset_length(set) == max);

  fprintf(stderr, "%s:2\n", __func__);
  sp_vec_copy_for_each2(pool, it)
  {
    struct hashset_test_str_val needle = {0};
    struct hashset_test_str_val *res   = NULL;
    sprintf(needle.key, "%d", *it);
    res = sp_hashset_lookup(set, &needle);
    assert(res);
    assert(res->value == *it);
    assert(strcmp(res->key, needle.key) == 0);
  }
  fprintf(stderr, "%s:3\n", __func__);
  assert(sp_hashset_length(set) == max);
  sp_vec_copy_shuffle(pool);

  sp_vec_copy_for_each2(pool, it)
  {
    bool rem;
    struct hashset_test_str_val needle = {0};
    struct hashset_test_str_val *res   = NULL;
    sprintf(needle.key, "%d", *it);
    rem = sp_hashset_remove(set, &needle);
    assert(rem);
    res = sp_hashset_lookup(set, &needle);
    assert(!res);
  }
  assert(sp_hashset_length(set) == 0);

  fprintf(stderr, "%s:4\n", __func__);
  sp_hashset_free(&set);
  sp_vec_copy_free(&pool);
}

int
sp_test_hashset(void)
{
  printf("%s: BEGIN\n", __func__);
  sp_do_test_simple();

  printf("%s: ==\n", __func__);
  sp_do_test_str_val();
  printf("%s: END\n", __func__);
  return 0;
}
