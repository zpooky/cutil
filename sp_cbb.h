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
sp_cbb_length(const struct sp_cbb *);

size_t
sp_cbb_capacity(const struct sp_cbb *);

//==============================
bool
sp_cbb_is_empty(const struct sp_cbb *);

bool
sp_cbb_is_full(const struct sp_cbb *);

//==============================
void
sp_cbb_clear(struct sp_cbb *);

//==============================
size_t
sp_cbb_push_back(struct sp_cbb *, const void *, size_t);

//==============================
bool
sp_cbb_write(struct sp_cbb *, const void *, size_t);

//==============================
struct sp_cbb_Arr {
  uint8_t *base;
  size_t len;
};

size_t
sp_cbb_read_buffer(const struct sp_cbb *, struct sp_cbb_Arr *res);

//==============================
bool
sp_cbb_consume_bytes(struct sp_cbb *self, size_t);

//==============================
size_t
sp_cbb_peek_front(const struct sp_cbb *, /*DEST*/ void *, size_t);

//==============================
size_t
sp_cbb_pop_front(struct sp_cbb *, /*DEST*/ void *, size_t);

//==============================
bool
sp_cbb_read(struct sp_cbb *, /*DEST*/ void *, size_t);

//==============================
int
sp_cbb_free(struct sp_cbb **);

//==============================
#endif