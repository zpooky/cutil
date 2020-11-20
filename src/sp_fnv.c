#include "sp_fnv.h"

//==============================
uint32_t
sp_fnv1a_hash(const void *raw, size_t l)
{
  /*2166136261*/
  uint32_t FNV_offset_basis = 0x811c9dc5;
  return sp_fnv1a_hash_update(raw, l, FNV_offset_basis);
}

/* fnv1a:
 * hash = offset_basis
 * for each octet_of_data to be hashed
 *         hash = hash xor octet_of_data
 *         hash = hash * FNV_prime
 * return hash
 */
uint32_t
sp_fnv1a_hash_update(const void *raw, size_t l, uint32_t hash)
{
  /*16777619*/
  const uint32_t FNV_PRIME = 0x01000193;
  const uint8_t *it        = raw;
  const uint8_t *const end = it + l;

  for (; it != end; ++it) {
    hash = hash ^ (uint32_t)*it;
    hash = hash * FNV_PRIME;
  }

  return hash;
}

//==============================
uint32_t
sp_fnv1_hash(const void *raw, size_t l)
{
  /*2166136261*/
  uint32_t FNV_offset_basis = 0x811c9dc5;
  return sp_fnv1_hash_update(raw, l, FNV_offset_basis);
}

/* fnv1:
 * hash = offset_basis
 * for each octet_of_data to be hashed
 *         hash = hash * FNV_prime
 *         hash = hash xor octet_of_data
 * return hash
 */
uint32_t
sp_fnv1_hash_update(const void *raw, size_t l, uint32_t hash)
{
  /*16777619*/
  const uint32_t FNV_PRIME = 0x01000193;
  const uint8_t *it        = raw;
  const uint8_t *const end = it + l;

  for (; it != end; ++it) {
    hash = hash * FNV_PRIME;
    hash = hash ^ (uint32_t)*it;
  }

  return hash;
}

//==============================
