#ifndef _SP_SINK_H
#define _SP_SINK_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

//==============================
struct sp_cbb;

//==============================
struct sp_sink;

typedef int (*sp_sink_write_cb)(struct sp_cbb *, void *);

int
sp_sink_file_write_out(struct sp_cbb *, void *);

//==============================
struct sp_sink *
sp_sink_init(sp_sink_write_cb, size_t cap, void *arg);

//==============================
bool
sp_sink_write(struct sp_sink *, const void *, size_t);

bool
sp_sink_write_cbb(struct sp_sink *, struct sp_cbb *);

//==============================
size_t
sp_sink_push_back(struct sp_sink *, const void *, size_t);

//==============================
int
sp_sink_flush(struct sp_sink *);

//==============================
bool
sp_sink_is_empty(const struct sp_sink *);

//==============================
int
sp_sink_error(const struct sp_sink *);

//==============================
int
sp_sink_free(struct sp_sink **);

//==============================
typedef void (*sp_sink_mark_t_commit_hook)(struct sp_cbb *, void *closure);
typedef struct {
  size_t before;
  bool rollback;

  size_t l_commit_hooks;
  sp_sink_mark_t_commit_hook commit_hooks[4];
  void *commit_closure[4];
} sp_sink_mark_t;

int
sp_sink_mark(struct sp_sink *, sp_sink_mark_t *out);

int
sp_sink_unmark(struct sp_sink *, const sp_sink_mark_t *in);

bool
sp_sink_is_marked(const struct sp_sink *);

//==============================
void
sp_sink_get_internal_state(struct sp_sink *self,
                           sp_sink_write_cb *w,
                           struct sp_cbb **buffer,
                           void **arg);

void
sp_sink_set_internal_state(struct sp_sink *self,
                           sp_sink_write_cb w,
                           struct sp_cbb *buffer,
                           void *arg);

//==============================
size_t sp_sink_debug_in_cbb(const struct sp_sink *self);

//==============================
#endif
