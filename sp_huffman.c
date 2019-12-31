#include "sp_huffman.h"

#include "sp_bst.h"
#include "sp_heap.h"
#include "sp_util.h"
#include "sp_queue.h"

#include <assert.h>
#include <memory.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
struct HufBST {
  sp_bst_Node base;
  char raw;
  uint32_t weight;
};

static int
hn_weight_cmp(struct HufBST *f, struct HufBST *s)
{
  return sp_util_uint32_cmp(f->weight, s->weight);
}

static int
hn_min_weight_cmp(struct HufBST *f, struct HufBST *s)
{
  return hn_weight_cmp(s, f);
}

/* ======================================== */
static int
hufBST_bst_cmp(struct HufBST *f, struct HufBST *s)
{
  return memcmp(&f->raw, &s->raw, sizeof(f->raw));
}

static struct HufBST *
hufBST_new(struct HufBST *in)
{
  struct HufBST *result;

  if ((result = calloc(1, sizeof(*in)))) {
    if (in) {
      memcpy(result, in, sizeof(*in));
    }
  }

  return result;
}

static int
hufBST_free(struct HufBST *in)
{
  free(in);
  return 0;
}

static struct sp_bst *
sp_bst_hufBST_init(void)
{
  return sp_bst_init((sp_bst_node_cmp_cb)hufBST_bst_cmp,
                     (sp_bst_node_new_cb)hufBST_new,
                     (sp_bst_node_free_cb)hufBST_free);
}

/* ======================================== */
struct HuffmanNode {
  sp_heap_T base;
  char raw; // only present when $left, $right is NULL (leaf)
  uint32_t weight;
  struct HuffmanNode *left;
  struct HuffmanNode *right;
};

struct Huffman {
  struct HuffmanNode *root;
};

static struct HuffmanNode *
hufNode_new(struct HufBST *in)
{
  struct HuffmanNode *result;

  if ((result = calloc(1, sizeof(*in)))) {
    if (in) {
      result->raw    = in->raw;
      result->weight = in->weight;
    }
  }

  return result;
}

/* ======================================== */
static struct sp_heap * /*HuffmanNode*/
huffman_build_heap(const char *plaintext)
{
  size_t i;
  struct sp_bst_It it;
  struct sp_bst *tree = sp_bst_hufBST_init();
  size_t plen         = strlen(plaintext);
  struct sp_heap * /*HuffmanNode*/ result;

  if (!(result = sp_heap_init((sp_heap_cmp_cb)hn_min_weight_cmp))) {
    goto Lout;
  }

  for (i = 0; i < plen; ++i) {
    struct HufBST *res;
    struct HufBST needle = {.raw = plaintext[i]};

    res = sp_bst_find(tree, &needle);
    if (!res) {
      res = sp_bst_insert(tree, &needle);
    }
    assert(res);
    ++res->weight;
  }

  sp_bst_for_each (&it, tree) {
    sp_heap_enqueue(result, hufNode_new(it.head));
  }

Lout:
  sp_bst_free(&tree);
  return result;
}

struct Huffman *
huffman_init(const char *plaintext)
{
  struct Huffman *result                = NULL;
  struct sp_heap * /*HuffmanNode*/ heap = NULL;

  if (!(heap = huffman_build_heap(plaintext))) {
    goto Lout;
  }

  while (sp_heap_length(heap) > 1) {
    struct HuffmanNode *first, *second;
    struct HuffmanNode *root;

    assertx_n(sp_heap_dequeue(heap, &first));
    assertx_n(sp_heap_dequeue(heap, &second));

    root         = hufNode_new(NULL);
    root->left   = first;
    root->right  = second;
    root->weight = first->weight + second->weight;

    assertx_n(sp_heap_enqueue(heap, root));
  }

  if (!sp_heap_is_empty(heap)) {
    result = calloc(1, sizeof(*result));
    assertx_n(sp_heap_dequeue(heap, &result->root));
  }

Lout:
  sp_heap_free(&heap);

  return result;
}

/* ======================================== */
int
huffman_free(struct Huffman **in)
{
  if (*in) {
    struct sp_queue * /*HuffmanNode*/ queue;
    struct HuffmanNode *it = (*in)->root;

    queue = sp_queue_init();
    do {
      if (it->left)
        assertx_n(sp_queue_enqueue(queue, it->left));

      if (it->right)
        assertx_n(sp_queue_enqueue(queue, it->right));

      free(it);
    } while (sp_queue_dequeue(queue, &it));

    free(*in);
    *in = NULL;
    sp_queue_free(&queue);
  }

  return 0;
}

/* ======================================== */
struct HufEncodeInfo {
  sp_bst_Node base;
  char raw;
  bool bits[8];
  size_t len;
};

static int
hufEncodeInfo_cmp(struct HufEncodeInfo *f, struct HufEncodeInfo *s)
{
  return memcmp(&f->raw, &s->raw, sizeof(f->raw));
}

static struct HufEncodeInfo *
hufEncodeInfo_new(struct HufEncodeInfo *in)
{
  struct HufEncodeInfo *result;

  if ((result = calloc(1, sizeof(*result)))) {
    memcpy(result, in, sizeof(*result));
  }

  return result;
}

static int
hufEncodeInfo_free(struct HufEncodeInfo *in)
{
  free(in);
  return 0;
}

/* ======================================== */
struct HufBuf {
  uint8_t *raw;
  size_t capacity;

  size_t length;
  size_t bit_length;
};

static void
huf_build_db(struct sp_bst * /*HufEncodeInfo*/ self,
             struct HuffmanNode *cur,
             bool *bits,
             size_t idx)
{
  if (cur->left || cur->right) {
    if (cur->left) {
      bits[idx] = false;
      huf_build_db(self, cur->left, bits, idx + 1);
    }

    if (cur->right) {
      bits[idx] = true;
      huf_build_db(self, cur->right, bits, idx + 1);
    }
  } else {
    struct HufEncodeInfo info = {
      .raw = cur->raw,
      .len = idx,
    };
    memcpy(&info.bits, bits, sizeof(info.bits));

    printf("cur->raw[%c, %p]\n", cur->raw, (void *)cur);
    assertx_n(sp_bst_insert(self, &info));
  }
}

static void
huf_write_bits(uint8_t b, size_t bits, struct HufBuf *dest)
{
  while (bits) {
    const uint8_t c  = dest->bit_length ? dest->raw[dest->length] : 0;
    size_t remaining = sp_util_min(8 - dest->bit_length, bits);

    uint8_t tmp = b >> dest->bit_length;
    tmp |= c;

    dest->raw[dest->length] = tmp;
    dest->bit_length += remaining;
    bits -= remaining;
    b = b << remaining;

    if (dest->bit_length == 8) {
      dest->length++;
      dest->bit_length = 0;
    }
  }
}

static uint8_t
huf_mask(size_t idx)
{
  uint8_t result = 1 << 7;
  return result >> idx;
}

static void
huf_encode_char(struct HufEncodeInfo *x, struct HufBuf *dest)
{
  size_t xb        = 0;
  size_t remaining = x->len;

  while (remaining > 0) {
    size_t i;
    uint8_t b   = 0;
    size_t bits = 0;
    for (i = 0; i < sp_util_min(8, remaining); ++i) {
      if (x->bits[xb++]) {
        b |= huf_mask(bits);
      }
      bits++;
    }
    huf_write_bits(b, bits, dest);
    remaining -= bits;
  }
}

void
huffman_encode(const struct Huffman *self, const char *plaintext)
{
  size_t i;
  struct sp_bst * /*HufEncodeInfo*/ db;
  struct HufBuf compressed = {
    .raw = calloc(1024, sizeof(uint8_t)),
  };
  size_t plen = strlen(plaintext);

  db = sp_bst_init((sp_bst_node_cmp_cb)hufEncodeInfo_cmp,
                   (sp_bst_node_new_cb)hufEncodeInfo_new,
                   (sp_bst_node_free_cb)hufEncodeInfo_free);
  if (!db) {
    goto Lout;
  }

  bool bits[8] = {0};
  size_t idx   = 0;

  huf_build_db(db, self->root, bits, idx);

  for (i = 0; i < plen; ++i) {
    struct HufEncodeInfo *info;

    struct HufEncodeInfo needle = {.raw = plaintext[i]};
    if ((info = sp_bst_find(db, &needle))) {
      huf_encode_char(info, &compressed);
    } else {
      //TODO fail
    }
  }

Lout:
  sp_bst_free(&db);
}

/* ======================================== */
