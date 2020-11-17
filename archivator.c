#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <limits.h>
#include <termio.h>
#include </home/anastasia/Archivator/headers/archivator.h>


int pack(char *dir_path)
{
    DIR *folder = opendir(dir_path);
    struct stat statbuf;
    struct dirent *entry;

    if (folder == NULL)
    {
        perror("Не удалось открыть папку");
        return -1;
    }

    while (entry = readdir(folder))
    {
        printf("%s\n", entry->d_name);

        if (DT_DIR == entry->d_type)
        {        

            if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
                continue;
            pack(entry->d_name);
        }

        else if (entry->d_type == DT_REG)
        {
            char* full_path = get_filepath(entry->d_name,dir_path); // length of path (full) NEED TO BE SAVED
            int file;

            rewrite_info(full_path);
            
            file = open(full_path, O_RDONLY);

            if (rewrite(file) == 0)
            {
                printf("Перезапись файла осуществлена успешно!\n");
            }       
        }

        
    }
    return 0; 
}

char* get_filepath(char* file, char* path)
{
    int mem = strlen(path)+strlen(file);
    char full_path[mem];
    char* full_path_ = NULL;

    free_mem(full_path, mem);
    strcat(full_path,"/home/anastasia/Archivator/");
    strcat(full_path,path);
    strcat(full_path,"/");
    strcat(full_path,file);
    
    full_path_ = to_char(full_path);

    return full_path_;
}

int free_mem(char array[], int n)
{
    for (int i = 0; i < n; i++)
    {
        array[i] = '\000';
    }

    return 0;
}

int rewrite(int in)
{
    char block[1024];
    int out;
    int nread;
    
    if (in == -1)
    {
        perror("Ошибка: входной файл не существет или не открывается.");
        return -1;
    }

    out = open("archiv.txt",O_WRONLY|O_APPEND|O_CREAT, S_IROTH|S_IRGRP|S_IRUSR|S_IWGRP|S_IWOTH|S_IWUSR);

    if (out == -1)
    {
        perror("Ошибка: не удаётся создать выходной файл или открыть существующий с таким же именем.");
        return -2;
    }

    nread = read(in, block, sizeof(block));

    if (nread == -1)
    {        
        perror("Ошибка: файл не читается.");
        return -3;
    }

    if (write(1, block, nread) != nread)
    {
        perror("Ошибка: не удаётся осуществить дозапись.");
        return -4;
    }

    write(out,block,nread);
    close(out);
    close(in);

    return 0;
}

int rewrite_info(char *path)
{
    int length = strlen(path);
    int out;
    
    char buf[3];
    char* length_;


    free_mem(buf, 3);
    
    sprintf(buf, "%d", length);
    length_ = to_char(buf);
    

    out = open("archiv.txt",O_WRONLY|O_APPEND|O_CREAT, S_IROTH|S_IRGRP|S_IRUSR|S_IWGRP|S_IWOTH|S_IWUSR);

    if (out == -1)
    {
        perror("Ошибка: не удаётся создать выходной файл или открыть существующий с таким же именем.");
        return -2;
    }

    if (write(out, length_, sizeof(char)*2) == -1)
    {
        perror("Ошибка: не удаётся осуществить дозапись.");
        return -4;
    }

    if (write(out, path, sizeof(char)*length) == -1)
    {
        perror("Ошибка: не удаётся осуществить дозапись.");
        return -4;
    }

    close(out);

    return 0;
}

char* to_char(char buf[])
{
    char** ptr = NULL;
    void* t;
    
    ptr = calloc(1, sizeof *ptr);
    t = realloc (ptr[0], sizeof*buf);

    if (t == NULL)
    {
        perror("Не удалось выделить память.");
    }

    ptr[0] = malloc(strlen(buf));
    strcpy(ptr[0],buf);

    return ptr[0];
}