#ifndef _SP_BUFFER_H
#define _SP_BUFFER_H

#include <stddef.h>
#include <stdbool.h>

//==============================
struct sp_buf;
typedef void sp_buf_T;

//==============================
struct sp_buf *
sp_buf_init(size_t cap);

//==============================
size_t
sp_buf_length(const struct sp_buf *);

//==============================
bool
sp_buf_is_empty(const struct sp_buf *);

//==============================
void
sp_buf_clear(struct sp_buf *);

//==============================
bool
sp_buf_push_back(struct sp_buf *, sp_buf_T *);

//==============================
bool
sp_buf_peek_front(const struct sp_buf *, sp_buf_T **out);

bool
sp_buf_peek_back(const struct sp_buf *, sp_buf_T **out);

//==============================
bool
sp_buf_pop_front(struct sp_buf *, sp_buf_T **out);

bool
sp_buf_pop_back(struct sp_buf *, sp_buf_T **out);

//==============================
int
sp_buf_free(struct sp_buf **);

//==============================

#endif
