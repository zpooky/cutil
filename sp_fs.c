#define _GNU_SOURCE
#include "sp_fs.h"

#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <libgen.h> //dirname&basename

#include "sp_str.h"

/*
 * path       dirname   basename
 * /usr/lib   /usr      lib
 * /usr/      /         usr
 * usr        .         usr
 * /          /         /
 * .          .         .
 * ..         .         ..
 */
//==============================
static const char *
sp_fs_basename_len(const char *path, size_t len)
{
  const char *last;

  if (len > 0) {
    if (len == 1) {
      if (*path == '/') {
        return "/";
      }

      goto Lerr;
    }

    last = path + (len - 1);
    if (*last != '/') {
      return basename((char *)path);
    } else {
      assert(false);
    }
  }

Lerr:
  return NULL;
}

const char *
sp_fs_basename(const char *path)
{
  assert(path);

  return sp_fs_basename_len(path, strlen(path));
}

const char *
sp_fs_basename_str(const struct sp_str *path)
{
  assert(path);

  return sp_fs_basename_len(sp_str_c_str(path), sp_str_length(path));
}

//==============================
/*
 * struct sp_str
 * sp_fs_dirname_str(const struct sp_str *in)
 * {
 *   struct sp_str result;
 * 
 *   assert(in);
 *   sp_str_init(&result, 0);
 * 
 *   dirname(sp_str_c_str(in));
 * 
 *   return result;
 * }
 */

//==============================
