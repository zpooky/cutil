#include "sp_xdg.h"

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>

//==============================
int
sp_xdg_runtime_dir(sp_uri2 *path)
{
  const char *data = getenv("XDG_RUNTIME_DIR");
  if (data == NULL || strcmp(data, "") == 0) {
    char buffer[64] = {0};
    long uid        = getuid();
    sp_uri2_init0(path);
    sp_uri2_append(path, "run");
    sp_uri2_append(path, "user");
    sprintf(buffer, "%ld", uid);
    sp_uri2_append(path, buffer);
    /* sp_uri2_init(path, "/tmp"); */
    return 0;
  }

  sp_uri2_init(path, data);
  return 0;
}

int
sp_xdg_cache_dir(char *path)
{
  // read env $XDG_DATA_HOME default to $HOME/.local/share
  // $XDG_CACHE_HOME default equal to $HOME/.cache
  const char *data = getenv("XDG_DATA_HOME");
  if (data == NULL || strcmp(data, "") == 0) {
    const char *home = getenv("HOME");
    if (!home) {
      return ENOENT;
    }

    sprintf(path, "%s/.local/share", home);
    return 0;
  }

  sprintf(path, "%s", data);
  return 0;
}

//==============================
