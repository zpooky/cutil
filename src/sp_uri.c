#include "sp_uri.h"

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sp_fs.h"
#include "sp_str.h"
#include "sp_vec.h"

//==============================
#define SP_URI_BLOCK_MAX (16)

struct sp_URI_block {
  sp_str arr[SP_URI_BLOCK_MAX];
  size_t index;
};

struct sp_URI {
  struct sp_vec /*sp_URI_block*/ *path;
  /* char buf[PATH_MAX]; */
};

//==============================
static struct sp_URI_block *
sp_uri_block_init(void)
{
  return calloc(1, sizeof(struct sp_URI_block *));
}

static int
sp_uri_block_free(struct sp_URI_block *self)
{
  size_t i;

  assert(self);

  for (i = 0; i < self->index; ++i) {
    sp_str_free(&self->arr[i]);
  }

  self->index = 0;
  free(self);

  return 0;
}

//==============================
struct sp_URI *
sp_uri_init0(void)
{
  struct sp_URI *result = NULL;
  struct sp_vec *path;

  if ((path = sp_vec_init())) {
    if ((result = calloc(1, sizeof(*result)))) {
      result->path = path;
    } else {
      sp_vec_free(&path);
    }
  }

  return result;
}

struct sp_URI *
sp_uri_init(const char *path)
{
  struct sp_URI *result;

  assert(path);

  if ((result = sp_uri_init0())) {
    char *token;
    const char *delim = "/";

    token = strtok((char *)path, delim);
    while (token) {
      sp_uri_append(result, token);
      token = strtok(NULL, delim);
    }
  }

  return result;
}

struct sp_URI *
sp_uri_init_str(const sp_str *path)
{
  assert(path);

  return sp_uri_init(sp_str_c_str(path));
}

struct sp_URI *
sp_uri_init_cpy(const struct sp_URI *other)
{
  struct sp_URI *result;
  sp_str path;

  path   = sp_uri_path(other);
  result = sp_uri_init_str(&path);

  sp_str_free(&path);

  return result;
}

//==============================
int
sp_uri2_init0(struct sp_uri2 *self)
{
  assert(self);

  memset(self->buf, '\0', sizeof(self->buf));

  return 0;
}

static int
sp_uri2_append_all(struct sp_uri2 *self, const char *path)
{
  int res               = 0;
  const char *it        = path;
  const char *const end = path + strlen(path);

  while (it < end) {
    const char *next;
    size_t len;

    if (!(next = strchr(it, '/'))) {
      next = end;
    }

    len = ((uintptr_t)next) - ((uintptr_t)it);
    if (len > 0) {
      if ((res = sp_uri2_append_len(self, it, len)) < 0) {
        assert(false);
        sp_uri2_clear(self);
        goto Lout;
      }
    }
    it += len + 1;
  }

Lout:
  return res;
}

int
sp_uri2_init(struct sp_uri2 *self, const char *path)
{
  int res = -ENOMEM;

  assert(self);
  assert(path);

  if (!(res = sp_uri2_init0(self))) {
    res = sp_uri2_append_all(self, path);

    if (strcmp(self->buf, path) != 0) {
      /* printf("==========\n"); */
      /* printf("self->buf: %s\n", self->buf); */
      /* printf("path: %s\n", path); */
      /* fflush(stdout); */
      /* fflush(stderr); */
      /* assert(strcmp(self->buf, path) == 0); */
    }
  }

  return res;
}

int
sp_uri2_init_str(struct sp_uri2 *self, const sp_str *path)
{
  assert(self);
  assert(path);

  return sp_uri2_init(self, sp_str_c_str(path));
}

int
sp_uri2_init_cpy(struct sp_uri2 *self, const struct sp_uri2 *path)
{
  assert(self);
  assert(path);
  assert(self != path);

  return sp_uri2_init(self, sp_uri2_path(path));
}

//==============================
sp_str
sp_uri_basename(const struct sp_URI *self)
{
  sp_str result;
  const struct sp_URI_block *last;

  assert(self);

  sp_str_init(&result, 0);

  if ((last = sp_vec_get_last(self->path))) {
    if (last->index > 0) {
      size_t index;
      index = last->index - 1;

      sp_str_append_str(&result, &last->arr[index]);
    }
  }

  return result;
}

const char *
sp_uri2_basename(const struct sp_uri2 *self)
{
  assert(self);

  return sp_fs_basename(self->buf);
}

sp_str
sp_uri_path(const struct sp_URI *self)
{
  sp_str result;
  struct sp_URI_block **it;
  bool prepend = false;

  assert(self);

  sp_str_init(&result, 0);

  sp_vec_for_each2 (it, self->path) {
    size_t i;

    for (i = 0; i < (*it)->index; ++i) {
      if (prepend) {
        sp_str_append(&result, "/");
        prepend = false;
      }

      sp_str_append_str(&result, &(*it)->arr[i]);

      if (i < ((*it)->index - 1)) {
        sp_str_append(&result, "/");
      } else {
        /* last entry in block */
        prepend = true;
      }
    } // for
  } // for_each

  return result;
}

const char *
sp_uri2_path(const struct sp_uri2 *self)
{
  assert(self);

  return self->buf;
}

//==============================
int
sp_uri_append(struct sp_URI *self, const char *element)
{
  int res = 0;
  struct sp_URI_block *last;

  assert(self);
  assert(element);

  /* TODO sanitize element so to not contain '/' */

Lretry:
  if ((last = sp_vec_get_last(self->path))) {
    if (last->index < SP_URI_BLOCK_MAX) {
      sp_str *entry;

      entry = &last->arr[last->index++];
      sp_str_init_cstr(entry, element);
      goto Lout;
    }
  }

  if ((last = sp_uri_block_init())) {
    sp_vec_append(self->path, last);
    goto Lretry;
  } else {
    res = -ENOMEM;
  }

Lout:
  return res;
}

int
sp_uri2_append_len(struct sp_uri2 *self, const char *elem, size_t elem_len)
{
  int res        = -ENOMEM;
  size_t buf_len = strlen(self->buf);

  assert(elem_len > 0);
  assert(memchr(elem, '/', elem_len) == NULL);
  assert(memchr(elem, ':', elem_len) == NULL);

  if ((buf_len + elem_len + 1) < sizeof(self->buf)) {
    res = 0;
    strcat(self->buf, "/");
    strncat(self->buf, elem, elem_len);
  }

  return res;
}

int
sp_uri2_append(struct sp_uri2 *self, const char *element)
{
  size_t elem_len;

  assert(self);
  assert(element);

  elem_len = strlen(element);

  return sp_uri2_append_len(self, element, elem_len);
}

int
sp_uri_append_str(struct sp_URI *self, const sp_str *element)
{
  assert(self);
  assert(element);

  return sp_uri_append(self, sp_str_c_str(element));
}

int
sp_uri2_append_str(struct sp_uri2 *self, const sp_str *element)
{
  assert(self);
  assert(element);

  return sp_uri2_append(self, sp_str_c_str(element));
}

//==============================
int
sp_uri_drop_head(struct sp_URI *self)
{
  struct sp_URI_block *last;

  assert(self);

  if ((last = sp_vec_get_last(self->path))) {
    assert(last->index > 0);
    last->index--;

    sp_str_free(&last->arr[last->index]);

    if (last->index == 0) {
      void *res;
      size_t idx = sp_vec_length(self->path) - 1;
      res        = sp_vec_remove(self->path, idx);
      assert(res);
      sp_uri_block_free(last);
    }
  } else {
    assert(false);
  }

  return 0;
}

int
sp_uri2_drop_head(struct sp_uri2 *self)
{
  int res = -EINVAL;
  char *last;

  assert(self);

  if ((last = strrchr(self->buf, '/'))) {
    res   = 0;
    *last = '\0';
  }

  return res;
}

//==============================
struct sp_vec * /*sp_str*/
sp_uri_path_elements(const struct sp_URI *self)
{
  struct sp_URI_block **it;
  struct sp_vec * /*sp_str*/ result;
  bool first = true;

  assert(self);

  if (!(result = sp_vec_init())) {
    return NULL;
  }

  sp_vec_for_each2 (it, self->path) {
    size_t i;
    for (i = 0; i < (*it)->index; ++i) {
      const sp_str *cur = (*it)->arr + i;
      if (first) {
        assert(sp_str_cmp(cur, "") == 0);
        first = false;
      } else {
        assert(sp_str_cmp(cur, "") != 0);
        sp_vec_append(result, sp_str_new_str(cur));
      }
    }
  }

  return result;
}

struct sp_vec * /*sp_str*/
sp_uri2_path_elements(const struct sp_uri2 *self)
{
  struct sp_vec * /*sp_str*/ result;
  const char *const path = sp_uri2_path(self);
  const char *it         = path;
  const char *const end  = path + strlen(path);

  if (!(result = sp_vec_init())) {
    return NULL;
  }

  // XXX this is a copy of append_all
  while (it < end) {
    const char *next;
    size_t len;

    if (!(next = strchr(it, '/'))) {
      next = end;
    }

    len = ((uintptr_t)next) - ((uintptr_t)it);
    if (len > 0) {
      sp_str *elem;
      if (!(elem = sp_str_new_cstr_len(it, len))) {
        assert(false);
      }
      sp_vec_append(result, elem);
    }
    it += len + 1;
  }
  goto Lout;

  /* Lerr: */

Lout:
  return result;
}

//==============================
bool
sp_uri2_eq(const sp_uri2 *f, const sp_uri2 *s)
{
  assert(f);
  assert(s);

  return strcmp(f->buf, s->buf) == 0;
}

bool
sp_uri2_prefix_eq(const sp_uri2 *self, const sp_uri2 *prefix)
{
  size_t plen;
  size_t slen;

  assert(self);
  assert(prefix);

  plen = strlen(sp_uri2_path(prefix));
  slen = strlen(sp_uri2_path(self));
  if (plen > slen) {
    return false;
  }

  if (strncmp(sp_uri2_path(self), sp_uri2_path(prefix), plen) != 0) {
    return false;
  }

  if (slen > plen) {
    if (sp_uri2_path(self)[plen] != '/') {
      return false;
    }
  }

  return true;
}

//==============================
int
sp_uri2_clear(sp_uri2 *self)
{
  return sp_uri2_init0(self);
}

//==============================
int
sp_uri2_join(sp_uri2 *self, const sp_uri2 *other)
{
  assert(self);
  assert(other);
  assert(self != other);

  return sp_uri2_append_all(self, sp_uri2_path(other));
}

//==============================
int
sp_uri_free(struct sp_URI **pself)
{
  assert(pself);

  if (*pself) {
    struct sp_URI *self = *pself;
    struct sp_URI_block **it;

    sp_vec_for_each2 (it, self->path) {
      sp_uri_block_free(*it);
    }

    sp_vec_free(&self->path);

    free(self);
    *pself = NULL;
  }

  return 0;
}

int
sp_uri2_free(struct sp_uri2 *self)
{
  return sp_uri2_init0(self);
}

//==============================
