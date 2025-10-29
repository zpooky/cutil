#ifndef _SP_HASHSET_H
#define _SP_HASHSET_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sp_callbacks.h>

//==============================
struct sp_hashset;

typedef void sp_T;

//==============================
typedef bool (*sp_hashset_clear_cb)(sp_T *, size_t sz, void *closure);

typedef int (*sp_hashset_for_each_cb)(const sp_T *, void *closure, size_t sz);

struct sp_hashset *
sp_hashset_new(size_t align,
               size_t sz,
               sp_cb_hash hash,
               sp_cb_move move,
               sp_cb_eq cmp);

int
sp_hashset_free(struct sp_hashset **);

//==============================
void
sp_hashset_ensure_capacity(struct sp_hashset *self, size_t capacity);

//==============================
void
sp_hashset_set_clear_cb(struct sp_hashset *self,
                        sp_hashset_clear_cb cb,
                        void *closure);

void
sp_hashset_clear(struct sp_hashset *self);

//==============================
sp_T *
sp_hashset_insert_move(struct sp_hashset *self, sp_T *in);

//==============================
sp_T *
sp_hashset_upsert_move(struct sp_hashset *self, sp_T *in);

//==============================
sp_T *
sp_hashset_lookup(struct sp_hashset *self, sp_T *);

//==============================
bool
sp_hashset_remove(struct sp_hashset *self, sp_T *);

//==============================
bool
sp_hashset_is_empty(const struct sp_hashset *self);

size_t
sp_hashset_length(const struct sp_hashset *self);

//==============================
void
sp_hashset_dump(struct sp_hashset *self);

//==============================
int
sp_hashset_for_each(const struct sp_hashset *self,
                    sp_hashset_for_each_cb,
                    void *closure);

//==============================
#endif
