#ifndef _SP_CIRCULAR_BYTE_BUFFER_H
#define _SP_CIRCULAR_BYTE_BUFFER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

//==============================
struct sp_cbb;

//==============================
struct sp_cbb *sp_cbb_init(size_t);

//==============================
size_t
sp_cbb_remaining_write(const struct sp_cbb *);

size_t
sp_cbb_remaining_read(const struct sp_cbb *);

//==============================
size_t
sp_cbb_capacity(const struct sp_cbb *);

//==============================
bool
sp_cbb_is_empty(const struct sp_cbb *);

bool
sp_cbb_is_full(const struct sp_cbb *);

bool
sp_cbb_is_readonly(const struct sp_cbb *);

//==============================
void
sp_cbb_clear(struct sp_cbb *);

//==============================
size_t
sp_cbb_push_back(struct sp_cbb *, const void *, size_t);

//==============================
bool
sp_cbb_write(struct sp_cbb *, const void *, size_t);

bool
sp_cbb_write_cbb(struct sp_cbb *, struct sp_cbb *);

//==============================
struct sp_cbb_Arr {
  uint8_t *base;
  size_t len;
};

size_t
sp_cbb_read_buffer(const struct sp_cbb *, struct sp_cbb_Arr *res);

//==============================
size_t
sp_cbb_write_buffer(struct sp_cbb *, struct sp_cbb_Arr *res);

//==============================
bool
sp_cbb_consume_bytes(struct sp_cbb *self, size_t);

bool
sp_cbb_produce_bytes(struct sp_cbb *self, size_t);

//==============================
size_t
sp_cbb_peek_front(const struct sp_cbb *, /*DEST*/ void *, size_t);

//==============================
size_t
sp_cbb_pop_front(struct sp_cbb *, /*DEST*/ void *, size_t);

//==============================
bool
sp_cbb_read(struct sp_cbb *, /*DEST*/ void *dest, size_t dest_len);

//==============================
uint8_t
sp_cbb_get(const struct sp_cbb *self, size_t idx);

//==============================
int
sp_cbb_free(struct sp_cbb **);

//==============================
typedef struct {
  size_t before;
  bool rollback;

  size_t l_commit_hooks;
  void (*commit_hooks[4])(struct sp_cbb *, void *closure);
  void *commit_hooks_closure[4];
} sp_cbb_mark_t;

int
sp_cbb_read_mark(struct sp_cbb *, sp_cbb_mark_t *out);

int
sp_cbb_read_unmark(struct sp_cbb *, sp_cbb_mark_t *);

int
sp_cbb_write_mark(struct sp_cbb *, sp_cbb_mark_t *out);

int
sp_cbb_write_unmark(struct sp_cbb *, sp_cbb_mark_t *);

//==============================
bool
sp_cbb_is_read_mark(const struct sp_cbb *);

bool
sp_cbb_is_write_mark(const struct sp_cbb *);

size_t sp_cbb_read_mark_length(const struct sp_cbb *, const sp_cbb_mark_t*);

size_t sp_cbb_write_mark_length(const struct sp_cbb *, const sp_cbb_mark_t*);

//==============================
struct sp_cbb *
sp_cbb_readonly_view(struct sp_cbb *self, size_t length);

struct sp_cbb *
sp_cbb_consume_readonly_view(struct sp_cbb *self, size_t length);

//==============================
#endif
