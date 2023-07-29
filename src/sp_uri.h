#ifndef _SP_URI_H
#define _SP_URI_H

#include <linux/limits.h> //PATH_MAX

#include "sp_str.h"

//==============================
struct sp_URI;

typedef struct sp_uri2 {
  char buf[PATH_MAX];
} sp_uri2;

const char *
sp_debug_sp_uri2(const struct sp_uri2 *in);

//==============================
struct sp_URI *
sp_uri_init0(void);

struct sp_URI *
sp_uri_init(const char *);

struct sp_URI *
sp_uri_init_str(const sp_str *);

struct sp_URI *
sp_uri_init_cpy(const struct sp_URI *);

//==============================
int
sp_uri2_init0(struct sp_uri2 *);

int
sp_uri2_init(struct sp_uri2 *, const char *);

int
sp_uri2_initl(struct sp_uri2 *, const char *, size_t);

int
sp_uri2_init_str(struct sp_uri2 *, const sp_str *);

int
sp_uri2_init_cpy(struct sp_uri2 *, const struct sp_uri2 *);

int
sp_uri2_init_cpy_dirname(struct sp_uri2 *, const struct sp_uri2 *);

//==============================
sp_str
sp_uri_basename(const struct sp_URI *);

const char *
sp_uri2_basename(const struct sp_uri2 *);

/* void */
/* sp_uri_dirname(const struct sp_URI *self, struct sp_URI *out); */

void
sp_uri2_dirname(const struct sp_uri2 *self, struct sp_uri2 *out);

sp_str
sp_uri_path(const struct sp_URI *);

char *
sp_uri2_path(const struct sp_uri2 *);

//==============================
int
sp_uri2_realpath(struct sp_uri2 *);

int
sp_uri2_normalize(struct sp_uri2 *);

//==============================
/* TODO appendf printf style */
int
sp_uri_append(struct sp_URI *, const char *);

int
sp_uri2_append_len(struct sp_uri2 *, const char *e, size_t elen);

int
sp_uri2_append(struct sp_uri2 *, const char *);

int
sp_uri_append_str(struct sp_URI *, const sp_str *);

int
sp_uri2_append_str(struct sp_uri2 *, const sp_str *);

//==============================
int
sp_uri_drop_head(struct sp_URI *);

int
sp_uri2_drop_head(struct sp_uri2 *);

//==============================
struct sp_vec;

struct sp_vec * /*sp_str*/
sp_uri_path_elements(const struct sp_URI *);

struct sp_vec * /*sp_str*/
sp_uri2_path_elements(const struct sp_uri2 *);

//==============================
bool
sp_uri2_eq(const sp_uri2 *, const sp_uri2 *);

bool
sp_uri2_prefix_eq(const sp_uri2 *self, const sp_uri2 *prefix);

//==============================
int
sp_uri2_clear(sp_uri2 *);

//==============================
int
sp_uri2_join(sp_uri2 *, const sp_uri2 *);

//==============================
int
sp_uri_free(struct sp_URI **);

int
sp_uri2_free(struct sp_uri2 *);

//==============================

#endif
