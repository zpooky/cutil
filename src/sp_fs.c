#define _GNU_SOURCE
#include "sp_fs.h"
#include "sp_util.h"

#include <unistd.h>
#include <fcntl.h>

#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <libgen.h>

#include <linux/limits.h> //PATH_MAX
#include <errno.h>

#include <sys/stat.h>

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
int
sp_fs_mkdirs(const char *path, mode_t mode)
{
  size_t plen        = strlen(path);
  char buf[PATH_MAX] = {0};
  char *const b_end  = buf + plen;
  char *b_it;
  struct stat st = {0};
  int res        = 0;
  bool first     = true;

  if (plen > sizeof(buf)) {
    errno = ENAMETOOLONG;
    return -1;
  }
  if (plen == 0) {
    errno = EFAULT;
    return -1;
  }
  b_it = b_end - 1;

  strncpy(buf, path, plen);

  if (!mode) {
    mode = S_IRUSR | S_IWUSR | S_IXUSR;
  }

  do {
    if ((res = stat(buf, &st)) < 0) {
      if (errno == ENOENT) {
        while (b_it > buf) {
          if (*b_it == '/') {
            *b_it = '\0';
            break;
          }
          --b_it;
        } //while
        if (b_it == buf) {
          goto Lmkdir;
        }
      } else {
        return -1;
      }
    } else if (!S_ISDIR(st.st_mode)) {
      errno = ENOTDIR;
      return -1;
    } else {
      if (first) {
        return 0;
      }
      break;
    }
    first = false;
  } while (1);
Lmkdir:
  if (strlen(buf) == 0) {
    while (b_it != b_end) {
      if (*b_it == '\0') {
        *b_it = '/';
        break;
      }
      ++b_it;
    } //while
  }

  do {
    res = mkdir(buf, mode);
    if (strlen(buf) == plen) {
      break;
    }

    while (b_it != b_end) {
      if (*b_it == '\0') {
        *b_it = '/';
        break;
      }
      ++b_it;
    } //while
  } while (res != 0);

  return res;
}

int
sp_fs_mkdirs2(const char *path, mode_t mode)
{
  int ret            = -1;
  size_t l_path      = strlen(path);
  char buf[PATH_MAX] = {0};
  char *rest         = buf;
  char *token;
  int dir_fd      = -1;
  const int flags = O_PATH | O_DIRECTORY | O_CLOEXEC;
  if (l_path > PATH_MAX) {
    return -1;
  }
  strcpy(buf, path);
  /* TODO cannonize */

  if ((dir_fd = open("/", flags)) < 0) {
    if (errno == EEXIST) {
    }
    return -1;
  }

  while ((token = strtok_r(rest, "/", &rest))) {
    int child_fd = -1;
  Lretry:
    if ((child_fd = openat(dir_fd, token, flags)) < 0) {
      if (errno == ENOTDIR) {
        goto Lerr;
      } else if (errno == ENOENT) {
        errno = 0;
        if (mkdirat(dir_fd, token, mode) < 0) {
          goto Lerr;
        }
        goto Lretry;
      } else {
        goto Lerr;
      }
    }

    if (dir_fd >= 0) {
      sp_util_close(&dir_fd);
    }
    dir_fd = child_fd;
  } //while

  ret = 0;
Lerr:
  if (dir_fd >= 0) {
    sp_util_close(&dir_fd);
  }

  return ret;
}

//==============================
