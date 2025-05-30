#include "sp_str.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

#include "sp_util.h"

//==============================
void
sp_str_view_init_str(sp_str_view *self, struct sp_str *str)
{
  self->raw = sp_str_c_str(str);
  self->len = str->length;
}

//==============================
#define SBUF_CAP (SP_STR_SBUF_SIZE + 1)

//==============================
static inline bool
is_static_alloc(size_t cap)
{
  return cap < SBUF_CAP;
}

int
sp_str_init0(sp_str *self)
{
  assertx(self);

  self->length = 0;
  memset(self->sbuf, '\0', sizeof(self->sbuf));
  self->capacity = SP_STR_SBUF_SIZE;

  return 0;
}

int
sp_str_init(struct sp_str *self, size_t cap)
{
  assertx(self);

  if (is_static_alloc(cap)) {
    sp_str_init0(self);
  } else {
    self->buf      = calloc(cap + 1, sizeof(char));
    self->capacity = cap;
  }
  self->length = 0;

  return 0;
}

int
sp_str_init_cstr_len(struct sp_str *self, const char *o, size_t len)
{
  int res;
  if ((res = sp_str_init(self, len)) == 0) {
    sp_str_append_len(self, o, len);
  }

  return res;
}

int
sp_str_init_cstr(struct sp_str *self, const char *o)
{
  assertx(self);
  assertx(o);

  return sp_str_init_cstr_len(self, o, strlen(o));
}

int
sp_str_init_str(sp_str *self, const sp_str *o)
{
  int res;
  size_t o_len;

  assertx(self);
  assertx(o);

  o_len = sp_str_length(o);
  if ((res = sp_str_init(self, o_len)) == 0) {
    sp_str_append_len(self, sp_str_c_str(o), o_len);
  }

  return res;
}

int
sp_str_init_str_view(sp_str *self, sp_str_view o)
{
  return sp_str_init_cstr_len(self, o.raw, o.len);
}

//==============================
sp_str *
sp_str_new(size_t cap)
{
  sp_str *result;
  if ((result = calloc(1, sizeof(*result)))) {
    sp_str_init(result, cap);
  }

  return result;
}

sp_str *
sp_str_new_cstr_len(const char *str, size_t len)
{
  sp_str *result;

  if ((result = calloc(1, sizeof(*result)))) {
    sp_str_init_cstr_len(result, str, len);
  }

  return result;
}

sp_str *
sp_str_new_cstr(const char *str)
{
  assertx(str);

  return sp_str_new_cstr_len(str, strlen(str));
}

sp_str *
sp_str_new_str(const sp_str *str)
{
  sp_str *result;
  if ((result = calloc(1, sizeof(*result)))) {
    sp_str_init_str(result, str);
  }

  return result;
}

//==============================
size_t
sp_str_length(const struct sp_str *self)
{
  assertx(self);

  return self->length;
}

//==============================
bool
sp_str_is_empty(const sp_str *self)
{
  return sp_str_length(self) == 0;
}

//==============================
int
sp_str_free(struct sp_str *self)
{
  assertx(self);

  if (!is_static_alloc(self->capacity)) {
    free(self->buf);
  }

  self->buf      = NULL;
  self->length   = 0;
  self->capacity = 0;

  sp_str_init(self, 0);

  return 0;
}

//==============================
static char *
sp_str_c_str_mut(struct sp_str *self)
{
  assertx(self);
  assertx(self->capacity > 0);

  if (is_static_alloc(self->capacity)) {
    return self->sbuf;
  }

  return self->buf;
}

const char *
sp_str_c_str(const struct sp_str *self)
{
  assertx(self);
  assertx(self->capacity == 0 ? self->sbuf[0] == '\0' : 1);

  if (is_static_alloc(self->capacity)) {
    return self->sbuf;
  }

  return self->buf;
}

//==============================
int
sp_str_append_len(struct sp_str *self, const char *o, size_t len)
{
  char *dest;

  assertx(self);
  assertx(self->capacity > 0);
  assertx(o);

  sp_str_ensure_capacity(self, self->length + len);
  assertx((self->length + len) <= self->capacity);

  dest = sp_str_c_str_mut(self) + self->length;
  memcpy(dest, o, len);
  self->length += len;

  return 0;
}

int
sp_str_append(struct sp_str *self, const char *o)
{
  assertx(self);
  assertx(self->capacity > 0);
  assertx(o);

  return sp_str_append_len(self, o, strlen(o));
}

int
sp_str_append_str(struct sp_str *self, const struct sp_str *o)
{
  assertx(self);
  assertx(self->capacity > 0);
  assertx(o);

  return sp_str_append_len(self, sp_str_c_str(o), sp_str_length(o));
}

int
sp_str_append_str_view(sp_str *self, sp_str_view o)
{
  assertx(self);
  assertx(self->capacity > 0);

  return sp_str_append_len(self, o.raw, o.len);
}

int
sp_str_append_char(sp_str *self, char c)
{
  return sp_str_append_len(self, &c, 1);
}

int
sp_str_appends(sp_str *self, ...)
{
  va_list ap;
  const char *it;

  /* $self is the the last argument before the variable argument list */
  va_start(ap, self);
  while ((it = va_arg(ap, const char *))) {
    sp_str_append(self, it);
  }
  va_end(ap);

  return 0;
}

//==============================
int
sp_str_replace_char(sp_str *self, char needle, char replace)
{
  char *it;
  assertx(self);
  assertx(needle != '\0');
  assertx(replace != '\0');

  for (it = sp_str_c_str_mut(self); *it != '\0'; ++it) {
    if (*it == needle) {
      *it = replace;
    }
  }

  return 0;
}

//==============================
int
sp_str_cmp(const struct sp_str *self, const char *o)
{
  assertx(self);

  //TODO does this take into account of length of strength?
  return strcmp(sp_str_c_str(self), o);
}

int
sp_str_cmp_str(const struct sp_str *self, const struct sp_str *other)
{
  assertx(other);

  return sp_str_cmp(self, sp_str_c_str(other));
}

//==============================
static int
sp_str_prefix_cmp_len(const sp_str *self, const char *prefix, size_t plen)
{
  if (plen > sp_str_length(self)) {
    return -1;
  }

  return strncmp(sp_str_c_str(self), prefix, plen);
}

int
sp_str_prefix_cmp(const sp_str *self, const char *prefix)
{
  assertx(self);
  assertx(prefix);

  return sp_str_prefix_cmp_len(self, prefix, strlen(prefix));
}

int
sp_str_prefix_cmp_str(const sp_str *self, const sp_str *prefix)
{
  assertx(self);
  assertx(prefix);

  return sp_str_prefix_cmp_len(self, sp_str_c_str(prefix),
                               sp_str_length(prefix));
}

//==============================
static int
sp_str_suffix_cmp_len(const sp_str *self, const char *suffix, size_t slen)
{
  size_t offset;
  if (slen > sp_str_length(self)) {
    return -1;
  }
  offset = sp_str_length(self) - slen;

  return strncmp(sp_str_c_str(self) + offset, suffix, slen);
}

int
sp_str_suffix_cmp(const sp_str *self, const char *suffix)
{
  assertx(self);
  assertx(suffix);

  return sp_str_suffix_cmp_len(self, suffix, strlen(suffix));
}

int
sp_str_suffix_cmp_str(const sp_str *self, const sp_str *suffix)
{
  assertx(self);
  assertx(suffix);

  return sp_str_suffix_cmp_len(self, sp_str_c_str(suffix),
                               sp_str_length(suffix));
}

//==============================
int
sp_str_clear(sp_str *self)
{
  char *raw;

  raw = sp_str_c_str_mut(self);
  assertx(raw);

  memset(raw, '\0', self->capacity);
  self->length = 0;

  return 0;
}

//==============================
void
sp_str_swap(struct sp_str *f, struct sp_str *s)
{
  char tmp[SP_STR_SBUF_SIZE] = {0};
  assertx(f);
  assertx(s);
  /*this will memcpy $buf as well*/
  memcpy(tmp, f->sbuf, sizeof(tmp));
  memcpy(f->sbuf, s->sbuf, sizeof(tmp));
  memcpy(s->sbuf, tmp, sizeof(tmp));

  sp_util_swap_size_t(&f->length, &s->length);
  sp_util_swap_size_t(&f->capacity, &s->capacity);
}

//==============================
void
sp_str_ensure_capacity(sp_str *self, size_t capacity)
{
  if (self->capacity < capacity) {
    struct sp_str tmp = {0};
    sp_str_init(&tmp, sp_max(capacity, sp_max(16, self->capacity * 2)));
    sp_str_append_str(&tmp, self);
    sp_str_swap(&tmp, self);
    sp_str_free(&tmp);
  }
}

//==============================
bool
sp_str_is_printable(const sp_str *str)
{
  return sp_util_is_printable(sp_str_c_str(str), str->length);
}

bool
sp_str_view_is_printable(const sp_str_view str)
{
  return sp_util_is_printable(str.raw, str.len);
}

//==============================
const char *
sp_debug_sp_str(const struct sp_str *self)
{
  static char buf[256] = {'\0'};
  if (!self)
    return "NULL";
  snprintf(buf, sizeof(buf), "sp_str(%p)[buf[%s]length[%zu]capacity[%zu]]",
           self, sp_str_c_str(self), self->length, self->capacity);
  return buf;
}

//==============================
