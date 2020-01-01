#include "sp_sink.h"
#include <stdlib.h>
#include "sp_cbb.h"
#include <assert.h>
#include <errno.h>

#include <sys/uio.h> //writev

//==============================
struct sp_sink {
  sp_sink_write_out_cb write;
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
sp_sink_init(sp_sink_write_out_cb w, size_t cap, void *arg)
{
  struct sp_sink *result;
  assert(w);

  if ((result = calloc(1, sizeof(*result)))) {
    result->write  = w;
    result->buffer = sp_cbb_init(cap);
    result->arg    = arg;
  }

  return result;
}

//==============================
int
sp_sink_write(struct sp_sink *self, const void *in, size_t length)
{
  int res = 0;
  if (length > sp_cbb_capacity(self->buffer) ||
      length > sp_cbb_remaining_write(self->buffer)) {
    if ((res = sp_sink_flush(self)) < 0) {
      goto Lout;
    }
  }

  if (length > sp_cbb_capacity(self->buffer)) {
    //TODO flush in directly
    /* struct sp_cbb tmp = {0}; */
    /* self->write(&tmp, self->arg); */
    assert(false);
  } else {
    if (!sp_cbb_write(self->buffer, in, length)) {
      res = -ENOMEM;
      goto Lout;
    }
  }

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
  return self->write(self->buffer, self->arg);
}

//==============================
int
sp_sink_free(struct sp_sink **self)
{
  assert(self);
  assert(*self);

  sp_sink_flush(*self);

  sp_cbb_free(&(*self)->buffer);

  free(*self);
  *self = NULL;

  return 0;
}

//==============================
