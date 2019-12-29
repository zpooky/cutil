#include "sp_cbb.h"

#include <stdint.h>
#include <memory.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#include "sp_util.h"

//==============================
struct sp_cbb {
  uint8_t *buffer;
  size_t read;
  size_t write;
  size_t capacity;
};

//==============================
struct sp_cbb *
sp_cbb_init(size_t capacity)
{
  struct sp_cbb *result = NULL;
  assert(capacity > 0);

  //XXX allocate $buffer together with calloc(cbyte_buffer)
  if ((result = calloc(1, sizeof(struct sp_cbb)))) {
    result->buffer   = calloc(capacity, sizeof(uint8_t));
    result->read     = 0;
    result->write    = 0;
    result->capacity = capacity;
  }

  return result;
}

//==============================
static size_t
sp_cbb_remaining_read2(size_t write, size_t read)
{
  if (write >= read) {
    return write - read;
  }

  assert(false); //TODO????
  return 0;
}

static size_t
sp_cbb_remaining_write2(size_t capacity, size_t length)
{
  assert(capacity >= length);
  return capacity - length;
}

size_t
sp_cbb_remaining_write(const struct sp_cbb *self)
{
  assert(self);
  assert(self->capacity >= sp_cbb_length(self));

  return self->capacity - sp_cbb_length(self);
}

size_t
sp_cbb_remaining_read(const struct sp_cbb *self)
{
  assert(self);

  return sp_cbb_remaining_read2(self->write, self->read);
}

//==============================
size_t
sp_cbb_length(const struct sp_cbb *self)
{
  assert(self);

  return sp_cbb_remaining_read(self);
}

size_t
sp_cbb_capacity(const struct sp_cbb *self)
{
  assert(self);

  return self->capacity;
}

//==============================
bool
sp_cbb_is_empty(const struct sp_cbb *self)
{
  assert(self);

  return self->read == self->write;
}

bool
sp_cbb_is_full(const struct sp_cbb *self)
{
  assert(self);

  return sp_cbb_length(self) == self->capacity;
}

//==============================
void
sp_cbb_clear(struct sp_cbb *self)
{
  assert(self);
  self->read = self->write = 0;
}

//==============================
static size_t
sp_cbb_index(size_t in, size_t capacity)
{
  return in & (capacity - 1);
}

static size_t
sp_cbb_write_buffer(struct sp_cbb *self, struct sp_cbb_Arr *res)
{
  size_t res_len = 0;
  /*
   * write     read    write
   * |xxxxxxxx|........|xxxxxxxxxx|
   */
  size_t writable;
  size_t w       = self->write;
  const size_t r = self->read;
Lit:

  writable =
    sp_cbb_remaining_write2(self->capacity, sp_cbb_remaining_read2(w, r));
  if (writable > 0) {
    const size_t w_idx                  = sp_cbb_index(w, self->capacity);
    const size_t length_until_array_end = sp_cbb_capacity(self) - w_idx;
    const size_t seg_len = sp_util_min(writable, length_until_array_end);
    uint8_t *seg         = self->buffer + w_idx;

    assert(seg_len > 0);
    assert(res_len < 2);

    {
      struct sp_cbb_Arr *cres = res + res_len;
      cres->base              = seg;
      cres->len               = seg_len;
      ++res_len;
    }

    w += seg_len;
    goto Lit;
  }

  return res_len;
}

static void
sp_cbb_produce_bytes(struct sp_cbb *self, size_t b)
{
  assert(sp_cbb_remaining_write(self) >= b);
  self->write += b;
}

size_t
sp_cbb_push_back(struct sp_cbb *self, const void *in, size_t in_len)
{
  size_t i;
  size_t result = 0;

  struct sp_cbb_Arr out[2];
  size_t out_len;

  out_len = sp_cbb_write_buffer(self, out);
  for (i = 0; i < out_len && in_len > 0; ++i) {
    uint8_t *const seg = out[i].base;
    size_t seg_len     = sp_util_min(out[i].len, in_len);

    memcpy(seg, in, seg_len);

    in = ((const uint8_t *)in) + result;
    result += seg_len;
    in_len -= seg_len;
  }
  sp_cbb_produce_bytes(self, result);

  return result;
}

//==============================
bool
sp_cbb_write(struct sp_cbb *self, const void *in, size_t length)
{
  if (sp_cbb_remaining_write(self) >= length) {
    size_t out = sp_cbb_push_back(self, in, length);

    assert(out == length);
    return true;
  }

  return false;
}

//==============================
static size_t
sp_cbb_read_buffer2(const struct sp_cbb *self,
                    struct sp_cbb_Arr *res,
                    size_t w,
                    size_t r)
{
  /*
   * read     write    read
   * |xxxxxxxx|........|xxxxxxxxxx|
   */
  size_t res_len = 0;
  size_t bytes;
Lit:
  bytes = sp_cbb_remaining_read2(w, r);
  if (bytes > 0) {
    const size_t r_idx = sp_cbb_index(r, self->capacity);
    const size_t l     = sp_util_min(bytes, sp_cbb_capacity(self) - r_idx);

    if (l > 0) {
      assert(res_len < 2);

      struct sp_cbb_Arr *cur = res + res_len;
      cur->base              = self->buffer + r_idx;
      cur->len               = l;
      ++res_len;

      r += l;
      goto Lit;
    }
  }

  return res_len;
}

size_t
sp_cbb_read_buffer(const struct sp_cbb *self, struct sp_cbb_Arr *res)
{
  return sp_cbb_read_buffer2(self, res, self->write, self->read);
}

//==============================
size_t
sp_cbb_peek_front(const struct sp_cbb *self, /*DEST*/ void *draw, size_t len)
{
  assert(self);
  assert(draw);

  struct sp_cbb_Arr out[2];
  size_t out_len;
  size_t i;
  size_t result = 0;
  uint8_t *dest = draw;

  out_len = sp_cbb_read_buffer2(self, out, self->write, self->read);
  for (i = 0; i < out_len && len > 0; ++i) {
    size_t seg_len           = sp_util_min(out[i].len, len);
    const uint8_t *const seg = out[i].base;

    memcpy(dest + result, seg, seg_len);
    result += seg_len;
    len -= seg_len;
  }

  return result;
}

//==============================
bool
sp_cbb_consume_bytes(struct sp_cbb *self, size_t b)
{
  assert(self);

  if ((self->read + b) <= self->write) {
    self->read += b;
    return true;
  }

  assert(false);
  return false;
}

//==============================
size_t
sp_cbb_pop_front(struct sp_cbb *self, /*DEST*/ void *draw, size_t len)
{
  assert(self);
  assert(draw);

  uint8_t *dest = draw;
  size_t result = sp_cbb_peek_front(self, dest, len);
  sp_cbb_consume_bytes(self, result);

  return result;
}

//==============================
bool
sp_cbb_read(struct sp_cbb *self, /*DEST*/ void *draw, size_t len)
{
  assert(self);
  assert(draw);

  uint8_t *dest = draw;
  size_t peeked;

  if (sp_cbb_remaining_read(self) < len) {
    return false;
  }

  peeked = sp_cbb_peek_front(self, dest, len);
  assert(peeked == len);

  sp_cbb_consume_bytes(self, len);

  return true;
}

//==============================
int
sp_cbb_free(struct sp_cbb **self)
{
  assert(self);

  if (*self) {
    assert(sp_cbb_is_empty(*self));

    free((*self)->buffer);
    free(*self);

    *self = NULL;
  }

  return 0;
}

//==============================
