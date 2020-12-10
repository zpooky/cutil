#ifndef _SP_HASHSET_H
#define _SP_HASHSET_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

//==============================
struct sp_hashset;

typedef void sp_hashset_T;

//==============================
typedef uint32_t (*sp_hashset_hash_cb)(const sp_hashset_T *);
typedef void (*sp_hashset_copy_cb)(sp_hashset_T *,
                                   const sp_hashset_T *,
                                   size_t sz);
typedef bool (*sp_hashset_eq_cb)(const sp_hashset_T *,
                                 const sp_hashset_T *,
                                 size_t sz);

struct sp_hashset *
sp_hashset_init(size_t align,
                size_t sz,
                sp_hashset_hash_cb hash,
                sp_hashset_copy_cb copy,
                sp_hashset_eq_cb cmp);

int
sp_hashset_free(struct sp_hashset **);

//==============================
sp_hashset_T *
sp_hashset_insert(struct sp_hashset *self, sp_hashset_T *needle);

//==============================
sp_hashset_T *
sp_hashset_lookup(struct sp_hashset *self, sp_hashset_T *);

//==============================
bool
sp_hashset_remove(struct sp_hashset *self, sp_hashset_T *);

//==============================
bool
sp_hashset_is_empty(const struct sp_hashset *self);

size_t
sp_hashset_length(const struct sp_hashset *self);

//==============================
void
sp_hashset_dump(struct sp_hashset *self);

//==============================

#endif
