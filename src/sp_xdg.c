#include "sp_xdg.h"

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdio.h>

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

//==============================
