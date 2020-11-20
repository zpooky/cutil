#include "sp_djb.h"

// https://softwareengineering.stackexchange.com/questions/49550/which-hashing-algorithm-is-best-for-uniqueness-and-speed

//==============================
uint32_t
sp_djb2_hash_update(const void *buf, size_t length, uint32_t hash)
{
  const unsigned char *it  = (const unsigned char *)buf;
  const unsigned char *end = it + length;

  for (; it != end; ++it) {
    uint32_t c = *it;
    hash       = ((hash << 5) + hash) + c;
  }

  return hash;
}

uint32_t
sp_djb2_hash(const void *buf, size_t length)
{
  uint32_t hash = 5381;
  return sp_djb2_hash_update(buf, length, hash);
}

//==============================
uint32_t
sp_djb2a_hash_update(const void *buf, size_t length, uint32_t hash)
{
  const unsigned char *it  = (const unsigned char *)buf;
  const unsigned char *end = it + length;

  for (; it != end; ++it) {
    uint32_t c = *it;
    hash       = (33 * hash) ^ c;
  }

  return hash;
}

uint32_t
sp_djb2a_hash(const void *buf, size_t length)
{
  uint32_t hash = 5381;
  return sp_djb2a_hash_update(buf, length, hash);
}

//==============================
