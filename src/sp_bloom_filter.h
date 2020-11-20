#ifndef _SP_BLOOM_FILTER_H
#define _SP_BLOOM_FILTER_H

#include <stddef.h>
#include <stdbool.h>

//==============================
struct sp_bloom_filter;

//==============================
struct sp_bloom_filter *
sp_bloom_filter_init(size_t bits);

void
sp_bloom_filter_free(struct sp_bloom_filter **);

//==============================
bool
sp_bloom_filter_test(const struct sp_bloom_filter *self, const void *, size_t);

//==============================
void
sp_bloom_filter_set(struct sp_bloom_filter *self, const void *, size_t);

//==============================
void
sp_bloom_filter_clear(struct sp_bloom_filter *self);

//==============================

#endif
