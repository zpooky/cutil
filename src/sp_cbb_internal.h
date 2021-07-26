#ifndef _SP_CIRCULAR_BYTE_BUFFER_INTERNAL_H
#define _SP_CIRCULAR_BYTE_BUFFER_INTERNAL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

//==============================
#define SP_CBB_HOOKS 8
struct sp_cbb {
  uint8_t *buffer;
  size_t r;
  size_t w;
  size_t capacity;

  int read_only;
  struct sp_cbb *root;
  bool free_buffer;

  int mark_r;
  size_t original_r;

  int mark_w;

  size_t l_read_commit_hooks;
  void (*read_commit_hooks[SP_CBB_HOOKS])(struct sp_cbb *, void *closure);
  void *read_commit_hooks_cloures[SP_CBB_HOOKS];

  size_t l_write_commit_hooks;
  void (*write_commit_hooks[SP_CBB_HOOKS])(struct sp_cbb *, void *closure);
  void *write_commit_hooks_cloures[SP_CBB_HOOKS];
};

static inline void
sp_cbb_init_internal0(struct sp_cbb *self,
                      uint8_t *b,
                      size_t capacity,
                      size_t r,
                      size_t w)
{
  self->buffer   = b;
  self->r        = r;
  self->w        = w;
  self->capacity = capacity;
}

//==============================

#endif
