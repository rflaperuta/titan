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

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include "crypto.h"

//Our magic number that's written into the
//encrypted file. Used to determine if the file
//is encrypted.
static const int MAGIC_HEADER = 0x33497546;

//Function generates random data from /dev/urandom
//Parameter size is how much random data caller
//wants to generate. Caller must free the return value.
//Returns the data or NULL on failure.
static char *generate_random_data(int size)
{
    char *data = NULL;
    FILE *frnd = NULL;

    data = calloc(1, size);

    if(data == NULL)
    {
        fprintf(stderr, "Malloc failed\n");
        return NULL;
    }

    frnd = fopen("/dev/urandom", "r");

    if(!frnd)
    {
        fprintf(stderr, "Cannot open /dev/urandom\n");
        free(data);
        return NULL;
    }

    fread(data, size, 1, frnd);
    fclose(frnd);

    return data;
}

static Key_t generate_key(const char *passphrase, bool *ok)
{
    char *salt = NULL;
    int iterations = 25000;
    Key_t key;
    int success;
    char *resultbytes[KEY_SIZE];

    salt = generate_random_data(SALT_SIZE);

    if(!salt)
    {
        *ok = false;
        return key;
    }

    success = PKCS5_PBKDF2_HMAC(passphrase, strlen(passphrase), (unsigned char*)salt,
                                strlen(salt), iterations, EVP_sha256(),
                                KEY_SIZE, (unsigned char*)resultbytes);

    if(success == 0)
    {
        free(salt);
        *ok = false;
        return key;
    }

    memmove(key.data, resultbytes, KEY_SIZE);
    memmove(key.salt, salt, SALT_SIZE);

    free(salt);
    *ok = true;

    return key;
}

static bool encrypt(unsigned char *plain, int plain_len, unsigned char *add,
             int add_len, unsigned char *key, unsigned char *iv,
             unsigned char *cipher /*out*/, unsigned char *tag /*out*/)
{



    return true;
}

static bool decrypt()
{

    return true;
}

bool encrypt_file(const char *passphrase, const char *path)
{

    bool ok;
    char *iv = NULL;
    Key_t key = generate_key(passphrase, &ok);

    if(!ok)
    {
        fprintf(stderr, "Key derivation failed.\n");
        return false;
    }

    iv = generate_random_data(IV_SIZE);

    if(!iv)
    {
        fprintf(stderr, "Initialization vector generation failed.\n");
        return false;
    }

    //read data from path
    //encrypt data
    //write magic header,iv, salt and additional data and the tag, to the beginning of the file
    //write ciphered data to the file

    return true;
}

bool decrypt_file(const char *passphrase, const char *path)
{

    return true;
}
