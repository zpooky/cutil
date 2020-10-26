#include "sp_bitset.h"

#include <assert.h>
#include <stdlib.h>

#include "sp_util.h"

//==============================
struct sp_bitset {
  int *raw;
  size_t length;
};

//==============================
struct sp_bitset *
sp_bitset_init(size_t bits_capacity) {
  const size_t bits = sizeof(int) * 8;
  size_t capacity =
      sp_util_max(bits_capacity / bits + bits_capacity % bits == 0 ? 0 : 1, 1);
  struct sp_bitset *result;

  if ((result = calloc(1, sizeof(*result)))) {
    result->raw = calloc(capacity, sizeof(*result->raw));
    result->length = capacity;
  }

  return result;
}

int
sp_bitset_free(struct sp_bitset **pself) {
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
mask_out(size_t idx) {
  int result = 0;
  return (result | 1) << idx;
}

static bool
test(int word, size_t bit_idx) {
  int mask = mask_out(bit_idx);
  return (mask & word) == mask;
}

static size_t
word_index(size_t abs_idx) {
  const size_t bits = sizeof(int) * 8;
  return abs_idx / bits;
}

static size_t
bit_index(size_t abs_idx) {
  const size_t bits = sizeof(int) * 8;
  return abs_idx % bits;
}

bool
sp_bitset_test(struct sp_bitset *self, size_t idx) {
  size_t wIdx = word_index(idx);
  assert(wIdx < self->length);

  size_t bIdx = bit_index(idx);
  int word = self->raw[wIdx];

  return test(word, bIdx);
}

//==============================
static void
set(int *word, size_t bit_idx, bool v) {
  const int mask = mask_out(bit_idx);
  if (v) {
    *word = *word | mask;
  } else {
    *word = *word & (~mask);
  }
}

bool
sp_bitset_set(struct sp_bitset *self, size_t idx, bool v) {
  size_t wIdx = word_index(idx);
  assert(wIdx < self->length);

  const int old_word = self->raw[wIdx];
  const size_t bIdx = bit_index(idx);

  set(&self->raw[wIdx], bIdx, v);

  return test(old_word, bIdx);
}

//==============================
