#include "sp_huffman.h"

#include "sp_bst.h"
#include "sp_heap.h"
#include "sp_util.h"
#include "sp_queue.h"
#include "sp_cbb.h"

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

#define BITS_MAX 16

/* ======================================== */
struct HufEncodeInfo {
  sp_bst_Node base;
  char raw;
  bool bits[BITS_MAX];
  size_t bits_len;
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
struct HufBST {
  sp_bst_Node base;
  char raw;
  uint32_t weight;
};

static int
hufBST_bst_cmp(struct HufBST *f, struct HufBST *s)
{
  return memcmp(&f->raw, &s->raw, sizeof(f->raw));
}

static struct HufBST *
hufBST_new(struct HufBST *in)
{
  struct HufBST *result;

  if ((result = calloc(1, sizeof(*result)))) {
    if (in) {
      memcpy(result, in, sizeof(*result));
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

static struct sp_bst * /*HufBST*/
sp_bst_hufBST_init(void)
{
  return sp_bst_init((sp_bst_node_cmp_cb)hufBST_bst_cmp,
                     (sp_bst_node_new_cb)hufBST_new,
                     (sp_bst_node_free_cb)hufBST_free);
}

/* ======================================== */
enum HufDecodeKind {
  HufDecodeKind_LEAF = 1,
  HufDecodeKind_NODE = 2,
  HufDecodeKind_EOS  = 3
};

struct HufDecodeInfo {
  sp_heap_T base;
  struct HufDecodeInfo *left;
  struct HufDecodeInfo *right;
  uint32_t weight;
  char raw;
  enum HufDecodeKind kind;
};

struct Huffman {
  struct HufDecodeInfo *decode;

  struct sp_bst * /*HufEncodeInfo*/ encode;
  struct HufEncodeInfo encode_eos;

  char decode_cur;
  size_t decode_bits;
};

static struct HufDecodeInfo *
hufDecodeInfo_new(struct HufBST *in)
{
  struct HufDecodeInfo *result;

  if ((result = calloc(1, sizeof(*result)))) {
    if (in) {
      result->kind   = HufDecodeKind_LEAF;
      result->raw    = in->raw;
      result->weight = in->weight;
    } else {
      result->kind = HufDecodeKind_NODE;
    }
  }

  return result;
}

static int
hn_weight_cmp(struct HufDecodeInfo *f, struct HufDecodeInfo *s)
{
  return sp_util_uint32_cmp(f->weight, s->weight);
}

static int
hn_min_weight_cmp(struct HufDecodeInfo *f, struct HufDecodeInfo *s)
{
  return hn_weight_cmp(s, f);
}

/* ======================================== */
static struct sp_heap * /*HufDecodeInfo*/
huffman_build_heap(const char *plaintext, size_t plen)
{
  size_t i;
  struct sp_bst_It it;
  struct sp_heap * /*HufDecodeInfo*/ result = NULL;
  struct sp_bst * /*HufBST*/ tree           = sp_bst_hufBST_init();

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
    sp_heap_enqueue(result, hufDecodeInfo_new(it.head));
  }

Lout:
  sp_bst_free(&tree);

  return result;
}

static void
huf_build_encode(struct Huffman *self,
                 struct HufDecodeInfo *cur,
                 bool *bits,
                 size_t idx)
{
  if (cur->left || cur->right) {
    if (cur->left) {
      bits[idx] = false;
      huf_build_encode(self, cur->left, bits, idx + 1);
    }

    if (cur->right) {
      bits[idx] = true;
      huf_build_encode(self, cur->right, bits, idx + 1);
    }
  } else {
    struct HufEncodeInfo info = {0};
    struct HufEncodeInfo *tmp = &info;

    if (cur->kind == HufDecodeKind_EOS) {
      tmp = &self->encode_eos;
      printf("HufDecodeKind_EOS: ");
    }

    tmp->raw      = cur->raw;
    tmp->bits_len = idx;
    memcpy(&tmp->bits, bits, sizeof(tmp->bits));

    printf("cur->raw[%c, %p]\n", cur->raw, (void *)cur);
    if (cur->kind != HufDecodeKind_EOS) {
      assertx_n(sp_bst_insert(self->encode, tmp));
    }
  }
}

struct Huffman *
huffman_init(const char *plaintext, size_t plen)
{
  struct Huffman *self                    = NULL;
  struct sp_heap * /*HufDecodeInfo*/ heap = NULL;
  struct HufDecodeInfo *eos               = NULL;

  if (!(heap = huffman_build_heap(plaintext, plen))) {
    goto Lout;
  }

  self = calloc(1, sizeof(*self));

  eos       = hufDecodeInfo_new(NULL);
  eos->kind = HufDecodeKind_EOS;

  assertx_n(sp_heap_enqueue(heap, eos));

  while (sp_heap_length(heap) > 1) {
    struct HufDecodeInfo *first;
    struct HufDecodeInfo *second;
    struct HufDecodeInfo *root;

    assertx_n(sp_heap_dequeue(heap, &first));
    assertx_n(sp_heap_dequeue(heap, &second));
    printf("[%u, %u]\n", first->weight, second->weight);

    root         = hufDecodeInfo_new(NULL);
    root->left   = first;
    root->right  = second;
    root->weight = first->weight + second->weight;

    assertx_n(sp_heap_enqueue(heap, root));
  }

  if (!sp_heap_is_empty(heap)) {
    bool bits[BITS_MAX] = {0};

    struct HufDecodeInfo *root = NULL;
    assertx_n(sp_heap_dequeue(heap, &root));

    self->decode = root;
    self->encode = sp_bst_init((sp_bst_node_cmp_cb)hufEncodeInfo_cmp,
                               (sp_bst_node_new_cb)hufEncodeInfo_new,
                               (sp_bst_node_free_cb)hufEncodeInfo_free);

    huf_build_encode(self, root, bits, 0);
  }

Lout:
  assert(sp_heap_is_empty(heap));
  sp_heap_free(&heap);

  return self;
}

/* ======================================== */
int
huffman_free(struct Huffman **in)
{
  assert(in);

  if (*in) {
    struct Huffman *self     = *in;
    struct HufDecodeInfo *it = self->decode;

    struct sp_queue * /*HufDecodeInfo*/ queue;
    queue = sp_queue_init();

    do {
      if (it->left)
        assertx_n(sp_queue_enqueue(queue, it->left));

      if (it->right)
        assertx_n(sp_queue_enqueue(queue, it->right));

      free(it);
    } while (sp_queue_dequeue(queue, &it));

    sp_bst_free(&self->encode);

    free(self);
    *in = NULL;

    sp_queue_free(&queue);
  }

  return 0;
}

/* ======================================== */
struct HufBuf {
  struct sp_cbb *dest;

  uint8_t raw;
  size_t bit_length;
};

static void
huf_write_bits(uint8_t b, size_t bits, struct HufBuf *dest)
{
  while (bits) {
    const uint8_t c  = dest->bit_length ? dest->raw : 0;
    size_t remaining = sp_util_min(8 - dest->bit_length, bits);

    uint8_t tmp = b >> dest->bit_length;
    tmp |= c;

    dest->bit_length += remaining;
    bits -= remaining;
    b = b << remaining;

    if (dest->bit_length == 8) {
      assertx_n(sp_cbb_write(dest->dest, &tmp, sizeof(tmp)));
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
huf_encode_char(const struct HufEncodeInfo *src, struct HufBuf *dest)
{
  size_t src_idx        = 0;
  size_t remaining = src->bits_len;

  //TODO check dest buff size
  while (remaining > 0) {
    size_t i    = 0;
    uint8_t b   = 0;
    size_t bits = 0;

    for (i = 0; i < sp_util_min(8, remaining); ++i) {
      if (src->bits[src_idx++]) {
        b |= huf_mask(bits);
      }
      bits++;
    }
    huf_write_bits(b, bits, dest);
    remaining -= bits;
  }
}

const char *
huffman_encode(const struct Huffman *self,
               const char *plaintext,
               size_t plen,
               struct sp_cbb *dest)
{
  const char *it           = plaintext;
  const char *const end    = it + plen;
  struct HufBuf compressed = {.dest = dest};

  //TODO check dest buff size
  while (it != end) {
    struct HufEncodeInfo *info;

    struct HufEncodeInfo needle = {.raw = *it};
    if ((info = sp_bst_find(self->encode, &needle))) {
      huf_encode_char(info, &compressed);
    } else {
      //TODO fail
    }

    ++it;
  }

  if (*it == '\0') {
    // TODO somehow ensure that there is always room for the eos, otherwise we
    // end up in an weird state
    huf_encode_char(&self->encode_eos, &compressed);
  }

  return it;
}

/* ======================================== */
typedef bool (*cb_t)(struct Huffman *self, bool, struct sp_cbb *sink);

static struct HufDecodeInfo *
huf_decode_walk(char *cur, size_t *bits, struct HufDecodeInfo *tree)
{
  const char mask = 1 << 7;
  bool head;

  assert(*bits);

  head = (*cur) & mask;
  *cur <<= 1;
  (*bits)--;

  if (head) {
    tree = tree->left;
  } else {
    tree = tree->right;
  }

  if (tree->kind != HufDecodeKind_NODE) {
    return tree;
  }

  return huf_decode_walk(cur, bits, tree);
}

static bool
huf_decode_bits(struct Huffman *self, char cur, struct sp_cbb *sink)
{
  struct HufDecodeInfo *tree = self->decode;
  size_t cur_bits            = 8;

Lit2 : {
  if (self->decode_bits) {
  Lit : {
    char before_cur    = self->decode_cur;
    size_t before_bits = self->decode_bits;

    tree = huf_decode_walk(&self->decode_cur, &self->decode_bits, tree);
    if (tree->kind == HufDecodeKind_LEAF) {
      sp_cbb_write(sink, &tree->raw, sizeof(tree->raw));
      tree = self->decode;
      if (self->decode_bits) {
        goto Lit;
      }
    } else if (tree->kind == HufDecodeKind_EOS) {
      return false;
    } else {
      if (cur_bits == 0) {
        self->decode_cur  = before_cur;
        self->decode_bits = before_bits;
      }
    }
  } //Lit
  }

  if (cur_bits) {
    self->decode_cur  = cur;
    self->decode_bits = cur_bits;
    cur_bits          = 0;
    goto Lit2;
  }
} //Lit2

  return true;
}

bool
huffman_decode(struct Huffman *self,
               struct sp_cbb *compressed,
               struct sp_cbb *sink)
{
  bool result = true;

  while (!sp_cbb_is_empty(compressed) && result) {
    char cur;
    assertx_n(sp_cbb_read(compressed, &cur, sizeof(cur)));
    result = huf_decode_bits(self, cur, sink);
  }

  return result;
}

/* ======================================== */
