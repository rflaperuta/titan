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

#ifndef __ENTRY_H
#define __ENTRY_H

typedef struct _entry
{
    int id;
    char *title;
    char *user;
    char *url;
    char *password;
    char *notes;

} Entry_t;


Entry_t *entry_new(const char *title, const char *user, const char *url,
                   const char *password, const char *notes);

void entry_free(Entry_t *entry);

#endif
