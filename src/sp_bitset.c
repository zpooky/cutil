#include "sp_bitset.h"

#include <assert.h>
#include <stdlib.h>

#include "sp_util.h"
#include "sp_cbb.h"

//==============================
struct sp_bitset {
  int *raw;
  size_t length;
  size_t bits;
};

//==============================
#define BS_BITS (sizeof(int) * 8)
static size_t
bits_to_capacity(size_t bits)
{
  return (bits / BS_BITS) + (bits % BS_BITS == 0 ? 0 : 1);
}

struct sp_bitset *
sp_bitset_init(size_t bits)
{
  size_t capacity = sp_util_max(bits_to_capacity(bits), 1);
  struct sp_bitset *result;

  if ((result = calloc(1, sizeof(*result)))) {
    result->raw    = calloc(capacity, sizeof(int));
    result->length = capacity;
    result->bits   = bits;
  }

  return result;
}

struct sp_bitset *
sp_bitset_init_cbb(struct sp_cbb *in, size_t length)
{
  struct sp_bitset *result;

  assert(in);
  if (length > sp_cbb_remaining_read(in)) {
    assert(false);
    return NULL;
  }

  if ((result = sp_bitset_init(length * 8))) {
    sp_bitset_read(result, in, length);
  }

  return result;
}

int
sp_bitset_free(struct sp_bitset **pself)
{
  assert(pself);

  struct sp_bitset *self = *pself;
  if (self) {
    if (self->raw) {
      free(self->raw);
    }

    free(self);
    *pself = NULL;
  }

  return 0;
}

//==============================
static int
mask_out(size_t idx)
{
  int result = 0;
  return (result | 1) << idx;
}
static int
mask_out_multiple(size_t bits)
{
  int result = 0;
  size_t i;
  assert(bits < BS_BITS);
  for (i = 0; i < bits; ++i) {
    result |= mask_out(i);
  }

  return result;
}

static bool
test(int word, size_t bit_idx)
{
  int mask = mask_out(bit_idx);
  return (mask & word) == mask;
}

static size_t
word_index(size_t abs_idx)
{
  return abs_idx / BS_BITS;
}

static size_t
bit_index(size_t abs_idx)
{
  return abs_idx % BS_BITS;
}

bool
sp_bitset_test(const struct sp_bitset *self, size_t idx)
{
  size_t wIdx = word_index(idx);
  assert(wIdx < self->length);

  size_t bIdx = bit_index(idx);
  int word    = self->raw[wIdx];

  return test(word, bIdx);
}

//==============================
static void
set(int *word, size_t bit_idx, bool v)
{
  const int mask = mask_out(bit_idx);
  if (v) {
    *word = *word | mask;
  } else {
    *word = *word & (~mask);
  }
}

bool
sp_bitset_set(struct sp_bitset *self, size_t idx, bool v)
{
  size_t wIdx = word_index(idx);
  assert(wIdx < self->length);

  const int old_word = self->raw[wIdx];
  const size_t bIdx  = bit_index(idx);

  set(&self->raw[wIdx], bIdx, v);

  return test(old_word, bIdx);
}

//==============================
void
sp_bitset_set_all(struct sp_bitset *self, int v)
{
  size_t i;
  for (i = 0; i < self->length; ++i) {
    self->raw[i] = v;
  }
}

//==============================
bool
sp_bitset_is_all(const struct sp_bitset *self, int v)
{
  size_t i    = 0;
  size_t bits = self->bits;
  while (bits > BS_BITS) {
    if (self->raw[i++] != v) {
      return false;
    }
    bits -= BS_BITS;
  } //while

  if (bits > BS_BITS) {
    v = v & mask_out_multiple(bits);
    if ((self->raw[i++] & v) != v) {
      return true;
    }
  }

  return true;
}

//==============================
/* test: {
 *   uint8_t r[128];
 *   size_t rl;
 *   size_t i;
 *   struct sp_bitset *b = sp_bitset_init(64);
 *   struct sp_bitset *bin;
 *   struct sp_cbb *out = sp_cbb_init(1024);
 *   sp_bitset_set(b, 0, true);
 *   sp_bitset_set(b, 1, true);
 *   sp_bitset_set(b, 8, true);
 *   sp_bitset_write(b, out);
 *   rl = sp_cbb_peek_front(out, r, sizeof(r));
 *   sp_util_to_hex(r, rl);
 *   bin = sp_bitset_init_cbb(out, sp_cbb_remaining_read(out));
 *   for (i = 0; i < 64; ++i) {
 *     assert(sp_bitset_test(b, i) == sp_bitset_test(bin, i));
 *   }
 * }
 */
/*
 * bit index:
 * [8-0][16-8][24-16][32-24]
 */
bool
sp_bitset_write(const struct sp_bitset *self, struct sp_cbb *out)
{
  bool result = true;
  size_t i;
  size_t bits = 0;
  uint8_t c   = 0;
  sp_cbb_mark_t m;

  sp_cbb_write_mark(out, &m);

  for (i = 0; i < self->bits; ++i) {
    c = c | ((sp_bitset_test(self, i) ? 1 : 0) << bits);
    ++bits;

    if (bits == 8) {
      bits = 0;
      if (!sp_cbb_write(out, &c, 1)) {
        result = false;
        break;
      }
      c = 0;
    }
  }

  if (result && bits > 0) {
    result = sp_cbb_write(out, &c, 1);
  }

  m.rollback = !result;
  sp_cbb_write_unmark(out, &m);
  return result;
}

bool
sp_bitset_read(struct sp_bitset *self, struct sp_cbb *in, size_t length)
{
  size_t bidx = 0;
  assert(self);
  assert(in);

  bool result = true;
  if (length > sp_cbb_remaining_read(in)) {
    assert(false);
    return false;
  }

  if (length > (self->length * BS_BITS)) {
    assert(false);
    return false;
  }

  while (length && result) {
    size_t i;
    char c;

    if (!(result = sp_cbb_read(in, &c, 1))) {
      break;
    }
    for (i = 0; i < 8; ++i) {
      char x = c >> i;
      sp_bitset_set(self, bidx++, x & 1);
    }

    --length;
  }

  return result;
}

//==============================
