#include "sp_test_bst.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

#include "sp_bst.h"

struct sp_test {
  struct sp_bst_Node base;
  int data;
};

static int
test_cmp(struct sp_test *first, struct sp_test *second)
{
  if (first->data < second->data) {
    return -1;
  }
  if (first->data > second->data) {
    return 1;
  }

  return 0;
}

static struct sp_bst_Node *
test_new(struct sp_test *in)
{
  assert(in);
  struct sp_test *out = calloc(1, sizeof(struct sp_test));
  out->data           = in->data;
  return &out->base;
}

static int
test_free(sp_bst_T *reclaim)
{
  assert(reclaim);
  free(reclaim);
  return 0;
}

static int
test_it_cb(struct sp_test *c, int *arg)
{
  assert(arg);

  assert(c->data == *arg);
  ++*arg;

  /* printf("%d-", c->data); */
  return 0;
}

static void
swap_int(int *f, int *s)
{
  int tmp = *f;
  *f      = *s;
  *s      = tmp;
}

static void
shuffle_int(int *arr, size_t len)
{
  size_t i;
  for (i = 0; i < len; ++i) {
    size_t idx = (size_t)rand() % len;
    swap_int(arr + i, arr + idx);
  }
}

int
sp_do_test_bst(void)
{
  const int MAX = 1024 * 1;
  int i;
  int a;
  int cmp;
  {
    struct sp_bst *bst =
      sp_bst_init((sp_bst_node_cmp_cb)test_cmp, (sp_bst_node_new_cb)test_new,
                  (sp_bst_node_free_cb)test_free);
    bool retry = true;

    srand((unsigned)time(NULL));

    int data[MAX];
    for (i = 0; i < MAX; ++i) {
      cmp = 0;
      sp_bst_in_order(bst, &cmp, (sp_bst_node_it_cb)test_it_cb);
      assert(cmp == 0);

      data[i] = i;
      {
        struct sp_test *out = NULL;
        struct sp_test tmp  = {.data = data[i]};
        out                 = sp_bst_find(bst, &tmp);
        assert(!out);
        out = sp_bst_remove(bst, &tmp);
        assert(!out);
      }
    }

  Lstart:
    shuffle_int(data, (size_t)MAX);

    for (i = 0; i < MAX; ++i) {
      for (a = 0; a < i; ++a) {
        struct sp_test *out = NULL;
        struct sp_test tmp  = {.data = data[a]};
        out                 = sp_bst_find(bst, &tmp);
        assert(out);
        assert(out->data == data[a]);
      }
      for (a = i; a < MAX; ++a) {
        struct sp_test *out = NULL;
        struct sp_test tmp  = {.data = data[a]};
        out                 = sp_bst_find(bst, &tmp);
        assert(!out);
      }

      struct sp_test *out = NULL;
      struct sp_test tmp  = {.data = data[i]};
      out                 = sp_bst_insert(bst, &tmp);
      assert(out);
      assert(out->data == data[i]);
      /*duplicate*/
      out = sp_bst_insert(bst, &tmp);
      assert(!out);
    }

    cmp = 0;
    sp_bst_in_order(bst, &cmp, (sp_bst_node_it_cb)test_it_cb);
    assert(cmp == MAX);

    sp_bst_rebalance(bst);

    cmp = 0;
    sp_bst_in_order(bst, &cmp, (sp_bst_node_it_cb)test_it_cb);
    assert(cmp == MAX);

    shuffle_int(data, (size_t)MAX);
    for (i = 0; i < MAX; ++i) {
      for (a = 0; a < i; ++a) {
        struct sp_test *out = NULL;
        struct sp_test tmp  = {.data = data[a]};
        out                 = sp_bst_find(bst, &tmp);
        assert(!out);
      }
      for (a = i; a < MAX; ++a) {
        struct sp_test *out = NULL;
        struct sp_test tmp  = {.data = data[a]};
        out                 = sp_bst_find(bst, &tmp);
        assert(out);
        assert(out->data == data[a]);
      }

      struct sp_test *out = NULL;
      struct sp_test tmp  = {.data = data[i]};
      out                 = sp_bst_remove(bst, &tmp);
      assert(out);
      assert(out->data == data[i]);
      free(out);
    }

    cmp = 0;
    sp_bst_in_order(bst, &cmp, (sp_bst_node_it_cb)test_it_cb);
    assert(cmp == 0);

    if (retry) {
      retry = false;
      goto Lstart;
    }

    /* printf("free\n"); */
    sp_bst_free(&bst);
  }
  {
    struct sp_bst *bst;
    bst =
      sp_bst_init((sp_bst_node_cmp_cb)test_cmp, (sp_bst_node_new_cb)test_new,
                  (sp_bst_node_free_cb)test_free);

    for (i = 0; i < MAX; ++i) {
      struct sp_test *out = NULL;
      struct sp_test tmp  = {.data = i};
      out                 = sp_bst_insert(bst, &tmp);
      assert(out);
      assert(out->data == i);
    }
    for (i = 0; i < MAX; ++i) {
      struct sp_test *out = NULL;
      struct sp_test tmp  = {.data = i};
      out                 = sp_bst_find(bst, &tmp);
      assert(out);
      assert(out->data == i);
    }

    cmp = 0;
    sp_bst_in_order(bst, &cmp, (sp_bst_node_it_cb)test_it_cb);
    assert(cmp == MAX);
    /* printf("\n----\n"); */
    sp_bst_rebalance(bst);
    cmp = 0;
    sp_bst_in_order(bst, &cmp, (sp_bst_node_it_cb)test_it_cb);
    /* printf("\n"); */
    assert(cmp == MAX);

    for (i = 0; i < MAX; ++i) {
      cmp = i;
      sp_bst_in_order(bst, &cmp, (sp_bst_node_it_cb)test_it_cb);
      assert(cmp == MAX);

      struct sp_test *out = NULL;
      struct sp_test tmp  = {.data = i};
      out                 = sp_bst_find(bst, &tmp);
      assert(out);
      assert(out->data == i);
      assert(sp_bst_remove_free(bst, &tmp));
    }

    for (i = 0; i < MAX; ++i) {
      cmp = 0;
      sp_bst_in_order(bst, &cmp, (sp_bst_node_it_cb)test_it_cb);
      assert(cmp == 0);

      struct sp_test *out = NULL;
      struct sp_test tmp  = {.data = i};
      out                 = sp_bst_find(bst, &tmp);
      assert(!out);
    }

    /* printf("free2\n"); */
    sp_bst_free(&bst);
  }
  return 0;
}

static int
sp_for_each_test_bst(void)
{
  const int MAX = 1024 * 1;
  int i;
  struct sp_bst *bst;
  bst = sp_bst_init((sp_bst_node_cmp_cb)test_cmp, (sp_bst_node_new_cb)test_new,
                    (sp_bst_node_free_cb)test_free);

  for (i = 0; i < MAX; ++i) {
    int cnt = 0;
    struct sp_bst_It it;
    /* printf(".%d\n",i); */
    sp_bst_for_each (&it, bst) {
      ++cnt;
    }
    assert(i == cnt);
    struct sp_test *out = NULL;
    struct sp_test tmp  = {.data = i};
    out                 = sp_bst_insert(bst, &tmp);
    assert(out);
    assert(out->data == i);
    sp_bst_rebalance(bst);
  }

  sp_bst_free(&bst);
  return 0;
}

static int
sp_do_test_simple(void)
{
  struct sp_bst *bst;
  bst = sp_bst_init((sp_bst_node_cmp_cb)test_cmp, (sp_bst_node_new_cb)test_new,
                    (sp_bst_node_free_cb)test_free);

  const int MAX = 1024 * 1;
  int i;
  int a;
  int data[MAX];
  for (i = 0; i < MAX; ++i) {
    data[i] = i;
  }

  shuffle_int(data, (size_t)MAX);
  for (i = 0; i < MAX; ++i) {
    struct sp_test *out = NULL;
    struct sp_test tmp  = {0};

    for (a = 0; a < i; ++a) {
      tmp.data = data[a];
      out      = sp_bst_find(bst, &tmp);

      assert(out);
      assert(out->data == data[a]);
    }

    assert(sp_bst_length(bst) == i);

    tmp.data = data[i];
    out      = sp_bst_insert(bst, &tmp);
    assert(out);
    assert(out->data == data[i]);

    for (a = i + 1; a < MAX; ++a) {
      tmp.data = data[a];
      out      = sp_bst_find(bst, &tmp);
      assert(!out);
    }
  } //for

  shuffle_int(data, (size_t)MAX);
  for (i = 0; i < MAX; ++i) {
    struct sp_test *out = NULL;
    struct sp_test tmp  = {0};

    for (a = 0; a < i; ++a) {
      tmp.data = data[a];
      out      = sp_bst_find(bst, &tmp);
      assert(!out);
    }

    printf(".remove: %d\n", i);
    tmp.data = data[i];
    out      = sp_bst_find(bst, &tmp);
    assert(out);
    assert(out->data == data[i]);
    sp_bst_remove_self(out);

    for (a = i; a < MAX; ++a) {
      tmp.data = data[a];
      out      = sp_bst_find(bst, &tmp);
      assert(out);
      assert(out->data == data[a]);
    }
  } //for
  assert(sp_bst_length(bst) == 0);

  sp_bst_free(&bst);
  return 0;
}

int
sp_test_bst(void)
{
  sp_do_test_simple();
  sp_do_test_bst();
  sp_for_each_test_bst();
  return 0;
}
