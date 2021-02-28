#include "sp_cbb.h"

#include <memory.h>
#include <stdlib.h>
#include <assert.h>

#include "sp_util.h"
#include "sp_cbb_internal.h"

//==============================
static struct sp_cbb *
sp_cbb_init_internal(uint8_t *b, size_t capacity, size_t r, size_t w)
{
  struct sp_cbb *result = NULL;
  if ((result = calloc(1, sizeof(*result)))) {
    sp_cbb_init_internal0(result, b, capacity, r, w);
  }

  return result;
}

struct sp_cbb *
sp_cbb_init(size_t capacity)
{
  struct sp_cbb *result = NULL;
  uint8_t *b;
  assert(capacity > 0);

  //TODO limit capacity to power of 8
  //XXX allocate $buffer together with calloc(cbyte_buffer)
  if ((b = calloc(capacity, sizeof(*b)))) {
    if ((result = sp_cbb_init_internal(b, capacity, 0, 0))) {
      result->read_only   = 0;
      result->root        = NULL;
      result->free_buffer = true;

    } else {
      free(b);
    }
  }

  return result;
}

//==============================
static size_t
sp_cbb_remaining_read2(size_t w, size_t r)
{
  assert(w >= r);
  return w - r;
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
  size_t r      = self->mark_r ? self->original_r : self->r;
  size_t length = sp_cbb_remaining_read2(self->w, r);

  return sp_cbb_remaining_write2(self->capacity, length);
}

size_t
sp_cbb_remaining_read(const struct sp_cbb *self)
{
  assert(self);

  return sp_cbb_remaining_read2(self->w, self->r);
}

//==============================
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

  return self->r == self->w;
}

bool
sp_cbb_is_full(const struct sp_cbb *self)
{
  assert(self);

  return sp_cbb_remaining_write(self) == 0;
}

bool
sp_cbb_is_readonly(const struct sp_cbb *self)
{
  assert(self);

  return self->read_only;
}

//==============================
void
sp_cbb_clear(struct sp_cbb *self)
{
  assert(self);
  if (self->read_only) {
    assert(false);
    return;
  }

  self->r = self->w = 0;
}

//==============================
static size_t
sp_cbb_index(size_t in, size_t capacity)
{
  return in & (capacity - 1);
}

size_t
sp_cbb_push_back(struct sp_cbb *self, const void *in, size_t in_len)
{
  size_t i;
  size_t result = 0;

  struct sp_cbb_Arr out[2];
  size_t out_len;

  if (self->read_only) {
    assert(false);
    return 0;
  }

  out_len = sp_cbb_write_buffer(self, out);
  for (i = 0; i < out_len && in_len > 0; ++i) {
    uint8_t *const seg = out[i].base;
    size_t seg_len     = sp_min(out[i].len, in_len);

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
  if (self->read_only) {
    assert(false);
    return false;
  }

  if (sp_cbb_remaining_write(self) >= length) {
    size_t out = sp_cbb_push_back(self, in, length);

    assert(out == length);
    return true;
  }

  return false;
}

bool
sp_cbb_write_cbb(struct sp_cbb *self, struct sp_cbb *in)
{
  const size_t length = sp_cbb_remaining_read(in);
  if (self->read_only) {
    assert(false);
    return false;
  }

  if (sp_cbb_remaining_write(self) >= length) {
    struct sp_cbb_Arr out[2];
    size_t l_out, i, debug = 0;

    l_out = sp_cbb_read_buffer(in, out);
    for (i = 0; i < l_out; ++i) {
      size_t written;
      written = sp_cbb_push_back(self, out[i].base, out[i].len);
      assert(written == out[i].len);
      debug += out[i].len;
    }
    sp_cbb_consume_bytes(in, length);

    assert(debug == length);
    assert(sp_cbb_remaining_read(in) == 0);
    return true;
  }

  return false;
}

//==============================
static size_t
sp_cbb_read_buffer2(const struct sp_cbb *self,
                    struct sp_cbb_Arr *res,
                    const size_t w,
                    const size_t r)
{
  /*  read     write    read
   * |xxxxxxxx|........|xxxxxxxxxx|
   */
  size_t res_len = 0;
  size_t bytes;
  size_t rd = r;

  memset(res, 0, sizeof(*res) * 2);

Lit:
  bytes = sp_cbb_remaining_read2(w, rd);
  if (bytes > 0) {
    const size_t r_idx = sp_cbb_index(rd, self->capacity);
    const size_t l     = sp_min(bytes, sp_cbb_capacity(self) - r_idx);

    if (l > 0) {
      assert(res_len < 2);

      struct sp_cbb_Arr *cur = res + res_len;
      cur->base              = self->buffer + r_idx;
      cur->len               = l;
      ++res_len;

      rd += l;
      goto Lit;
    }
  }
  assert(sp_cbb_remaining_read2(w, r) == res[0].len + res[1].len);

  return res_len;
}

size_t
sp_cbb_read_buffer(const struct sp_cbb *self, struct sp_cbb_Arr *res)
{

  return sp_cbb_read_buffer2(self, res, self->w, self->r);
}

//==============================
size_t
sp_cbb_write_buffer(struct sp_cbb *self, struct sp_cbb_Arr *res)
{
  size_t res_len = 0;
  /*
   * write     read    write
   * |xxxxxxxx|........|xxxxxxxxxx|
   */
  size_t writable;
  size_t w       = self->w;
  const size_t r = self->mark_r ? self->original_r : self->r;

  if (self->read_only) {
    assert(false);
    return 0;
  }

Lit:

  writable =
    sp_cbb_remaining_write2(self->capacity, sp_cbb_remaining_read2(w, r));
  if (writable > 0) {
    const size_t w_idx                  = sp_cbb_index(w, self->capacity);
    const size_t length_until_array_end = sp_cbb_capacity(self) - w_idx;
    const size_t seg_len = sp_min(writable, length_until_array_end);
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

//==============================
size_t
sp_cbb_peek_front(const struct sp_cbb *self, /*DEST*/ void *draw, size_t len)
{
  struct sp_cbb_Arr out[2];
  size_t out_len;
  size_t i;
  size_t result = 0;
  uint8_t *dest = draw;

  assert(self);
  assert(draw);

  out_len = sp_cbb_read_buffer2(self, out, self->w, self->r);
  for (i = 0; i < out_len && len > 0; ++i) {
    size_t seg_len           = sp_min(out[i].len, len);
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

  if ((self->r + b) <= self->w) {
    self->r += b;
    return true;
  }

  assert(false);
  return false;
}

bool
sp_cbb_produce_bytes(struct sp_cbb *self, size_t b)
{

  if (self->read_only) {
    assert(false);
    return false;
  }

  if (sp_cbb_remaining_write(self) >= b) {
    self->w += b;
    return true;
  } else {
    assert(false);
    return false;
  }
}

//==============================
size_t
sp_cbb_pop_front(struct sp_cbb *self, /*DEST*/ void *draw, size_t len)
{
  uint8_t *dest = draw;
  size_t result;

  assert(self);
  assert(draw);

  result = sp_cbb_peek_front(self, dest, len);
  sp_cbb_consume_bytes(self, result);

  return result;
}

//==============================
bool
sp_cbb_read(struct sp_cbb *self, /*DEST*/ void *draw, size_t len)
{
  uint8_t *dest = draw;
  size_t peeked;

  assert(self);
  assert(draw);

  if (sp_cbb_remaining_read(self) < len) {
    return false;
  }

  peeked = sp_cbb_peek_front(self, dest, len);
  assert(peeked == len);

  sp_cbb_consume_bytes(self, len);

  return true;
}

//==============================
uint8_t
sp_cbb_get(const struct sp_cbb *self, size_t idx)
{
  assert(self);
  size_t r = self->r + idx;
  assert(idx < sp_cbb_remaining_read(self));

  return self->buffer[sp_cbb_index(r, self->capacity)];
}

//==============================
int
sp_cbb_free(struct sp_cbb **pself)
{
  assert(pself);

  if (*pself) {
    struct sp_cbb *self = *pself;
    if (!self->root) {
      assert(sp_cbb_is_empty(self));
    }

    if (self->read_only) {
      self->read_only--;

      assert(self->root);
      assert(self->root->read_only > 0);
      self->root->read_only--;
    }
    self->root = NULL;

    if (self->free_buffer) {
      assert(self->read_only == 0);
      free(self->buffer);
    }
    self->buffer = NULL;

    free(self);
    *pself = NULL;
  }

  return 0;
}

//==============================
int
sp_cbb_read_mark(struct sp_cbb *self, sp_cbb_mark_t *out)
{
  out->before   = self->r;
  out->rollback = false;
  if (self->mark_r == 0) {
    self->original_r = self->r;
  }
  self->mark_r++;
  return 0;
}

int
sp_cbb_read_unmark(struct sp_cbb *self, const sp_cbb_mark_t *in)
{
  assert(self);
  assert(in);

  if (in->rollback) {
    self->r = in->before;
    assert(self->r <= self->w);
  }

  assert(self->mark_r > 0);
  self->mark_r--;
  return 0;
}

int
sp_cbb_write_mark(struct sp_cbb *self, sp_cbb_mark_t *out)
{
  out->before   = self->w;
  out->rollback = false;
  self->mark_w++;

  return 0;
}

int
sp_cbb_write_unmark(struct sp_cbb *self, const sp_cbb_mark_t *in)
{
  if (in->rollback) {
    self->w = in->before;
    /* we have to make sure that $r not becomes gt $w when we rollback*/
    self->r = self->r <= self->w ? self->r : self->w;
  }

  assert(self->mark_w > 0);
  self->mark_w--;

  return 0;
}

//==============================
bool
sp_cbb_is_read_mark(const struct sp_cbb *self)
{
  return self->mark_r > 0;
}

bool
sp_cbb_is_write_mark(const struct sp_cbb *self)
{
  return self->mark_w > 0;
}

//==============================
static struct sp_cbb *
sp_cbb_consume_readonly_view_internal(struct sp_cbb *self,
                                      size_t consume,
                                      size_t length)
{
  struct sp_cbb *root   = self;
  struct sp_cbb *result = NULL;
  size_t r;
  assert(self);

  if (length > sp_cbb_capacity(self)) {
    return NULL;
  }

  if (sp_cbb_remaining_read(self) < length) {
    return NULL;
  }

  r = self->r;
  if (!sp_cbb_consume_bytes(self, consume)) {
    assert(false);
    return NULL;
  }

  result = sp_cbb_init_internal(self->buffer, self->capacity, r, r + length);
  if (result) {
    assert(sp_cbb_remaining_read(result) == length);
    while (root->root) {
      root = root->root;
    }
    root->read_only++;

    result->read_only   = 1;
    result->root        = root;
    result->free_buffer = false;
  }

  return result;
}

struct sp_cbb *
sp_cbb_readonly_view(struct sp_cbb *self, size_t length)
{
  return sp_cbb_consume_readonly_view_internal(self, 0, length);
}

struct sp_cbb *
sp_cbb_consume_readonly_view(struct sp_cbb *self, size_t length)
{
  return sp_cbb_consume_readonly_view_internal(self, length, length);
}

//==============================
