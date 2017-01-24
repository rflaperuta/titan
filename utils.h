/*
 * Copyright (C) 2017 Niko Rosvall <niko@byteptr.com>
 */


#ifndef __UTILS_H
#define __UTILS_H

char *get_lockfile_path();
void write_lock(const char *db_path);
char *read_lock();
bool has_lock();


#endif
