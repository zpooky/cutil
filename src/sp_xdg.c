#include "sp_xdg.h"

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

//==============================
int
sp_xdg_run_dir(sp_uri2 *path)
{
  char buffer[64] = {0};
  long uid        = getuid();
  sp_uri2_init0(path);
  sp_uri2_append(path, "run");
  sp_uri2_append(path, "user");
  sprintf(buffer, "%ld", uid);
  sp_uri2_append(path, buffer);

  return 0;
}

//==============================
