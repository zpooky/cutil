#ifndef _SP_BITSET_H
#define _SP_BITSET_H

#include <stdbool.h>
#include <stddef.h>

//==============================
struct sp_bitset;
struct sp_cbb;

//==============================
struct sp_bitset *
sp_bitset_init(size_t bits_capacity);

struct sp_bitset *
sp_bitset_init_cbb(struct sp_cbb *in, size_t length);

int
sp_bitset_free(struct sp_bitset **);

//==============================
bool
sp_bitset_test(const struct sp_bitset *self, size_t idx);

//==============================
bool
sp_bitset_set(struct sp_bitset *self, size_t idx, bool v);

//==============================
void
sp_bitset_set_all(struct sp_bitset *self, int v);

//==============================
bool
sp_bitset_is_all(const struct sp_bitset *self, int v);

//==============================
bool
sp_bitset_write_BYTES(const struct sp_bitset *self, struct sp_cbb *out);

bool
sp_bitset_read(struct sp_bitset *self, struct sp_cbb *in, size_t length);

//==============================
size_t
sp_bitset_bits(const struct sp_bitset *self);

//==============================

#endif
