#ifndef _SP_HASH_FNV_H
#define _SP_HASH_FNV_H

#include <stdint.h>
#include <stddef.h>

//==============================
uint32_t
sp_fnv1a_hash(const void *, size_t);

uint32_t
sp_fnv1a_hash_update(const void *, size_t, uint32_t h);

//==============================
uint32_t
sp_fnv1_hash(const void *, size_t);

uint32_t
sp_fnv1_hash_update(const void *, size_t, uint32_t h);

//==============================

#endif
