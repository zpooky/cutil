#ifndef _SP_SOURCE_H
#define _SP_SOURCE_H

#include <stddef.h>
#include <stdbool.h>

//==============================
struct sp_cbb;
struct sp_source;

typedef int (*sp_source_read_cb)(struct sp_cbb *, void *);

//==============================
struct sp_source *
sp_source_init(sp_source_read_cb, size_t cap, void *arg);

//==============================
int
sp_source_free(struct sp_source **);

//==============================
bool
sp_source_read(struct sp_source *, void *, size_t);

//==============================
size_t
sp_source_pop_front(struct sp_source *, void *, size_t);

//==============================
size_t
sp_source_peek_front(const struct sp_source *, void *, size_t);

//==============================
size_t
sp_source_capacity(const struct sp_source *);

//==============================
typedef struct {
  size_t before;
  bool rollback;
} sp_source_mark_t;

int
sp_source_mark(struct sp_source *, sp_source_mark_t *out);

int
sp_source_unmark(struct sp_source *, const sp_source_mark_t *);

//==============================
bool
sp_source_consume_bytes(struct sp_source *, size_t);

//==============================
struct sp_cbb *
sp_source_reaonly_view(struct sp_source *, size_t length);

struct sp_cbb *
sp_source_consume_reaonly_view(struct sp_source *, size_t length);

//==============================
void
sp_source_dump_hex(const struct sp_source *);

//==============================
void
sp_source_get_internal_state(struct sp_source *self,
                             sp_source_read_cb *r,
                             void **arg);

void
sp_source_set_internal_state(struct sp_source *self,
                             sp_source_read_cb r,
                             void *arg);

//==============================

#endif
