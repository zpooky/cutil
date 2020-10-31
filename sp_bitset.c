#include "sp_bitset.h"

#include <assert.h>
#include <stdlib.h>

#include "sp_util.h"

#define BS_BITS (sizeof(int) * 8)

//==============================
struct sp_bitset {
  int *raw;
  size_t length;
  size_t bits;
};

//==============================
static size_t
capacity_of_bits(size_t capacity)
{
  return capacity / BS_BITS + (capacity % BS_BITS == 0 ? 0 : 1);
}

struct sp_bitset *
sp_bitset_init(size_t bits_capacity)
{
  size_t capacity = sp_util_max(capacity_of_bits(bits_capacity), 1);
  struct sp_bitset *result;

  if ((result = calloc(1, sizeof(*result)))) {
    result->raw    = calloc(capacity, sizeof(*result->raw));
    result->length = capacity;
    result->bits   = bits_capacity;
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
sp_bitset_test(struct sp_bitset *self, size_t idx)
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
