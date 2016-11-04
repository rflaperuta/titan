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
#include <stdbool.h>
#include <string.h>
#include <termios.h>

#include "cmd_ui.h"
#include "entry.h"
#include "db.h"
#include "utils.h"

extern int fileno(FILE *stream);

/*Removes new line character from a string.*/
static void
strip_newline_str(char *str)
{

    char *i = str;
    char *j = str;

    while (*j != '\0')
    {
        *i = *j++;

        if(*i != '\n')
            i++;
    }

    *i = '\0';
}

/*Turns echo of from the terminal and asks for a passphrase.
 *Usually stream is stdin. Returns length of the passphrase,
 *passphrase is stored to lineptr. Lineptr must be allocated beforehand.
 */
static size_t
my_getpass(char *prompt, char **lineptr, size_t *n, FILE *stream)
{
    struct termios old, new;
    int nread;

    /*Turn terminal echoing off.*/
    if(tcgetattr(fileno(stream), &old) != 0)
        return -1;

    new = old;
    new.c_lflag &= ~ECHO;

    if(tcsetattr(fileno(stream), TCSAFLUSH, &new) != 0)
        return -1;

    if(prompt)
        printf("%s", prompt);

    /*Read the password.*/
    nread = getline(lineptr, n, stream);

    if(nread >= 1 && (*lineptr)[nread - 1] == '\n')
    {
        (*lineptr)[nread - 1] = 0;
        nread--;
    }

    printf("\n");

    /*Restore terminal echo.*/
    tcsetattr(fileno(stream), TCSAFLUSH, &old);

    return nread;
}

void init_database(const char *path)
{
    if(!has_lock())
    {
        if(db_init_new(path))
            write_lock(path);
    }
    else
    {
        fprintf(stderr, "Existing database is decrypted. "
                "Encrypt it before creating a new one.\n");
    }
}

/* Interactively adds a new entry to the database */
bool add_new_entry()
{
    if(!has_lock())
    {
        fprintf(stderr, "No decrypted database found.\n");
        return false;
    }

    char title[1024] = {0};
    char user[1024] = {0};
    char url[1024] = {0};
    char notes[1024] = {0};
    size_t pwdlen = 1024;
    char pass[pwdlen];
    char *ptr = pass;

    fprintf(stdout, "Title: ");
    fgets(title, 1024, stdin);
    fprintf(stdout, "Username: ");
    fgets(user, 1024, stdin);
    fprintf(stdout, "Url: ");
    fgets(url, 1024, stdin);
    fprintf(stdout, "Notes: ");
    fgets(notes, 1024, stdin);

    my_getpass("Password: ", &ptr, &pwdlen, stdin);

    strip_newline_str(title);
    strip_newline_str(user);
    strip_newline_str(url);
    strip_newline_str(notes);

    Entry_t *entry = entry_new(title, user, url, pass,
                               notes);

    if(!entry)
        return false;

    if(!db_insert_entry(entry))
    {
        fprintf(stderr, "Failed to add a new entry.\n");
        return false;
    }

    entry_free(entry);

    return true;
}

bool edit_entry(int id)
{
    if(!has_lock())
    {
        fprintf(stderr, "No decrypted database found.\n");
        return false;
    }

    Entry_t *entry = db_get_entry_by_id(id);

    if(!entry)
        return false;

    if(entry->id == -1)
    {
        printf("Nothing found.\n");
        free(entry);
        return false;
    }

    char title[1024] = {0};
    char user[1024] = {0};
    char url[1024] = {0};
    char notes[1024] = {0};
    size_t pwdlen = 1024;
    char pass[pwdlen];
    char *ptr = pass;

    fprintf(stdout, "Current title %s\n", entry->title);
    fprintf(stdout, "New title: ");
    fgets(title, 1024, stdin);
    fprintf(stdout, "Current username %s\n", entry->user);
    fprintf(stdout, "New username: ");
    fgets(user, 1024, stdin);
    fprintf(stdout, "Current url %s\n", entry->url);
    fprintf(stdout, "New url: ");
    fgets(url, 1024, stdin);
    fprintf(stdout, "Current notes %s\n", entry->notes);
    fprintf(stdout, "New note: ");
    fgets(notes, 1024, stdin);
    fprintf(stdout, "Current password %s\n", entry->password);
    my_getpass("New password: ", &ptr, &pwdlen, stdin);

    strip_newline_str(title);
    strip_newline_str(user);
    strip_newline_str(url);
    strip_newline_str(notes);

    if(title[0] != '\0')
	entry->title = strdup(title);
    if(user[0] != '\0')
	entry->user = strdup(user);
    if(url[0] != '\0')
	entry->url = strdup(url);
    if(notes[0] != '\0')
	entry->notes = strdup(notes);
    if(pass[0] != '\0')
	entry->password = strdup(pass);

    db_update_entry(entry->id, entry);

    entry_free(entry);

    return true;
}

bool remove_entry(int id)
{
    if(!has_lock())
    {
        fprintf(stderr, "No decrypted database found.\n");
        return false;
    }

    bool changes = false;

    if(db_delete_entry(id, &changes))
    {
        if(changes == true)
            fprintf(stdout, "Entry was deleted from the database.\n");
        else
            fprintf(stdout, "No entry with id %d was found.\n", id);

        return true;
    }

    return false;
}

void list_by_id(int id, int show_password)
{
    if(!has_lock())
    {
        fprintf(stderr, "No decrypted database found.\n");
        return;
    }

    Entry_t *entry = db_get_entry_by_id(id);

    if(!entry)
        return;


    if(entry->id == -1)
    {
        printf("Nothing found with id %d.\n", id);
        free(entry);
        return;
    }

    fprintf(stdout, "=====================================================================\n");
    fprintf(stdout, "Title: %s\n", entry->title);
    fprintf(stdout, "User:  %s\n", entry->user);
    fprintf(stdout, "Url:   %s\n", entry->url);
    fprintf(stdout, "Notes: %s\n", entry->notes);

    if(show_password == 1)
        fprintf(stdout, "Password: %s\n", entry->password);
    else
        fprintf(stdout, "Password: **********\n");

    fprintf(stdout, "=====================================================================\n");
    entry_free(entry);
}

/* Loop through all entries in the database.
 * At the moment printing to stdout is handled by sqlite callback.
 * While it's ok for the command line, but if we ever need GUI
 * then this needs to be designed better using a linked list etc.
 */
void list_all(int show_password)
{
    if(!has_lock())
    {
        fprintf(stderr, "No decrypted database found.\n");
        return;
    }

    db_list_all(show_password);
}

/* Uses sqlite "like" query and prints results to stdout.
 * This is ok for the command line version of Titan. However
 * better design is needed _if_ GUI version will be developed.
 */
void find(const char *search, int show_password)
{
    if(!has_lock())
    {
        fprintf(stderr, "No decrypted database found.\n");
        return;
    }

    db_find(search, show_password);
}
