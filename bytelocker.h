////////////////////////////////////////////////////////////////////////
// COMP1521 22T2 --- Assignment 2: `bytelocker', a simple file encryptor
// <https://www.cse.unsw.edu.au/~cs1521/22T2/assignments/ass2/index.html>
//
// Written by YOUR-NAME-HERE (z5555555) on INSERT-DATE-HERE.
//
// 2022-07-22   v1.2    Team COMP1521 <cs1521 at cse.unsw.edu.au>
////////////////////////////// DO NOT CHANGE THIS FILE. //////////////////////////////
// THIS FILE IS NOT SUBMITTED AND WILL BE PROVIDED AS IS DURING TESTING AND MARKING //
////////////////////////////// DO NOT CHANGE THIS FILE. //////////////////////////////


#ifndef _BYTELOCKER_H
#define _BYTELOCKER_H

#include <stdbool.h>

#define MAX_PATH_LEN 4096
#define MAX_LINE_LEN 2048
#define MAX_NUM_FINDS 512
#define MAX_SEARCH_LENGTH 512
#define RAND_STR_LEN 16
#define CIPHER_BLOCK_SIZE 16

// Subset 0
void show_perms(char filename[MAX_PATH_LEN]);
void print_current_directory(void);
void change_directory(char dest_directory[MAX_PATH_LEN]);
void list_current_directory(void);

// Subset 1
bool test_can_encrypt(char filename[MAX_PATH_LEN]);
void simple_xor_encryption(char filename[MAX_PATH_LEN]);
void simple_xor_decryption(char filename[MAX_PATH_LEN]);

// Subset 2
void search_by_filename(char filename[MAX_SEARCH_LENGTH]);
void search_by_text(char text[MAX_SEARCH_LENGTH]);

// Subset 3
void electronic_codebook_encryption(char filename[MAX_PATH_LEN], char password[CIPHER_BLOCK_SIZE + 1]);
void electronic_codebook_decryption(char filename[MAX_PATH_LEN], char password[CIPHER_BLOCK_SIZE + 1]);

char *shift_encrypt(char *plaintext, char *password);
char *shift_decrypt(char *ciphertext, char *password);

// Subset 4
void cyclic_block_shift_encryption(char filename[MAX_PATH_LEN], char password[CIPHER_BLOCK_SIZE + 1]);
void cyclic_block_shift_decryption(char filename[MAX_PATH_LEN], char password[CIPHER_BLOCK_SIZE + 1]);


struct text_find {
    char path[MAX_PATH_LEN];
    int count;
};

#endif // _BYTELOCKER_H
