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
sp_source_peek_front(struct sp_source *, void *, size_t);

//==============================
int
sp_source_error(const struct sp_source *);

//==============================
size_t
sp_source_capacity(const struct sp_source *);

//==============================
typedef int (*sp_source_mark_t_commit_hook)(struct sp_cbb *, void *closure);
typedef struct {
  size_t before;
  bool rollback;

  size_t l_commit_hooks;
  sp_source_mark_t_commit_hook commit_hooks[4];
  void *commit_closure[4];
} sp_source_mark_t;

int
sp_source_mark(struct sp_source *, sp_source_mark_t *out);

int
sp_source_unmark(struct sp_source *, const sp_source_mark_t *);

bool
sp_source_is_marked(const struct sp_source *);

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
sp_source_dump_hex(struct sp_source *);

//==============================
void
sp_source_eager_fill(struct sp_source *);

//==============================
void
sp_source_get_internal_state(struct sp_source *self,
                             sp_source_read_cb *r,
                             struct sp_cbb **buffer,
                             void **arg);

void
sp_source_set_internal_state(struct sp_source *self,
                             sp_source_read_cb r,
                             struct sp_cbb *buffer,
                             void *arg);

//==============================
bool
sp_source_ensure_at_least_readable(struct sp_source *self, size_t len);

//==============================
size_t sp_source_debug_in_cbb(const struct sp_source *self);

//==============================
#endif
