#include "sp_huffman.h"

#include "sp_bst.h"
#include "sp_heap.h"
#include "sp_util.h"

#include <assert.h>
#include <memory.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*
 * Huffman coding assigns codes to characters such that the length of the code
 * depends on the relative frequency or weight of the corresponding character.
 * Huffman codes are of variable-length, and prefix-free (no code is prefix of
 * any other). Any prefix-free binary code can be visualized as a binary tree
 * with the encoded characters stored at the leaves.
 *
 * Huffman Tree is a full binary tree in which each leaf of the tree
 * corresponds to a letter in the given alphabet.
 */

/* ======================================== */
struct HufNode {
  sp_bst_Node base;
  char raw; // only present when $left, $right is NULL (leaf)
  uint32_t weight;

  struct HufNode *left;
  struct HufNode *right;
};

struct Huffman {
  struct HufNode *root;
};

static int
hn_weight_cmp(struct HufNode *f, struct HufNode *s)
{
  return sp_util_uint32_cmp(f->weight, s->weight);
}

static int
hn_min_weight_cmp(struct HufNode *f, struct HufNode *s)
{
  return hn_weight_cmp(s, f);
}

static int
hn_bst_cmp(struct HufNode *f, struct HufNode *s)
{
  return memcmp(&f->raw, &s->raw, sizeof(f->raw));
}

static struct HufNode *
hn_new(struct HufNode *in)
{
  struct HufNode *result;

  if ((result = calloc(1, sizeof(*in)))) {
    if (in) {
      memcpy(result, in, sizeof(*in));
    }
  }

  return result;
}

static int
hn_free(struct HufNode *in)
{
  free(in);
  return 0;
}

static struct sp_bst *
sp_bst_hn_new(void)
{
  return sp_bst_init((sp_bst_node_cmp_cb)hn_bst_cmp, (sp_bst_node_new_cb)hn_new,
                     (sp_bst_node_free_cb)hn_free);
}

/* ======================================== */
static struct sp_bst *
huffman_build_bst(const char *plaintext)
{
  size_t i;
  struct sp_bst *result = sp_bst_hn_new();
  size_t plen           = strlen(plaintext);

  for (i = 0; i < plen; ++i) {
    struct HufNode *res;
    struct HufNode needle = {0};

    needle.raw = plaintext[i];
    res        = sp_bst_find(result, &needle);
    if (!res) {
      res = sp_bst_insert(result, &needle);
    }
    assert(res);
    ++res->weight;
  }

  return result;
}

struct Huffman *
huffman_init(const char *plaintext)
{
  struct Huffman *result = NULL;
  struct sp_bst_It it;
  struct sp_heap *heap = NULL;
  struct sp_bst *tree  = NULL;

  if (!(tree = huffman_build_bst(plaintext))) {
    goto Lout;
  }

  if (!(heap = sp_heap_init((sp_heap_cmp_cb)hn_min_weight_cmp))) {
    goto Lout;
  }

  sp_bst_for_each (&it, tree) {
    sp_heap_enqueue(heap, hn_new(it.head));
  }

  while (sp_heap_length(heap) > 1) {
    struct HufNode *first, *second;
    struct HufNode *root;

    if (!sp_heap_dequeue(heap, &first)) {
      assert(false);
    }
    if (!sp_heap_dequeue(heap, &second)) {
      assert(false);
    }

    root         = hn_new(NULL);
    root->left   = first;
    root->right  = second;
    root->weight = first->weight + second->weight;

    sp_heap_enqueue(heap, root);
  }

  if (!sp_heap_is_empty(heap)) {
    result = calloc(1, sizeof(*result));

    if (!sp_heap_dequeue(heap, &result->root)) {
      assert(false);
    }
  }

Lout:
  sp_heap_free(&heap);
  sp_bst_free(&tree);

  return result;
}

int
huffman_free(struct Huffman **in)
{
  //TODO
  return 0;
}

/* ======================================== */
