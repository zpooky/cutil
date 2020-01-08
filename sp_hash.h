#ifndef SP_HASH_H
#define SP_HASH_H

#include <stdint.h>
#include <stddef.h>

/* ======================================== */
uint32_t
sp_hash_fnv1a(uint32_t hash, const void *raw, size_t l);

uint32_t
sp_hash_fnv1a0(const void *raw, size_t l);

/* ======================================== */
uint32_t
sp_hash_fnv1(uint32_t hash, const void *raw, size_t l);

uint32_t
sp_hash_fnv1_0(const void *raw, size_t l);

/* ======================================== */

#endif
