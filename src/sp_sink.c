#include "sp_sink.h"
#include <stdlib.h>
#include "sp_cbb.h"
#include <assert.h>
#include <errno.h>

#include <sys/uio.h> //writev

//==============================
struct sp_sink {
  sp_sink_write_cb write_cb;
  struct sp_cbb *buffer;
  void *arg;
};

//==============================
int
sp_sink_file_write_out(struct sp_cbb *buffer, void *closure)
{
  int res         = 0;
  ssize_t written = 0;
  int *fd         = closure;

  assert(buffer);
  assert(fd);

  do {
    int points = 0;
    struct iovec point[2];
    size_t arr_len           = 0;
    struct sp_cbb_Arr arr[2] = {0};

    arr_len = sp_cbb_read_buffer(buffer, arr);

    for (; (size_t)points < arr_len; ++points) {
      point[points].iov_base = arr[points].base;
      point[points].iov_len  = arr[points].len;
    }

    if (points == 0) {
      break;
    }

    written = writev(*fd, point, points);
    res     = errno;
    if (written > 0) {
      sp_cbb_consume_bytes(buffer, (size_t)written);
    }

  } while ((written < 0 && res == EAGAIN) && sp_cbb_remaining_read(buffer) > 0);

  return -res;
}

//==============================
struct sp_sink *
sp_sink_init(sp_sink_write_cb w, size_t cap, void *arg)
{
  struct sp_sink *result;
  assert(w);

  if ((result = calloc(1, sizeof(*result)))) {
    result->write_cb = w;
    result->buffer   = sp_cbb_init(cap);
    result->arg      = arg;
  }

  return result;
}

//==============================
bool
sp_sink_write(struct sp_sink *self, const void *in, size_t length)
{
  bool res = true;
  if (length > sp_cbb_capacity(self->buffer)) {
    res = false;
    goto Lout;
  }

  if (length > sp_cbb_remaining_write(self->buffer)) {
    if ((sp_sink_flush(self)) != 0) {
      res = false;
      goto Lout;
    }
  }

  if (!sp_cbb_write(self->buffer, in, length)) {
    res = false;
    goto Lout;
  }

Lout:
  return res;
}

bool
sp_sink_write_cbb(struct sp_sink *self, struct sp_cbb *in)
{
  bool res = false;
  if (sp_cbb_remaining_read(in) > sp_cbb_capacity(self->buffer)) {
    goto Lout;
  }

  if (sp_cbb_remaining_read(in) > sp_cbb_remaining_write(self->buffer)) {
    if (sp_sink_flush(self) != 0) {
      goto Lout;
    }
  }

  if (!sp_cbb_write_cbb(self->buffer, in)) {
    goto Lout;
  }

  res = true;
Lout:
  return res;
}

//==============================
size_t
sp_sink_push_back(struct sp_sink *self, const void *in, size_t in_len)
{
  const uint8_t *it = in;
  size_t written;
  size_t result = 0;

Lit:
  written = sp_cbb_push_back(self->buffer, it, in_len);
  assert(written <= in_len);

  in_len -= written;
  it += written;
  result += written;

  if (in_len > 0) {
    //XXX how to handle error code?
    if (sp_sink_flush(self) == 0) {
      goto Lit;
    }
  }

  return result;
}

//==============================
int
sp_sink_flush(struct sp_sink *self)
{
  return self->write_cb(self->buffer, self->arg);
}

//==============================
bool
sp_sink_is_empty(const struct sp_sink *self)
{
  return sp_cbb_is_empty(self->buffer);
}

//==============================
int
sp_sink_free(struct sp_sink **pself)
{
  assert(pself);
  if (*pself) {
    struct sp_sink *self = *pself;
    sp_sink_flush(self);
    sp_cbb_clear(self->buffer);
    sp_cbb_free(&self->buffer);
    free(self);
    *pself = NULL;
  }

  return 0;
}

//==============================
int
sp_sink_mark(struct sp_sink *self, sp_sink_mark_t *out)
{
  sp_cbb_mark_t mark = {0};

  assert(self);
  assert(out);
  /* just to try to ensure that not double marked the same $out */
  assert(out->before == 0);

  /* TODO: the purpose of this mark is to only allow flush:ing of unmarked bytes */

  sp_cbb_write_mark(self->buffer, &mark);
  out->before   = mark.before;
  out->rollback = mark.rollback;

  return 0;
}

int
sp_sink_unmark(struct sp_sink *self, const sp_sink_mark_t *in)
{
  sp_cbb_mark_t mark = {
    .before   = in->before,
    .rollback = in->rollback,
  };
  assert(self);
  return sp_cbb_write_unmark(self->buffer, &mark);
}

bool
sp_sink_is_marked(const struct sp_sink *self)
{
  return sp_cbb_is_write_mark(self->buffer);
}

//==============================
void
sp_sink_get_internal_state(struct sp_sink *self,
                           sp_sink_write_cb *w,
                           struct sp_cbb **buffer,
                           void **arg)
{
  assert(self);
  if (w) {
    *w = self->write_cb;
  }
  if (buffer) {
    *buffer = self->buffer;
  }
  if (arg) {
    *arg = self->arg;
  }
}

void
sp_sink_set_internal_state(struct sp_sink *self,
                           sp_sink_write_cb w,
                           struct sp_cbb *buffer,
                           void *arg)
{
  assert(self);
  if (w) {
    self->write_cb = w;
  }
  if (buffer) {
    self->buffer = buffer;
  }
  if (arg) {
    self->arg = arg;
  }
}

//==============================
