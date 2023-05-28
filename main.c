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

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include "bytelocker.h"

void choose_search(void);
void choose_encrypt(void);
void choose_decrypt(void);

void do_show_perms(void);
void do_print_current_directory(void);
void do_change_directory(void);
void do_list_current_directory(void);
void do_test_can_encrypt(void);
void do_search_by_filename(void);
void do_search_by_text(void);
void do_simple_xor_encryption(void);
void do_simple_xor_decryption(void);
void do_electronic_codebook_encryption(void);
void do_electronic_codebook_decryption(void);

/////////////////////////////////////////////////////////////////////////////
// This is just to suppress warnings about unused constants.
extern const char *const MSG_ERROR_FILE_STAT;
extern const char *const MSG_ERROR_FILE_OPEN;
extern const char *const MSG_ERROR_CHANGE_DIR;
extern const char *const MSG_ERROR_DIRECTORY;
extern const char *const MSG_ERROR_READ;
extern const char *const MSG_ERROR_WRITE;
extern const char *const MSG_ERROR_SEARCH;
extern const char *const MSG_ERROR_RESERVED;

/////////////////////////////////////////////////////////////////////////////

int main(void) {
    char cwd[MAX_PATH_LEN];
    getcwd(cwd, MAX_PATH_LEN);

    bool run = true;

    static const char *option_prompt =
        "(h)elp - show this prompt\n"
        "(v)iew file permissions\n"
        "(p)rint current directory\n"
        "(l)ist current directory\n"
        "(c)hange directory\n"
        "(t)est if a file can be encrypted\n"
        "(s)earch\n"
        "(e)ncrypt file\n"
        "(d)ecrypt file\n"
        "(r)eset program state\n"
        "(q)uit\n";

    static const char *command_prompt = "Command: ";

    printf("%s", option_prompt);

    while (run) {
        char input = 0;

        printf("%s", command_prompt);
        fflush(stdout);

        if (scanf(" %c", &input) == EOF) {
            clearerr(stdin);
        }

        switch (input) {
            case 'h':
                printf("%s", option_prompt);
                break;

            case 'v':
                do_show_perms();
                break;

            case 'p':
                do_print_current_directory();
                break;

            case 'l':
                do_list_current_directory();
                break;

            case 'c':
                do_change_directory();
                break;

            case 't':
                do_test_can_encrypt();
                break;

            case 's':
                choose_search();
                break;

            case 'e':
                choose_encrypt();
                break;

            case 'd':
                choose_decrypt();
                break;

            case 'r':
                chdir(cwd);
                break;

            case 'q':
                run = false;
                printf("See you later!\n");
                break;

            default:
                printf("Please enter a valid option\n");
                break;
        }

    }

    return 0;
}

void do_show_perms(void) {
    char filename[MAX_PATH_LEN];
    fprintf(stdout, "Which file would you like to scan? ");
    scanf("%s", filename);
    show_perms(filename);
}

void do_print_current_directory(void) {
    print_current_directory();
}

void do_change_directory(void) {
    char dest_directory[MAX_PATH_LEN];
    printf("Which directory do you want to move to? ");
    scanf("%s", dest_directory);
    change_directory(dest_directory);
}

void do_list_current_directory(void) {
    list_current_directory();
}

void do_test_can_encrypt(void) {
    char filename[MAX_PATH_LEN];
    printf("Which file would you like to test? ");
    scanf("%s", filename);
    if (test_can_encrypt(filename)) {
        printf("%s can be encrypted!\n", filename);
    }
}

void do_search_by_filename(void) {
    char search[MAX_SEARCH_LENGTH];
    printf("What would you like to search for? ");
    scanf(" %s", search);
    search_by_filename(search);
}

void do_search_by_text(void) {
    char search[MAX_SEARCH_LENGTH];
    printf("What would you like to search for? ");
    scanf(" %s", search);
    search_by_text(search);
}

void do_simple_xor_encryption(void) {
    char filename[MAX_PATH_LEN] = { 0 };
    printf("What file would you like to encrypt? ");
    if (scanf("%s%*c", filename) == EOF) {
        clearerr(stdin);
        printf("\n");
        return;
    }
    simple_xor_encryption(filename);
}

void do_simple_xor_decryption(void) {
    char filename[MAX_PATH_LEN] = { 0 };
    printf("What file would you like to decrypt? ");
    if (scanf("%s%*c", filename) == EOF) {
        clearerr(stdin);
        printf("\n");
        return;
    }
    simple_xor_decryption(filename);
}

void do_electronic_codebook_encryption(void) {
    char filename[MAX_PATH_LEN] = { 0 };
    printf("What file would you like to encrypt? ");
    scanf("%s", filename);

    char password[CIPHER_BLOCK_SIZE + 1] = { 0 };
    printf("Enter your %d character password: ", CIPHER_BLOCK_SIZE);
    if (scanf("%16s%*c", password) == EOF) {
        clearerr(stdin);
        printf("\n");
        return;
    }
    /*if (strlen(password) != CIPHER_BLOCK_SIZE) {
        fprintf(stderr, "Your password must be %d characters.\n", CIPHER_BLOCK_SIZE);
        return;
    }*/
    electronic_codebook_encryption(filename, password);
}

void do_electronic_codebook_decryption(void) {
    char filename[MAX_PATH_LEN] = { 0 };
    printf("What file would you like to decrypt? ");
    scanf("%s", filename);

    char password[CIPHER_BLOCK_SIZE + 1] = { 0 };
    printf("Enter your %d character password: ", CIPHER_BLOCK_SIZE);
    if (scanf("%16s%*c", password) == EOF) {
        clearerr(stdin);
        printf("\n");
        return;
    }
    /*if (strlen(password) != CIPHER_BLOCK_SIZE) {
        fprintf(stderr, "Your password must be %d characters.\n", CIPHER_BLOCK_SIZE);
        return;
    }*/
    electronic_codebook_decryption(filename, password);
}

void do_cyclic_block_shift_encryption(void) {
    char filename[MAX_PATH_LEN] = { 0 };
    printf("What file would you like to encrypt? ");
    scanf("%s", filename);

    char password[CIPHER_BLOCK_SIZE + 1] = { 0 };
    printf("Enter your %d character password: ", CIPHER_BLOCK_SIZE);
    if (scanf("%16s%*c", password) == EOF) {
        clearerr(stdin);
        printf("\n");
        return;
    }

    cyclic_block_shift_encryption(filename, password);
}

void do_cyclic_block_shift_decryption(void) {
    char filename[MAX_PATH_LEN] = { 0 };
    printf("What file would you like to decrypt? ");
    scanf("%s", filename);

    char password[CIPHER_BLOCK_SIZE + 1] = { 0 };
    printf("Enter your %d character password: ", CIPHER_BLOCK_SIZE);
    if (scanf("%16s%*c", password) == EOF) {
        clearerr(stdin);
        printf("\n");
        return;
    }
    /*if (strlen(password) != CIPHER_BLOCK_SIZE) {
        fprintf(stderr, "Your password must be %d characters.\n", CIPHER_BLOCK_SIZE);
        return;
    }*/
    cyclic_block_shift_decryption(filename, password);
}

void choose_search(void) {
    char mode;

    printf("How would you like to search?\n");
    printf("(n)ame\n");
    printf("(t)ext content\n");
    printf("mode: ");

    if (scanf(" %c%*c", &mode) == EOF) {
        clearerr(stdin);
        printf("\n");
        return;
    }

    switch (mode) {
        case 'n':
            do_search_by_filename();
            break;

        case 't':
            do_search_by_text();
            break;

        default:
            printf("Invalid mode\n");
            break;
    }
}

void choose_encrypt(void) {
    char mode;

    printf("Which mode of encryption would you like to use?\n");
    printf("(x)or mode\n");
    printf("(e)lectronic codebook cipher mode\n");
    printf("(c)yclic block cipher mode\n");
    printf("mode: ");

    if (scanf(" %c%*c", &mode) == EOF) {
        clearerr(stdin);
        printf("\n");
        return;
    }

    switch (mode) {
        case 'x':
            do_simple_xor_encryption();
            break;

        case 'e':
            do_electronic_codebook_encryption();
            break;

        case 'c':
            do_cyclic_block_shift_encryption();
            break;

        default:
            printf("Invalid mode\n");
            break;
        }
}

void choose_decrypt(void) {
    char mode;

    printf("Which mode of decryption would you like to use?\n");
    printf("(x)or mode\n");
    printf("(e)lectronic codebook cipher mode\n");
    printf("(c)yclic block cipher mode\n");
    printf("mode: ");

    if (scanf(" %c", &mode) == EOF) {
        clearerr(stdin);
        printf("\n");
        return;
    }

    switch (mode) {
        case 'x':
            do_simple_xor_decryption();
            break;

        case 'e':
            do_electronic_codebook_decryption();
            break;

        case 'c':
            do_cyclic_block_shift_decryption();
            break;

        default:
            printf("Invalid mode\n");
            break;
        }
}
