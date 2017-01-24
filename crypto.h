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

#ifndef __CRYPTO_H
#define __CRYPTO_H

#define KEY_SIZE (32)  //256 bits
#define IV_SIZE (16)   //128 bits
#define SALT_SIZE (64) //512 bits

#define TITAN_MODE_DECRYPT (0)
#define TITAN_MODE_ENCRYPT (1)

typedef struct Key
{
    char data[32];
    char salt[64];

} Key_t;

bool encrypt_file(const char *passphrase, const char *path);
bool decrypt_file(const char *passphrase, const char *path);

#endif
