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

#define _XOPEN_SOURCE 700

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

//Generate key from passphrase. If oldsalt is NULL, new salt is created.
//ok is set to true on success, false on failure
static Key_t generate_key(const char *passphrase, char *old_salt,
                          bool *ok)
{
    char *salt = NULL;
    int iterations = 25000;
    Key_t key;
    int success;
    char *resultbytes[KEY_SIZE];

    if(old_salt == NULL)
        salt = generate_random_data(SALT_SIZE);
    else
        salt = strdup(old_salt);

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

//Function appends ext to the orig string.
//Returns new string on success, NULL on failure.
//Caller must free the return value.
static char *get_output_filename(const char *orig, const char *ext)
{
    char *path = NULL;
    size_t len;
    len = strlen(orig) + strlen(ext) + 1;
    path = malloc(len * sizeof(char));

    if(!path)
    {
        fprintf(stderr, "Malloc failed\n");
        return NULL;
    }

    strcpy(path, orig);
    strcat(path, ext);

    return path;
}

static bool is_file_encrypted(const char *path)
{
    FILE *fp = NULL;
    int data;

    fp = fopen(path, "r");

    if(!fp)
    {
        fprintf(stderr, "Failed to open file.\n");
        return false;
    }

    fseek(fp, 0, SEEK_END);
    int len = ftell(fp);
    int offset = sizeof(int) + IV_SIZE + SALT_SIZE;
    rewind(fp);

    fseek(fp, len - offset, SEEK_CUR);

    //Read our magic header
    fread((void*)&data, sizeof(MAGIC_HEADER), 1, fp);
    fclose(fp);

    if(data != MAGIC_HEADER)
        return false;

    return true;
}

static bool encrypt_decrypt(unsigned char *data_in, int data_in_len, FILE *out, unsigned char *key,
                    unsigned char *iv, int is_encrypt)
{
    EVP_CIPHER_CTX ctx;
    unsigned char *out_buffer;
    int output_len = 0;
    int output_len_final = 0;

    if(EVP_CipherInit(&ctx, EVP_aes_256_cbc(), key, iv, is_encrypt) != 1)
    {

    }

    out_buffer = malloc(data_in_len * 2);

    if(EVP_CipherUpdate(&ctx, out_buffer, &output_len, data_in, data_in_len) != 1)
    {
        printf("bar\n");
    }

    EVP_CipherFinal(&ctx, out_buffer + output_len, &output_len_final);
    fwrite(out_buffer, sizeof(unsigned char), output_len + output_len_final, out);

    free(out_buffer);

    return true;
}

bool encrypt_file(const char *passphrase, const char *path)
{
    bool ok;
    char *iv = NULL;
    FILE *plain = NULL;
    FILE *cipher = NULL;
    char *output_filename = NULL;
    char *plain_data = NULL;

    if(is_file_encrypted(path))
    {
        fprintf(stderr, "File is already encrypted.\n");
        return false;
    }

    Key_t key = generate_key(passphrase, NULL, &ok);

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

    plain = fopen(path, "r");

    if(!plain)
    {
        fprintf(stderr, "Unable to open %s\n", path);
        free(iv);
        return false;
    }

    fseek(plain, 0, SEEK_END);
    int plain_len = ftell(plain);
    fseek(plain, 0, SEEK_SET);

    plain_data = malloc(plain_len * sizeof(char));

    if(!plain_data)
    {
        fprintf(stderr, "Unable to allocate memory.\n");
        fclose(plain);
        free(iv);
        return false;
    }

    fread(plain_data, sizeof(char), plain_len, plain);
    fclose(plain);

    output_filename = get_output_filename(path, ".titan");

    if(!output_filename)
    {
        fprintf(stderr, "Unable to create output filename.\n");
        free(iv);
        free(plain_data);
        return false;
    }

    cipher = fopen(output_filename, "w");

    if(!cipher)
    {
        fprintf(stderr, "Unable to open %s for writing.\n", output_filename);
        free(iv);
        free(output_filename);
        free(plain_data);
        return false;
    }

    encrypt_decrypt((unsigned char*)plain_data, plain_len, cipher, (unsigned char *)key.data, (unsigned char *)iv,
                    TITAN_MODE_ENCRYPT);

    //write iv etc. into the end of the file
    fwrite((void*)&MAGIC_HEADER, sizeof(MAGIC_HEADER), 1, cipher);
    fwrite(iv, 1, IV_SIZE, cipher);
    fwrite(key.salt, 1, SALT_SIZE, cipher);

    fclose(cipher);
    free(iv);
    free(plain_data);

    //Finally remove the plain file
    if(remove(path) != 0)
    {
        fprintf(stderr, "WARNING: Error deleting plain file %s.", path);
    }

    //And rename our ciphered file back to the original name
    rename(output_filename, path);
    free(output_filename);

    return true;
}

bool decrypt_file(const char *passphrase, const char *path)
{
    bool ok;
    char *iv = NULL;
    char *salt = NULL;
    FILE *plain = NULL;
    FILE *cipher = NULL;
    char *output_filename = NULL;
    char *cipher_data = NULL;

    if(!is_file_encrypted(path))
    {
        fprintf(stderr, "File is already decrypted or malformed.\n");
        return false;
    }

    iv = malloc(IV_SIZE);

    if(!iv)
    {
        fprintf(stderr, "Malloc failed.\n");
        return false;
    }

    salt = malloc(SALT_SIZE);

    if(!salt)
    {
        fprintf(stderr, "Unable to allocate salt memory.\n");
        free(iv);
        return false;
    }

    cipher = fopen(path, "r");

    if(!cipher)
    {
        fprintf(stderr, "Unable to open %s for reading.\n", path);
        free(iv);
        free(salt);
        return false;
    }

    fseek(cipher, 0, SEEK_END);
    int len = ftell(cipher);
    int offset = len - (sizeof(int) + IV_SIZE + SALT_SIZE);

    rewind(cipher);

    fseek(cipher, offset, SEEK_CUR);
    //Skip the magic header
    fseek(cipher, sizeof(int), SEEK_CUR);
    //iv, salt
    fread(iv, IV_SIZE, 1, cipher);
    fread(salt, SALT_SIZE, 1, cipher);

    rewind(cipher);

    //read all data, skip header, salt, iv
    cipher_data = malloc(offset * sizeof(char));

    if(!cipher_data)
    {
        fprintf(stderr, "Unable to allocate memory.\n");
        free(iv);
        free(salt);
        fclose(cipher);

        return false;
    }

    fread(cipher_data, sizeof(char), offset, cipher);
    fclose(cipher);

    Key_t key = generate_key(passphrase, salt, &ok);

    if(!ok)
    {
        fprintf(stderr, "Key derivation failed.\n");
        free(iv);
        free(salt);
        free(cipher_data);
        return false;
    }

    output_filename = get_output_filename(path, ".plain");

    if(!output_filename)
    {
        fprintf(stderr, "Unable to create output filename.\n");
        free(iv);
        free(salt);
        free(cipher_data);
        return false;
    }

    plain = fopen(output_filename, "w");

    if(!plain)
    {
        fprintf(stderr, "Unable to open %s for writing.\n", output_filename);
        free(iv);
        free(salt);
        free(output_filename);
        free(cipher_data);
        return false;
    }

    encrypt_decrypt((unsigned char*)cipher_data, offset, plain, (unsigned char *)key.data,
                    (unsigned char *)iv, TITAN_MODE_DECRYPT);

    //Finally remove the cipher file
    if(remove(path) != 0)
    {
        fprintf(stderr, "WARNING: Error deleting file %s.", path);
    }

    //And rename our ciphered file back to the original name
    rename(output_filename, path);
    free(output_filename);

    free(iv);
    free(salt);
    fclose(plain);
    free(cipher_data);

    return true;
}
