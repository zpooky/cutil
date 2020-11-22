#include "sp_bst.h"

#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "sp_queue.h"
#include "sp_vec.h"
#include "sp_stack.h"
#include "sp_util.h"

//==============================
struct sp_bst {
  struct sp_bst_Node *root;

  sp_bst_node_cmp_cb node_cmp;
  sp_bst_node_new_cb node_new;
  sp_bst_node_free_cb node_free;
};

//==============================
struct sp_bst *
sp_bst_init(sp_bst_node_cmp_cb node_cmp,
            sp_bst_node_new_cb node_new,
            sp_bst_node_free_cb node_free)
{
  struct sp_bst *result;

  assert(node_cmp);
  assert(node_new);
  assert(node_free);

  if ((result = calloc(1, sizeof(*result)))) {
    result->node_cmp  = node_cmp;
    result->node_new  = node_new;
    result->node_free = node_free;
  }

  return result;
}

//==============================
static int
sp_bst_voidp_cmp_cb(sp_bst_voidp_Node *f, sp_bst_voidp_Node *s)
{
  assert(f);
  assert(s);

  return sp_util_void_cmp(f->value, s->value);
}

static struct sp_bst_Node *
sp_bst_voidp_new_cb(sp_bst_voidp_Node *in)
{
  sp_bst_voidp_Node *result;
  result = calloc(1, sizeof(*result));

  result->value = in->value;
  return &result->base;
}

static int
sp_bst_voidp_free_cb(sp_bst_voidp_Node *in)
{
  assert(in);
  free(in);

  return 0;
}

static struct sp_bst_Node *
sp_bst_identity_new_cb(struct sp_bst_Node *in)
{
  return in;
}

static int
sp_bst_identity_free_cb(struct sp_bst_Node *in)
{
  return 0;
}

struct sp_bst *
sp_bst_voidp_init(void)
{
  return sp_bst_init((sp_bst_node_cmp_cb)sp_bst_voidp_cmp_cb, //
                     (sp_bst_node_new_cb)sp_bst_voidp_new_cb, //
                     (sp_bst_node_free_cb)sp_bst_voidp_free_cb);
}

struct sp_bst *
sp_bst_voidp_init_cmp(sp_bst_node_cmp_cb cmp)
{
  return sp_bst_init(cmp, //
                     (sp_bst_node_new_cb)sp_bst_voidp_new_cb, //
                     (sp_bst_node_free_cb)sp_bst_voidp_free_cb);
}

struct sp_bst *
sp_bst_init_identity(sp_bst_node_cmp_cb cmp)
{
  return sp_bst_init(cmp, //
                     (sp_bst_node_new_cb)sp_bst_identity_new_cb, //
                     (sp_bst_node_free_cb)sp_bst_identity_free_cb);
}

//==============================
bool
sp_bst_is_empty(const struct sp_bst *self)
{
  return self->root == NULL;
}

//==============================
static size_t
sp_bst_node_length(const struct sp_bst_Node *self)
{ //XXX recursive
  size_t result = 0;
  if (self) {
    result += 1;
    result += sp_bst_node_length(self->left);
    result += sp_bst_node_length(self->right);
  }

  return result;
}

size_t
sp_bst_length(const struct sp_bst *self)
{
  return sp_bst_node_length(self->root);
}

//==============================
sp_bst_T *
sp_bst_insert_impl(struct sp_bst *self, sp_bst_T *in)
{
  return sp_bst_insert2_impl(self, in, self->node_new);
}

sp_bst_T *
sp_bst_insert2_impl(struct sp_bst *self,
                    sp_bst_T *in,
                    sp_bst_node_new_cb node_new)
{
  struct sp_bst_Node *res = NULL;

  assert(self);
  assert(in);
  assert(node_new);

  if (!self->root) {
    res = self->root = node_new(in);
    res->parent      = self;
    assert(res);
  } else {
    struct sp_bst_Node *it     = self->root;
    struct sp_bst_Node *parent = NULL;
    int in_cmp;

  Lit:
    parent = it;
    in_cmp = self->node_cmp(in, it);
    if (in_cmp > 0) {
      if (it->right) {
        it = it->right;
        goto Lit;
      }
      res = it->right = node_new(in);
      assert(res);
      res->parent = parent;
    } else if (in_cmp < 0) {
      if (it->left) {
        it = it->left;
        goto Lit;
      }
      res = it->left = node_new(in);
      assert(res);
      res->parent = parent;
    }
    /* else duplicate */
  }

  if (res) {
    res->left  = NULL;
    res->right = NULL;
  }

  return res;
}

static struct sp_bst_Node *
sp_bst_insert_identity_cb(sp_bst_T *in)
{
  return (struct sp_bst_Node *)in;
}

sp_bst_T *
sp_bst_insert_identity_impl(struct sp_bst *self, sp_bst_T *in)
{
  return sp_bst_insert2_impl(self, in, sp_bst_insert_identity_cb);
}

//==============================
sp_bst_T *
sp_bst_find_impl(struct sp_bst *self, sp_bst_T *needle)
{
  struct sp_bst_Node *it;
  struct sp_bst_Node *res    = NULL;
  struct sp_bst_Node *parent = NULL;

  assert(self);
  assert(needle);

  it = self->root;

  while (it) {
    int in_cmp = self->node_cmp(needle, it);
    parent     = it;
    if (in_cmp > 0) {
      it = it->right;
      assert(it && it->parent == parent);
    } else if (in_cmp < 0) {
      it = it->left;
      assert(it && it->parent == parent);
    } else {
      res = it;
      break;
    }
  } //while

  return res;
}

//==============================
static sp_bst_T *
remove_min(struct sp_bst *self, struct sp_bst_Node *in)
{
  while (in->left) {
    in = in->left;
  }

  sp_bst_remove_self_impl(in);
  return in;
}

sp_bst_T *
sp_bst_remove_impl(struct sp_bst *self, sp_bst_T *needle)
{
  struct sp_bst_Node *res = NULL;
  struct sp_bst_Node *it;

  assert(self);
  assert(needle);

  if ((it = sp_bst_find_impl(self, needle))) {
    struct sp_bst_Node *inherit = NULL;

    if (it->left && it->right) {
      inherit       = remove_min(self, it->right);
      inherit->left = it->left;

      if (inherit != it->right) {
        inherit->right = it->right;
      }
    } else if (it->left) {
      inherit = it->left;
    } else if (it->right) {
      inherit = it->right;
    }

    /* correct parent -> child link */
    if (it->parent != self) {
      struct sp_bst_Node *parent = it->parent;
      if (parent->right == it) {
        parent->right = inherit;
      } else {
        assert(parent->left == it);
        parent->left = inherit;
      }
      inherit->parent = parent;
    } else {
      self->root      = inherit;
      inherit->parent = self;
    }

    res        = it;
    res->left  = NULL;
    res->right = NULL;
  }

  return res;
}

bool
sp_bst_remove_free_impl(struct sp_bst *self, sp_bst_T *needle)
{
  struct sp_bst_Node *subject;

  assert(self);
  assert(needle);

  if ((subject = sp_bst_remove_impl(self, needle))) {
    self->node_free(subject);

    return true;
  }

  return false;
}

void
sp_bst_remove_self_impl(sp_bst_Node *needle)
{
  struct sp_bst *self = NULL;
  sp_bst_Node *it     = needle->parent;
  assert(it);
  while (it->parent) {
    it = needle->parent;
  }
  self = (struct sp_bst *)it;

  sp_bst_remove_impl(self, needle);
}

//==============================
int
sp_bst_clear(struct sp_bst *self)
{
  assert(self);

  if (self->root) {
    struct sp_bst_Node *current;
    struct sp_queue *stack = sp_queue_init();

    current = self->root;

    do {
      if (current->left) {
        sp_queue_enqueue(stack, current->left);
      }
      if (current->right) {
        sp_queue_enqueue(stack, current->right);
      }

      current->parent = current->left = current->right = NULL;
      self->node_free(current);
    } while (sp_queue_dequeue(stack, &current));

    sp_queue_free(&stack);
  }

  self->root = NULL;

  return 0;
}

//==============================
int
sp_bst_free(struct sp_bst **pself)
{
  assert(pself);

  if (*pself) {
    sp_bst_clear(*pself);

    free(*pself);
    *pself = NULL;
  }

  return 0;
}

//==============================
static int
sp_bst_node_in_order(struct sp_bst_Node *self, void *arg, sp_bst_node_it_cb it)
{
  if (self) {
    sp_bst_node_in_order(self->left, arg, it);
    it(self, arg);
    sp_bst_node_in_order(self->right, arg, it);
  }

  return 0;
}

int
sp_bst_in_order(struct sp_bst *self, void *arg, sp_bst_node_it_cb it)
{
  return sp_bst_node_in_order(self->root, arg, it);
}

int
sp_bst_node_reverse_order(struct sp_bst_Node *self,
                          void *arg,
                          sp_bst_node_it_cb it)
{
  if (self) {
    sp_bst_node_reverse_order(self->right, arg, it);
    it(self, arg);
    sp_bst_node_reverse_order(self->left, arg, it);
  }
  return 0;
}

int
sp_bst_reverse_order(struct sp_bst *self, void *arg, sp_bst_node_it_cb it)
{
  return sp_bst_node_reverse_order(self->root, arg, it);
}

int
sp_bst_node_pre_order(struct sp_bst_Node *self, void *arg, sp_bst_node_it_cb it)
{
  if (self) {
    it(self, arg);
    sp_bst_node_pre_order(self->left, arg, it);
    sp_bst_node_pre_order(self->right, arg, it);
  }

  return 0;
}

int
sp_bst_pre_order(struct sp_bst *self, void *arg, sp_bst_node_it_cb it)
{
  return sp_bst_node_pre_order(self->root, arg, it);
}

int
sp_bst_node_post_order(struct sp_bst_Node *self,
                       void *arg,
                       sp_bst_node_it_cb it)
{
  if (self) {
    sp_bst_node_post_order(self->left, arg, it);
    sp_bst_node_post_order(self->right, arg, it);
    it(self, arg);
  }
  return 0;
}

int
sp_bst_post_order(struct sp_bst *self, void *arg, sp_bst_node_it_cb it)
{
  return sp_bst_node_post_order(self->root, arg, it);
}

//==============================
static int
sp_bst_balance_it_cb(sp_bst_T *current, void *arg)
{
  assert(current);
  assert(arg);
  struct sp_vec *vec = arg;

  sp_vec_append(vec, current);

  return 0;
}

static struct sp_bst_Node *
sp_bst_make_balance(struct sp_bst_Node **arr, size_t length)
{
  size_t mid_idx = length / 2;
  struct sp_bst_Node *mid;

  if (length == 0) {
    return NULL;
  }
  mid       = arr[mid_idx];
  mid->left = sp_bst_make_balance(arr, mid_idx);

  size_t tail = mid_idx + 1;
  assert(length >= tail);
  mid->right = sp_bst_make_balance(arr + tail, length - tail);

  return mid;
}

int
sp_bst_rebalance(struct sp_bst *self)
{
  assert(self);

  size_t length            = sp_bst_length(self);
  struct sp_vec *vec       = sp_vec_init_cap(length);
  struct sp_bst_Node **arr = NULL;

  sp_bst_in_order(self, vec, sp_bst_balance_it_cb);

  arr        = (struct sp_bst_Node **)sp_vec_array(vec);
  self->root = sp_bst_make_balance(arr, sp_vec_length(vec));

  sp_vec_free(&vec);

  return 0;
}

//==============================
void
sp_bst_begin(struct sp_bst *self, struct sp_bst_It *it)
{
  struct sp_stack *order  = NULL;
  struct sp_bst_Node *bit = NULL;

  assert(self);
  assert(it);

  it->head  = NULL;
  it->queue = NULL;

  if (sp_bst_is_empty(self)) {
    return;
  }

  it->queue = sp_queue_init();
  bit       = self->root;

  do {
    while (bit) {
      if (!sp_stack_push(&order, bit)) {
        assert(false);
      }
      bit = bit->left;
    }

    if (sp_stack_pop(&order, &bit)) {
      if (!sp_queue_enqueue(it->queue, bit)) {
        assert(false);
      }

      bit = bit->right;
    }
  } while (!sp_stack_is_empty(order) || bit);

  assert(sp_bst_length(self) == sp_queue_length(it->queue));

  sp_bst_next(it);
}

void
sp_bst_next(struct sp_bst_It *it)
{
  struct sp_queue /*sp_bst_T*/ *q;

  assert(it);

  q = it->queue;

  assert(q);
  if (sp_queue_is_empty(q)) {
    sp_queue_free(&q);
    it->queue = NULL;
    it->head  = NULL;
    it        = NULL;
  } else {
    bool res = sp_queue_dequeue(q, &it->head);
    assert(res);
  }
}

//==============================
