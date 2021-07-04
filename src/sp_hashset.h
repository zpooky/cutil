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
typedef void (*sp_hashset_copy_cb)(sp_hashset_T *dest,
                                   const sp_hashset_T *src,
                                   size_t sz);
typedef bool (*sp_hashset_eq_cb)(const sp_hashset_T *f,
                                 const sp_hashset_T *s,
                                 size_t sz);

typedef bool (*sp_hashset_clear_cb)(sp_hashset_T *, size_t sz, void *closure);

typedef void (*sp_hashset_for_each_cb)(const sp_hashset_T *,
                                       void *closure,
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
void
sp_hashset_set_clear_cb(struct sp_hashset *self,
                        sp_hashset_clear_cb cb,
                        void *closure);

void
sp_hashset_clear(struct sp_hashset *self);

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
void
sp_hashset_memcpy(sp_hashset_T *dest, const sp_hashset_T *src, size_t sz);

//==============================
void
sp_hashset_for_each(const struct sp_hashset *self,
                    sp_hashset_for_each_cb,
                    void *closure);

//==============================
#endif
