/*
 * Copyright (C) 2017 Niko Rosvall <niko@byteptr.com>
 */

#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include "utils.h"

/* Function checks that we have a valid path
 * in our lock file and if the database is not
 * encrypted.
 */
bool has_lock()
{
    char *path = NULL;

    path = get_lockfile_path();

    if(!path)
        return false;

    struct stat buf;

    if(stat(path, &buf) != 0)
    {
        free(path);
        return false;
    }

    //TODO: Check if the database is decrypted too
    //If not it's safe to return false so we can override it

    free(path);

    return true;
}

/* Returns the path of ~/.titan.lock file.
 * Caller must free the return value */
char *get_lockfile_path()
{
    char *home = NULL;
    char *path = NULL;

    home = getenv("HOME");

    if(!home)
        return NULL;

    /* /home/user/.titan.lock */
    path = malloc(sizeof(char) * (strlen(home) + 13));

    if(!path)
        return NULL;

    strcpy(path, home);
    strcat(path, "/.titan.lock");

    return path;
}

/* Reads and returns the path of currently decrypted
 * database. Caller must free the return value */
char *read_lock()
{
    FILE *fp = NULL;
    char *path = NULL;
    char *lockpath = NULL;
    size_t len;

    path = get_lockfile_path();

    if(!path)
        return NULL;

    fp = fopen(path, "r");

    if(!fp)
    {
        free(path);
        return NULL;
    }

    /* We only need the first line from the file */

    if(getline(&lockpath, &len, fp) < 0)
    {
        if(lockpath)
            free(lockpath);

        fclose(fp);
        free(path);

        return NULL;
    }

    fclose(fp);
    free(path);

    return lockpath;
}

void write_lock(const char *db_path)
{
    FILE *fp = NULL;
    char *path = NULL;

    path = get_lockfile_path();

    if(!path)
        return;

    fp = fopen(path, "w");

    if(!fp)
    {
        fprintf(stderr, "Error creating lock file\n");
        return;
    }

    fprintf(fp, "%s", db_path);
    fclose(fp);

    free(path);
}
