#ifndef _SP_FS_H
#define _SP_FS_H

//==============================
struct sp_str;

//==============================
const char *
sp_fs_basename(const char *);

const char *
sp_fs_basename_str(const struct sp_str *);

//==============================
// struct sp_str
// sp_fs_dirname_str(const struct sp_str *);

//==============================
struct sp_str
sp_fs_parent_str(const struct sp_str *);

//==============================
#endif
