////////////////////////////////////////////////////////////////////////
// COMP1521 22T2 --- Assignment 2: `bytelocker', a simple file encryptor
// <https://www.cse.unsw.edu.au/~cs1521/22T2/assignments/ass2/index.html>
//
// Written by YOUR-NAME-HERE (z5555555) on INSERT-DATE-HERE.
//
// 2022-07-22   v1.2    Team COMP1521 <cs1521 at cse.unsw.edu.au>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h> //unix library
#include <dirent.h>
#include "bytelocker.h"


char *generate_random_string(int seed);
void sort_by_count(struct text_find *files, size_t n);
void sort_by_name(char *filenames[], size_t num_files);


//defining helping function
//count the number of occurences 
int countString(char *mainstring, char *searchstring);
//keep the information about the file names 
struct files
    {
    int count;
    char* path[40];
    char* name[40];
};
//List all the directories, and sub directories by transversing 
void listdir(char *name,char *con, struct files *f1,bool d_flag);

// Some provided strings which you may find useful. Do not modify.
const char *const MSG_ERROR_FILE_STAT  = "Could not stat file\n";
const char *const MSG_ERROR_FILE_OPEN  = "Could not open file\n";
const char *const MSG_ERROR_CHANGE_DIR = "Could not change directory\n";
const char *const MSG_ERROR_DIRECTORY  =
    "%s cannot be encrypted: bytelocker does not support encrypting directories.\n";
const char *const MSG_ERROR_READ       =
    "%s cannot be encrypted: group does not have permission to read this file.\n";
const char *const MSG_ERROR_WRITE      =
    "%s cannot be encrypted: group does not have permission to write here.\n";
const char *const MSG_ERROR_SEARCH     = "Please enter a search string.\n";
const char *const MSG_ERROR_RESERVED   =
    "'.' and '..' are reserved filenames, please search for something else.\n";



//////////////////////////////////////////////
//                                          //
//              SUBSET 0                    //
//                                          //
//////////////////////////////////////////////

//
//  Read the file permissions of the current directory and print them to stdout.
//
void show_perms(char filename[MAX_PATH_LEN]) {
  //fp is a file pointer we will use to open the cmd file
  FILE *fp; 
  char command[100]="ls -la "; 
  strcat(command,filename); //ls -l <filename> return the permissions of the file
  char ret[12];
  fp = popen(command, "r");
  
    //in case of error, print the error and return
    if (fgets(ret, sizeof(ret), fp)==NULL) {
    printf("%s",MSG_ERROR_FILE_STAT);
    return;
    }  

    if (ret[0]!='d' && ret[0]!='-') //"d > directories , - > files"
        {fgets(ret, sizeof(ret), fp); }
    printf("%s: %s\n",filename, ret);  //printing the permissions  directory/file name
    pclose(fp);
}

//
//  Prints current working directory to stdout.
//
void print_current_directory(void) {
    FILE *fp;
    char ret[100];
    fp = popen("pwd", "r"); //current directory
  
    //in case of error, print the error and return
    if (fp == NULL) {
    printf("Failed to run command\n" );
    exit(1);
    }

    //read the file to read the current current directory.
    fgets(ret, sizeof(ret), fp);
    printf("Current directory is: %s",ret);
    pclose(fp);
}

//
//  Changes directory to the given path.
//
void change_directory(char dest_directory[MAX_PATH_LEN]) {
    //if user input is ~ we move to /etc folder according to the instructions
    if(dest_directory[0]=='~') //exception
    {
        //printing the output 
        printf("Moving to %s\n","/etc");
        chdir("/etc");
    }
    else{  
    printf("Moving to %s\n",dest_directory);
    chdir(dest_directory); //move to the given directory
}
}

//
//  Lists the contents of the current directory to stdout.
//
void list_current_directory(void) {
    FILE *fp;
    char ret[100];
    fp = popen("ls -la", "r"); //get list of all files and directories and permissions
    fgets(ret,sizeof(ret),fp);//get a string
    
    //sentinal loop to read whole file.
    while (fgets(ret,sizeof(ret),fp)!=NULL) 
    {
        int c=0;
        char* word=strtok(ret," \n"); //slice string with delimater of " "
        while(word!=NULL)
        {
            if(c==0){printf("%s\t",word);} //permission
            if(c==8){printf("%s",word);} //file name
            c=(c+1)%9; //rotated value from 0-8
            word=strtok(NULL," "); //slicing
        }

    }
}


//////////////////////////////////////////////
//                                          //
//              SUBSET 1                    //
//                                          //
//////////////////////////////////////////////
bool test_can_encrypt(char filename[MAX_PATH_LEN]) {
  FILE *fp;
  char command[100]="ls -l ";
  strcat(command,filename); //la -l <filename> returns the permissions
  
  //keeping the output of permissions in ret 
  char ret[12];
  fp = popen(command, "r");
  
  if (fp == NULL) //if Some Error occoures, exit.
  {
    //exception handling
    printf("Failed to run command\n" );
    exit(1);
  }
 fgets(ret, sizeof(ret), fp);
  if(ret[0]!='-'){printf(MSG_ERROR_DIRECTORY,filename);return false;} //if not directory
  if(ret[4]!='r'){printf(MSG_ERROR_READ,filename);return false;}  //if group cant read
  if(ret[5]!='w'){printf(MSG_ERROR_WRITE,filename);return false;} //if group cant write
  else{return true;}
}

//
void simple_xor_encryption(char filename[MAX_PATH_LEN]) {
    if (!test_can_encrypt(filename)) return;
    char writebuffer[1000]; //buffer which will be written off on file
    char readbuffer[1];//buffer which will be read once char at a time
    /* Open file for both reading and writing */
    int count=0; //specify the number of bytes to read
    FILE *fp;
    fp = fopen(filename, "rb");
    //feof() is special type eof for binary files/
    while (!feof(fp))
    {
        //read byte by byte and xor it with 0xFF
        fread(readbuffer, sizeof(readbuffer),1, fp);
        writebuffer[count]=readbuffer[0]^0xFF; //xoring 
        count++;
    }
    
    //writing to file
    char outputfile[100]=""; //outputfile name
    strcat(outputfile,filename);
    strcat(outputfile,".xor");
    fp = fopen(outputfile, "wb");
    fwrite(writebuffer, count-1, 1, fp);
    fclose(fp);
}


void simple_xor_decryption(char filename[MAX_PATH_LEN]) 
{
    if (!test_can_encrypt(filename)) return;
    char writebuffer[1000]; //buffer which will be written off on file
    char readbuffer[1]; //buffer which will be read once char at a time
    /* Open file for both reading and writing */
    int count=0;
    FILE *fp;
    fp = fopen(filename, "rb");
    //feof() is special type eof for binary files
    while (!feof(fp))
    {
        fread(readbuffer, sizeof(readbuffer),1, fp);
        writebuffer[count]=readbuffer[0]^0xFF; //xoring charecter by charecters and storing it back to the string
        count++;
    }
    
    //writing to file
    char outputfile[100]="";
    strcat(outputfile,filename); //strcat, concatinate filename to outputfile
    strcat(outputfile,".dec");
    fp = fopen(outputfile, "wb");
    fwrite(writebuffer, count-1, 1, fp);
    fclose(fp);
}


//////////////////////////////////////////////
//                                          //
//              SUBSET 2                    //
//                                          //
//////////////////////////////////////////////

void search_by_filename(char filename[MAX_SEARCH_LENGTH]) {
    //exception handling    
    //if user enter . or .. we give the MSG_ERROR_RESERVED exception 
    if(strcmp(filename, "..") == 0 ||strcmp(filename, ".") == 0){printf("%s",MSG_ERROR_RESERVED);} 
    //if user enter empty we give the MSG_ERROR_SEARCH exception 
    if(strcmp(filename, "") == 0){printf("%s",MSG_ERROR_SEARCH);}
    struct files f1;
    f1.count=0;
    
    //memory allocation
    for(int i=0;i<40;i++){ //there will be 40 files in the structure
        f1.path[i]=(char*) malloc(100);  //address of file can be 100 char
        f1.name[i]=(char*) malloc(40);   //name of the file can be 40 char
        strcpy(f1.path[i],"");
        strcpy(f1.name[i],"");
    }
   
    //current directory
    FILE *fp;
    char ret[20]=""; 
    fp = popen("basename \"$PWD\"", "r");  //baseline "$PWD" return the name of current working directory
     
    //error handling
    if (fp == NULL) {printf("Failed to run command\n" );exit(1);}
    fgets(ret, sizeof(ret), fp);
    pclose(fp);
    
    //filling out files struct
    chdir("..");  //moving back a directory
    char temp[20]="";
    strncpy(temp,ret,strlen(ret)-1); 
    listdir(temp,".",&f1,1);  //list all the directories in the working directory 
    chdir(temp); //move the the working directory

    //saving paths
    char* p[40]; //paths
    for(int i=0;i<40;i++){
        p[i]=(char*) malloc(100);strcpy(p[i],"");}

    int filecount=0; //count of files
    for(int i=0;i<40;i++){
        if(strstr(f1.name[i],filename)){
            strcpy(p[filecount],f1.path[i]);
            filecount++;
        }
    }

    sort_by_name(p,filecount); //given function that sort the files in assending order
    for (int i =0;i<filecount;i++){
            
            //finding the permissions of the file just as in subset 1
            char command[100]="ls -la "; 
            strcat(command,p[i]);
            char ret1[11]="";
            fp= popen(command, "r");
            fgets(ret1, sizeof(ret1), fp);
            if(ret1[0]!='-' && ret1[0]!='d'){fgets(ret1, sizeof(ret1), fp);}
    
            //output
            printf("%s\t%s\n",ret1,p[i]);

    }
    //deallocation
    for(int i=0;i<40;i++){
        free(f1.path[i]); 
        free(f1.name[i]);
        free(p[i]);
    }
}


void search_by_text(char text[MAX_SEARCH_LENGTH]) {
    if(strcmp(text, "") == 0){printf("%s",MSG_ERROR_SEARCH);}
    struct files f1;
    f1.count=0;
    //40 files can be saved
    for(int i=0;i<40;i++){
        f1.path[i]=(char*) malloc(100);
        f1.name[i]=(char*) malloc(40);
        strcpy(f1.path[i],"");
        strcpy(f1.name[i],"");
    }
    //executing a command to find the name of current directory 
    FILE *fp;
    char outp[20]=""; //filename
    fp = popen("basename \"$PWD\"", "r"); //pwd outpurns current directory
    
    if (fp == NULL) {printf("Failed to run command\n" );exit(1);}
    fgets(outp, sizeof(outp), fp);
    pclose(fp);
    
    //this will help us print all the files from 
    chdir(".."); //going back a directory
    char temp[20]="";
    strncpy(temp,outp,strlen(outp)-1); //removing the '\0' from the input string
    listdir(temp,".",&f1,0);  
    chdir(temp); //going into the directory
    
    char readstring[3000]=""; //size of the file to be read 3000 char
    struct text_find tf[20]; //20files handled at max
    for (int i=0;i<20;i++){strcpy(tf[i].path,"");tf[i].count=0;}  //20 files structure
 
    int filecount=0;
    for (int i=0;i<40;i++)
    {
        strncpy(readstring,"",sizeof(readstring));
        if(strcmp(f1.path[i],"")!=0 && strstr(f1.path[i],".txt"))
        {
            //reading file content
            fp = fopen(f1.path[i], "rb");
            fread(readstring, 3000, 1, fp);
            //finding number of occuerences
            int c=countString(readstring,text);
            //filter out the 0 count files 
            if(c)
            {
            filecount++;
            strcpy(tf[i].path,f1.path[i]); 
            tf[i].count=c;
            }

            fclose(fp);
        }
    }
        //use sort by count which is a given function
        sort_by_count(tf,20);
        
        //print the files
        for (int i=0;i<filecount;i++)
        {
         printf("%d:\t%s\n",tf[i].count,tf[i].path);
        }

        //deallocate the memory
        for(int i=0;i<40;i++){
        free(f1.path[i]);
        free(f1.name[i]);
    }

}

//////////////////////////////////////////////
//                                          //
//              SUBSET 3                    //
//                                          //
//////////////////////////////////////////////
void electronic_codebook_encryption(char filename[MAX_PATH_LEN], char password[CIPHER_BLOCK_SIZE + 1]) {
    if (!test_can_encrypt(filename)) return;
    char writebuffer[1000]="";
    char readbuffer[16]="";

    /* Open file for both reading and writing */
    FILE *fp;
    fp = fopen(filename, "rb");
    int count=0;
    while (!feof(fp))
    {
        strncpy(readbuffer,"",sizeof(readbuffer));
        count++;
        fread(readbuffer, sizeof(readbuffer),1, fp); //reading the file 16 byte at a time 
        if(!strcmp(readbuffer,"")){count--;} //if the read string was empty, dont save it in encrypted file

        char* ret=shift_encrypt(readbuffer,password);
        strncat(writebuffer,ret,16);
        free(ret); //ret is a pointer to malloc(16) in function, and should be cleared by programmer
    }

    fclose(fp);
    //writing the encrypted file.
    char outputfile[100]="";
    strcat(outputfile,filename); 
    strcat(outputfile,".ecb");
    fp = fopen(outputfile, "wb");
    fwrite(writebuffer, (count)*16, 1, fp);
    fclose(fp);
}

void electronic_codebook_decryption(char filename[MAX_PATH_LEN], char password[CIPHER_BLOCK_SIZE + 1]) {
     if (!test_can_encrypt(filename)) return;
    char writebuffer[1000]="";
    char readbuffer[16]="";
    /* Open file for both reading and writing */
    FILE *fp;
    fp = fopen(filename, "rb");
    int count=0;
    while (!feof(fp))
    {
        strncpy(readbuffer,"",sizeof(readbuffer));
        count++;
        fread(readbuffer, sizeof(readbuffer),1, fp);//reading the file 16 byte at a time 
        if(!strcmp(readbuffer,"")){count--;}//if the read string was empty, dont save it in encrypted file
        char *ret=shift_decrypt(readbuffer,password);
        strncat(writebuffer,ret,16);
        free(ret); //ret is a pointer to malloc(16) in function, and should be cleared by programmer
    }
    fclose(fp);
    
    //writing the decrypted file.
    char outputfile[100]="";
    strcat(outputfile,filename); 
    strcat(outputfile,".dec");
    fp = fopen(outputfile, "wb");
    fwrite(writebuffer, (count)*16, 1, fp);
    fclose(fp);
}


char* shift_encrypt(char *plaintext, char *password) {
    //shift 16 char of plain text left with respect to password 
    char* encrypted=(char *)malloc(16);
    for(int i=0;i<16;i++)
    {
    int shift=password[i]%8;
    encrypted[i]= (((unsigned char)plaintext[i])<<shift)|(((unsigned char)plaintext[i])>>(8-shift));
    }
    return encrypted;
}


char *shift_decrypt(char *ciphertext, char *password) {
     //shift 16 char of plain text right with respect to password 
    char* decrypted=(char*)malloc(16); 
    for(int i=0;i<16;i++)
    {
    int shift=password[i]%8;
    decrypted[i]= (((unsigned char)ciphertext[i])>>shift)|(((unsigned char)ciphertext[i])<<(8-shift));
    }
    return decrypted;
}



//////////////////////////////////////////////
//                                          //
//              SUBSET 4                    //
//                                          //
//////////////////////////////////////////////
//in progress
void cyclic_block_shift_encryption(char filename[MAX_PATH_LEN], char password[CIPHER_BLOCK_SIZE + 1]) {
}


void cyclic_block_shift_decryption(char filename[MAX_PATH_LEN], char password[CIPHER_BLOCK_SIZE + 1]) {
}




// PROVIDED FUNCTIONS, DO NOT MODIFY

// Generates a random string of length RAND_STR_LEN.
// Requires a seed for the random number generator.
// The same seed will always generate the same string.
// The string contains only lowercase + uppercase letters,
// and digits 0 through 9.
// The string is returned in heap-allocated memory,
// and must be freed by the caller.
char *generate_random_string(int seed) {
    if (seed != 0) {
        srand(seed);
    }

    char *alpha_num_str =
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "0123456789";

    char *random_str = malloc(RAND_STR_LEN);

    for (int i = 0; i < RAND_STR_LEN; i++) {
        random_str[i] = alpha_num_str[rand() % (strlen(alpha_num_str) - 1)];
    }

    return random_str;
}

// Sorts the given array (in-place) of files with
// associated counts into descending order of counts.
// You must provide the size of the array as argument `n`.
void sort_by_count(struct text_find *files, size_t n) {
    if (n == 0 || n == 1) return;
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (files[j].count < files[j + 1].count) {
                struct text_find temp = files[j];
                files[j] = files[j + 1];
                files[j + 1] = temp;
            } else if (files[j].count == files[j + 1].count && strcmp(files[j].path, files[j + 1].path) > 0) {
                struct text_find temp = files[j];
                files[j] = files[j + 1];
                files[j + 1] = temp;
            }
        }
    }
}

// Sorts the given array (in-place) of strings alphabetically.
// You must provide the size of the array as argument `n`.
void sort_by_name(char *filenames[], size_t num_filenames) {
    if (num_filenames == 0 || num_filenames == 1) return;
    for (int i = 0; i < num_filenames - 1; i++) {
        for (int j = 0; j < num_filenames - i - 1; j++) {
            if (strcmp(filenames[j], filenames[j + 1]) > 0) {
                char *temp = filenames[j];
                filenames[j] = filenames[j + 1];
                filenames[j + 1] = temp;
            }
        }
    }
}


// ADD YOUR FUNCTION DEFINITIONS HERE
int countString(char *mainstring, char *searchstring){
    //count the number of occurences of needle in the haystack
    int occurences = 0;
    char *tmp = mainstring; //pointer to the cursor
    while(tmp = strstr(tmp, searchstring)) //loop unless there is a match
    {
        occurences++;
        tmp++;
    }
    return occurences;
}

void listdir(char *name,char *connecter, struct files *f1,bool d_flag)
{
    DIR *dir; //dirent to navigate directories
    struct dirent *entry;
    if (!(dir = opendir(name)))
        return;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            char path[1024]="";         
            char disp[1024]="";

            if (strcmp(entry->d_name, "..") == 0 ||strcmp(entry->d_name, ".") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            snprintf(disp, sizeof(disp), "%s/%s", connecter, entry->d_name);
            if(d_flag) 
            {
                //saving the directory name in struct
                char temp[1024]="";
                strcpy(temp,connecter);
                strcat(temp,"/");
                strcat(temp,entry->d_name);
                strcpy(f1->name[f1->count],entry->d_name);
                strcpy(f1->path[f1->count],temp);
                f1->count++;
            }
            listdir(path,disp,f1,d_flag);
        }
        else {
            //saving the adress of file in the struct
            char temp[1024]="";
            strcpy(temp,connecter);
            strcat(temp,"/");
            strcat(temp,entry->d_name);
            strcpy(f1->name[f1->count],entry->d_name);
            strcpy(f1->path[f1->count],temp);
            f1->count++;
        }
}
closedir(dir);
}