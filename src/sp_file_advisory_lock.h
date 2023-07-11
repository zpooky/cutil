#ifndef _SP_FILE_ADVISORY_LOCK_H
#define _SP_FILE_ADVISORY_LOCK_H

//==============================
typedef struct {
  int fd;
} sp_file_advisory_lock;

//==============================
int
sp_file_advisory_lock_init(sp_file_advisory_lock *self, const char *fpath);

int
sp_file_advisory_lock_free(sp_file_advisory_lock *self);

//==============================
int
sp_file_advisory_lock_shared_lock(sp_file_advisory_lock *self);

int
sp_file_advisory_lock_try_shared_lock(sp_file_advisory_lock *self);

int
sp_file_advisory_lock_exclusive_lock(sp_file_advisory_lock *self);

int
sp_file_advisory_try_lock_exclusive_lock(sp_file_advisory_lock *self);

int
sp_file_advisory_lock_unlock(sp_file_advisory_lock *self);

//==============================

#endif
