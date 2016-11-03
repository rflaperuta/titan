/*
 * Copyright (C) 2016 Niko Rosvall <niko@byteptr.com>
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

#ifndef __DB_H
#define __DB_H

bool db_init_new(const char *path);
bool db_insert_entry(Entry_t *entry);
bool db_update_entry(int id, Entry_t *new_entry);
bool db_delete_entry(int id, bool *changes);
Entry_t *db_get_entry_by_id(int id);
bool db_list_all(int show_password);

#endif
