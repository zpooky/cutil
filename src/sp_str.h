#ifndef _SP_STRING_H
#define _SP_STRING_H

#include <stddef.h>
#include <stdbool.h>

/* TODO sp_str_append_printf();
 */

//==============================
typedef struct sp_str {
  union {
    /* TODO use internal_ prefix */
    char *buf;
    char sbuf[16];
  };
  size_t length;
  size_t capacity;
} sp_str;

//==============================
typedef struct {
  const char *raw;
  size_t len;
} sp_str_view;

void
sp_str_view_init_str(sp_str_view *self, struct sp_str *str);

//==============================
int
sp_str_init0(sp_str *);

int
sp_str_init(sp_str *, size_t cap);

int
sp_str_init_cstr_len(struct sp_str *, const char *, size_t);

int
sp_str_init_cstr(sp_str *, const char *);

int
sp_str_init_str(sp_str *, const sp_str *);

int
sp_str_init_str_view(sp_str *, sp_str_view);

//==============================
sp_str *
sp_str_new(size_t cap);

sp_str *
sp_str_new_cstr_len(const char *, size_t);

sp_str *
sp_str_new_cstr(const char *);

sp_str *
sp_str_new_str(const sp_str *);

//==============================
size_t
sp_str_length(const sp_str *);

//==============================
bool
sp_str_is_empty(const sp_str *);

//==============================
int
sp_str_free(sp_str *);

//==============================
const char *
sp_str_c_str(const sp_str *);

//==============================
int
sp_str_append_len(sp_str *, const char *, size_t);

int
sp_str_append(sp_str *, const char *);

int
sp_str_append_str(sp_str *, const sp_str *);


int
sp_str_append_str_view(sp_str *, sp_str_view);

int
sp_str_append_char(sp_str *, char);

int
sp_str_appends(sp_str *, ...);

//==============================
int
sp_str_replace_char(sp_str *, char needle, char replace);

//==============================
int
sp_str_cmp(const sp_str *, const char *);

int
sp_str_cmp_str(const sp_str *, const sp_str *);

//==============================
int
sp_str_prefix_cmp(const sp_str *, const char *);

int
sp_str_prefix_cmp_str(const sp_str *, const sp_str *);

//==============================
int
sp_str_suffix_cmp(const sp_str *, const char *);
int
sp_str_suffix_cmp_str(const sp_str *, const sp_str *);

//==============================
int
sp_str_clear(sp_str *);

//==============================
void
sp_str_swap(sp_str *, sp_str *);

//==============================
void
sp_str_ensure_capacity(sp_str *, size_t);

//==============================
bool
sp_str_is_printable(const sp_str *str);

bool
sp_str_view_is_printable(const sp_str_view str);

//==============================
const char *
sp_debug_sp_str(const struct sp_str *in);

//==============================
#endif
