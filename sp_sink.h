#ifndef _SP_SINK_H
#define _SP_SINK_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

//==============================
struct sp_cbb;

//==============================
struct sp_sink;

typedef int (*sp_sink_write_out_cb)(struct sp_cbb *, void *);

int
sp_sink_file_write_out(struct sp_cbb *, void *);

//==============================
struct sp_sink *
sp_sink_init(sp_sink_write_out_cb, size_t cap, void *arg);

//==============================
int
sp_sink_write(struct sp_sink *, const void *, size_t);

int
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
sp_sink_free(struct sp_sink **);

//==============================
typedef int sp_sink_mark_t;

sp_sink_mark_t
sp_sink_mark(struct sp_sink *);

int
sp_sink_unmark(struct sp_sink *, sp_sink_mark_t, bool);

//==============================
#endif
