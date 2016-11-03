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

#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "entry.h"

/* Allocate and return a new entry containing data.
   Called must free the return value.
*/
Entry_t *entry_new(const char *title, const char *user,
                   const char *url, const char *password,
                   const char *notes)
{

    Entry_t *new = NULL;

    new = malloc(sizeof(struct _entry));

    if(new == NULL)
        return NULL;

    new->title = strdup(title);
    new->user = strdup(user);
    new->url = strdup(url);
    new->password = strdup(password);
    new->notes = strdup(notes);

    return new;
}

void entry_free(Entry_t *entry)
{
    if(!entry)
        return;

    free(entry->title);
    free(entry->user);
    free(entry->url);
    free(entry->password);
    free(entry->notes);

    free(entry);
}
