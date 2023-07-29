#include "sp_test_url.h"

#include <assert.h>
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>

#include <sp_uri.h>

int
sp_test_url(void)
{
  sp_uri2 uri = {0};
  {
    char cwd[PATH_MAX] = {0};
    getcwd(cwd, sizeof(cwd));
    sp_uri2_init(&uri, "./");
    sp_uri2_normalize(&uri);
    assert(strcmp(cwd, sp_uri2_path(&uri)) == 0);
  }

  {
    char cwd[PATH_MAX] = {0};
    getcwd(cwd, sizeof(cwd));
    sp_uri2_init(&uri, ".");
    sp_uri2_normalize(&uri);
    assert(strcmp(cwd, sp_uri2_path(&uri)) == 0);
  }

  {
    char cwd[PATH_MAX] = {0};
    getcwd(cwd, sizeof(cwd));
    sp_uri2_init(&uri, "./wasd");
    sp_uri2_normalize(&uri);
    strcat(cwd, "/wasd");
    assert(strcmp(cwd, sp_uri2_path(&uri)) == 0);
  }
  {
    char cwd[PATH_MAX] = {0};
    getcwd(cwd, sizeof(cwd));
    sp_uri2_init(&uri, "./wasd/..");
    sp_uri2_normalize(&uri);
    assert(strcmp(cwd, sp_uri2_path(&uri)) == 0);
  }
  {
    char cwd[PATH_MAX] = {0};
    getcwd(cwd, sizeof(cwd));
    sp_uri2_init(&uri, "./wasd/asd/../..");
    sp_uri2_normalize(&uri);
    assert(strcmp(cwd, sp_uri2_path(&uri)) == 0);
  }
#if 0
  /* TODO */
  {
    char cwd[PATH_MAX] = {0};
    getcwd(cwd, sizeof(cwd));
    sp_uri2_init(&uri, "../");
    sp_uri2_normalize(&uri);
    assert(strcmp(cwd, sp_uri2_path(&uri)) == 0);
  }
  {
    char cwd[PATH_MAX] = {0};
    getcwd(cwd, sizeof(cwd));
    sp_uri2_init(&uri, "..");
    sp_uri2_normalize(&uri);
    assert(strcmp(cwd, sp_uri2_path(&uri)) == 0);
  }
  {
    char cwd[PATH_MAX] = {0};
    getcwd(cwd, sizeof(cwd));
    sp_uri2_init(&uri, "../wasd");
    sp_uri2_normalize(&uri);
    strcat(cwd, "/wasd");
    assert(strcmp(cwd, sp_uri2_path(&uri)) == 0);
  }
  {
    char cwd[PATH_MAX] = {0};
    getcwd(cwd, sizeof(cwd));
    sp_uri2_init(&uri, "../wasd/..");
    sp_uri2_normalize(&uri);
    assert(strcmp(cwd, sp_uri2_path(&uri)) == 0);
  }
  {
    char cwd[PATH_MAX] = {0};
    getcwd(cwd, sizeof(cwd));
    sp_uri2_init(&uri, "../wasd/asd/../..");
    sp_uri2_normalize(&uri);
    assert(strcmp(cwd, sp_uri2_path(&uri)) == 0);
  }
#endif
  return 0;
}
