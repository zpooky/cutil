#include "sp_file_advisory_lock.h"

#include "sp_util.h"

#include <sys/file.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>

/*
 * # https://man7.org/linux/man-pages/man2/flock.2.html
 * On success, zero is returned.
 * On error, -1 is returned, and errno is set to indicate the error.
 * EBADF  fd is not an open file descriptor.
 * EINTR  While waiting to acquire a lock, the call was interrupted by delivery of a signal caught by a handler; see signal(7).
 * EINVAL operation is invalid.
 * ENOLCK The kernel ran out of memory for allocating lock records.
 * EWOULDBLOCK The file is locked and the LOCK_NB flag was selected.
 *
 * TODO self->fd = fd+1 so that a 0 init struct is invalid (because 0 is a valid fd)
 */

//==============================
const char *
sp_debug_sp_file_advisory_lock(const sp_file_advisory_lock *in)
{
  static char buf[1024] = {'\0'};
  if (!in)
    return "sp_file_advisory_lock(NULL)";
  snprintf(buf, sizeof(buf), "sp_file_advisory_lock{fd[%d]}", in->fd);
  return buf;
}
//==============================
int
sp_file_advisory_lock_init(sp_file_advisory_lock *self, const char *fpath)
{
  assertx(self);
  assertx(fpath);

  /* Lretry: */
  if ((self->fd = open(fpath, O_CREAT | O_EXCL) < 0)) {
#if 0
    struct flock fl = {0};
    int ret;
    int fd = -1;
    if (fcntl(fd, F_GETLK, &fl) < 0) {
    }
    /* TODO if file already exist but the owning process is dead */

    ret = close(fd);
    assertx(ret == 0);

#endif
    return self->fd;
  }

  return 0;
}

int
sp_file_advisory_lock_init1(sp_file_advisory_lock *self,
                            int dir,
                            const char *file)
{

  assertx(dir >= 0);

  if ((self->fd = openat(dir, file, O_CREAT | O_EXCL) < 0)) {
    return self->fd;
  }

  return 0;
}

int
sp_file_advisory_lock_free(sp_file_advisory_lock *self)
{
  if (self->fd >= 0) {
    int ret = close(self->fd);
    assertx(ret == 0);
    self->fd = -1;
  }

  return 0;
}

//==============================
int
sp_file_advisory_lock_shared_lock(sp_file_advisory_lock *self)
{
  assertx(self);
  assertx(self->fd >= 0);
  return flock(self->fd, LOCK_SH);
}

int
sp_file_advisory_lock_try_shared_lock(sp_file_advisory_lock *self)
{
  assertx(self);
  assertx(self->fd >= 0);
  return flock(self->fd, LOCK_SH | LOCK_NB);
}

int
sp_file_advisory_lock_exclusive_lock(sp_file_advisory_lock *self)
{
  assertx(self);
  assertx(self->fd >= 0);
  return flock(self->fd, LOCK_EX);
}

int
sp_file_advisory_try_lock_exclusive_lock(sp_file_advisory_lock *self)
{
  assertx(self);
  assertx(self->fd >= 0);
  return flock(self->fd, LOCK_EX | LOCK_NB);
}

int
sp_file_advisory_lock_unlock(sp_file_advisory_lock *self)
{
  assertx(self);
  assertx(self->fd >= 0);
  return flock(self->fd, LOCK_UN);
}

//==============================
