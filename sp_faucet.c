#include "sp_faucet.h"
#include "sp_cbb.h"

#include <assert.h>
#include <stdlib.h>

//==============================
struct sp_faucet {
  size_t read_head;

  struct sp_cbb *buffer;

  sp_faucet_fill_cb fill;

  void *arg;
};

//==============================
struct sp_faucet *
sp_faucet_init(sp_faucet_fill_cb cb, size_t cap, void *arg)
{
  struct sp_faucet *result;
  result = calloc(1, sizeof(struct sp_faucet));
  if (result) {
    result->buffer = sp_cbb_init(cap);
    result->fill = cb;
    result->arg = arg;
  }

  return result;
}

//==============================
int
sp_faucet_free(struct sp_faucet **pself)
{
  assert(pself);

  if (*pself) {
    struct sp_faucet *self = *pself;
    sp_cbb_free(&self->buffer);
    free(self);
    *pself = NULL;
  }

  return 0;
}

//==============================
size_t
sp_faucet_peek_front(struct sp_faucet *self, void *dest, size_t l)
{
  assert(self);

  if (l > sp_cbb_remaining_read(self->buffer)) {
    /* XXX return code */
    self->fill(self->buffer, self->arg);
  }

  return sp_cbb_peek_front(self->buffer, dest, l);
}

//==============================
size_t
sp_faucet_pop_front(struct sp_faucet *self, void *dest, size_t l)
{
  assert(self);

  size_t result = sp_faucet_peek_front(self, dest, l);
  sp_cbb_consume_bytes(self->buffer, result);

  return result;
}

//==============================
bool
sp_faucet_read(struct sp_faucet *self, void *dest, size_t l)
{
  size_t result;
  assert(self);

  if (l > sp_cbb_capacity(self->buffer)) {
    assert(false);
    return false;
  }

  if (l > sp_cbb_remaining_read(self->buffer)) {
    /* XXX return code */
    self->fill(self->buffer, self->arg);

    if (l > sp_cbb_remaining_read(self->buffer)) {
      return false;
    }
  }

  result = sp_cbb_read(self->buffer, dest, l);
  assert(result == l);

  return true;
}

//==============================
