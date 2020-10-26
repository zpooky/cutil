#ifndef _SP_BITSET_H
#define _SP_BITSET_H

#include <stdbool.h>
#include <stddef.h>

//==============================
struct sp_bitset;

//==============================
struct sp_bitset *
sp_bitset_init(size_t bits_capacity);

int
sp_bitset_free(struct sp_bitset **);

//==============================
bool
sp_bitset_test(struct sp_bitset *self, size_t idx);

//==============================
bool
sp_bitset_set(struct sp_bitset *self, size_t idx, bool v);

//==============================

#endif
