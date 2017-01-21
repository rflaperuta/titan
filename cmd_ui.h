/*
 * Copyright (C) 2017 Niko Rosvall <niko@byteptr.com>
 *
 * This file is part of Titan.
 *
 * Titan is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Titan is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Titan. If not, see <http://www.gnu.org/licenses/>.
 *
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
