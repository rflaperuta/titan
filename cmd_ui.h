/*
 * Copyright (C) 2017 Niko Rosvall <niko@byteptr.com>
 */

#ifndef __CMD_UI_H
#define __CMD_UI_H

void init_database(const char *path, int force);
bool add_new_entry();
bool edit_entry(int id);
bool remove_entry(int id);
void list_by_id(int id, int show_password);
void list_all(int show_password);
void find(const char *search, int show_password);
void show_current_db_path();
void set_use_db(const char *path);

#endif
