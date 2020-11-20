#ifndef _SP_HASH_DJB_H
#define _SP_HASH_DJB_H

#include <stddef.h>
#include <stdint.h>

//==============================
uint32_t
sp_djb2_hash_update(const void *buf, size_t length, uint32_t hash);

uint32_t
sp_djb2_hash(const void *buf, size_t length);

//==============================
uint32_t
sp_djb2a_hash_update(const void *buf, size_t length, uint32_t hash);

uint32_t
sp_djb2a_hash(const void *buf, size_t length);

//==============================
#endif
