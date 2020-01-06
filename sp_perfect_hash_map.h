#ifndef SP_PERFECT_HASH_MAP_H
#define SP_PERFECT_HASH_MAP_H

#include <stddef.h>
#include <stdint.h>

/* ======================================== */
struct sp_pair;

/* ======================================== */
struct sp_phash_map;

typedef void sp_phash_map_Key;
typedef void sp_phash_map_Value;

/* ======================================== */
typedef uint32_t (*sp_phash_map_hash_cb)(uint32_t seed,
                                         sp_phash_map_Key *,
                                         void *closure);

struct sp_phash_map *
sp_phash_map_init(struct sp_pair *,
                  size_t,
                  sp_phash_map_hash_cb,
                  void *closure);

void
sp_phash_map_free(struct sp_phash_map **);

/* ======================================== */
sp_phash_map_Value *
sp_phash_map_lookup(struct sp_phash_map *, sp_phash_map_Key *);

/* ======================================== */

#endif
