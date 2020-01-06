#include "sp_hash.h"

/* ======================================== */
/* fnv1:
 * hash = offset_basis
 * for each octet_of_data to be hashed
 *         hash = hash * FNV_prime
 *         hash = hash xor octet_of_data
 * return hash
 */

/* fnv1a:
 * hash = offset_basis
 * for each octet_of_data to be hashed
 *         hash = hash xor octet_of_data
 *         hash = hash * FNV_prime
 * return hash
 */
uint32_t
sp_hash_fnv1a(uint32_t hash, const void *raw, size_t l)
{
  const uint32_t FNV_PRIME = 16777619;
  const uint8_t *it        = raw;
  const uint8_t *const end = it + l;

  for (; it != end; ++it) {
    hash = hash ^ *it;
    hash = hash * FNV_PRIME;
  }

  return hash;
}

uint32_t
sp_hash_fnv1a0(const void *raw, size_t l)
{
  const uint32_t offset_basis = 2166136261;
  return sp_hash_fnv1a(offset_basis, raw, l);
}

/* ======================================== */
