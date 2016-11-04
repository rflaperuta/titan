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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include "cmd_ui.h"
#include "entry.h"
#include "db.h"
#include "utils.h"
#include "pwd-gen.h"

static int show_password = 0;

static void version()
{
    printf("Titan version 0.9\n");
}

static void usage()
{
#define HELP "\
SYNOPSIS\n\
\n\
    titan [flags] [options]\n\
\n\
OPTIONS\n\
\n\
    -i --init         <path>         Initialize new database\n\
    -e --encrypt                     Encrypt current database\n\
    -d --decrypt      <path>         Decrypt database\n\
    -a --add                         Add new entry\n\
    -r --remove       <id>           Remove entry pointed by id\n\
    -f --find         <search>       Search entries\n\
    -c --edit         <id>           Edit entry pointed by id\n\
    -l --list-entry   <id>           List entry pointed by id\n\
    -A --list-all                    List all entries\n\
    -h --help                        Show short help and exit. This page\n\
    -g --gen-password <length>       Generate password\n\
    -V --version                     Show version number of program\n\
\n\
FLAGS\n\
\n\
    --show-password                  Show passwords in listings\n\
\n\
For more information and examples see man titan(1).\n\
\n\
AUTHORS\n\
    Copyright (C) 2016 Niko Rosvall <niko@byteptr.com>\n\
\n\
    Released under license GPL-3+. For more information, see\n\
    http://www.gnu.org/licenses\n\
"
    printf(HELP);
}

int main(int argc, char *argv[])
{
    int c;

    while(true)
    {
        static struct option long_options[] =
        {
            {"init",                  required_argument, 0, 'i'},
            {"decrypt",               required_argument, 0, 'd'},
            {"encrypt",               no_argument,       0, 'e'},
            {"add",                   no_argument,       0, 'a'},
            {"remove",                required_argument, 0, 'r'},
            {"find",                  required_argument, 0, 'f'},
            {"edit",                  required_argument, 0, 'c'},
            {"list-entry",            required_argument, 0, 'l'},
            {"list-all",              no_argument,       0, 'A'},
            {"help",                  no_argument,       0, 'h'},
            {"version",               no_argument,       0, 'V'},
            {"gen-password",          required_argument, 0, 'g'},
            {"show-password",         no_argument,       &show_password, 1},
            {0, 0, 0, 0}
        };

        int option_index = 0;

        c = getopt_long(argc, argv, "i:d:ear:f:c:l:AhVg:", long_options, &option_index);

        if(c == -1)
            break;

        switch(c)
        {
        case 0:
            /* Handle flags here automatically */
            break;
        case 'i':
            init_database(optarg);
            break;
        case 'd': //decrypt
            break;
        case 'e': //encrypt
            break;
        case 'a':
            add_new_entry();
            break;
        case 'h':
            usage();
            break;
        case 'r':
            remove_entry(atoi(optarg));
            break;
        case 'f':
            find(optarg, show_password);
            break;
        case 'c':
            edit_entry(atoi(optarg));
            break;
        case 'l':
            list_by_id(atoi(optarg), show_password);
            break;
        case 'A':
            list_all(show_password);
            break;
        case 'V':
            version();
            break;
        case 'g':
            generate_password(atoi(optarg));
            break;
        case '?':
            break;
        default:
            usage();
            return 0;
        }
    }

    return 0;
}
