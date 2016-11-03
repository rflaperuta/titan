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

static int show_password = 0;

static void usage()
{
    printf("Usage: foo\n");

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
            {"generate-password",     required_argument, 0, 'g'},
            {"show-password",         no_argument,       &show_password, 1},
            {0, 0, 0, 0}
        };

        int option_index = 0;

        c = getopt_long(argc, argv, "i:d:ear:f:c:l:Ag:", long_options, &option_index);

        if(c == -1)
            break;

        switch(c)
        {
        case 0:
            /* Handle flags here automatically */
            break;
        case 'i':
            if(!has_lock())
            {
                if(db_init_new(optarg))
                    write_lock(optarg);
            }
            else
            {
                fprintf(stderr,
                        "Existing database is decrypted. Encrypt it before creating a new one.\n");
            }
            break;
        case 'd': //decrypt
            break;
        case 'e': //encrypt
            break;
        case 'a':
            add_new_entry();
            break;
        case 'r':
            remove_entry(atoi(optarg));
            break;
        case 'f'://find
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
        case 'g':
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
