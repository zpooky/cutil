#ifndef _SP_STRING_UTIL_H
#define _SP_STRING_UTIL_H

#include <stdbool.h>
#include <stddef.h>

// ========================================
bool
sp_str_util_ends_with0(const char *str, size_t l_str, const char *suffix);

bool
sp_str_util_ends_with(const char *str, const char *suffix);

/* bool
 * sp_str_util_ends_withi(const char *str, const char *suffix);
 */

// ========================================
int
sp_str_util_append(char *dest,
                   size_t dest_capacity,
                   size_t *l_dest,
                   const char *src,
                   size_t l_src);

int
sp_str_util_append_char(char *dest,
                        size_t dest_capacity,
                        size_t *l_dest,
                        char src);

// ========================================
bool
sp_str_to_ll(const char *in, long long *out);

bool
sp_str_to_ull(const char *in, unsigned long long *out);

bool
sp_str_to_ul(const char *in, unsigned long *out);

bool
sp_str_to_ui(const char *in, unsigned int *out);

bool
sp_str_to_ull_max(const char *in,
                  unsigned long long *out,
                  unsigned long long max);

bool
sp_str_to_ul_max(const char *in, unsigned long *out, unsigned long max);

bool
sp_str_to_ui_max(const char *in, unsigned int *out, unsigned int max);

#if 0
#define sp_str_to_generic_max(in, out, max)                                    \
  _Generic((out), unsigned long long                                           \
           : sp_str_to_ull_max, default                                        \
           : sp_str_to_ull_max)(in, out, max)
#endif

// ========================================

#endif
