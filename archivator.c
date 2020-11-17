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
            char* full_path = get_filepath(entry->d_name,dir_path);
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
    int out, nread, size;
    char* size_;
    
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
    
    size = sizeof(int)*nread;
    size_ = from_int_to_char(nread, size);
    size = strlen(size_);
    write(out, size_, sizeof(char)*size);

    if (write(1, block, nread) != nread)
    {
        perror("Не удаётся записать данные");
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
    int mem = 3;
    char* length_;
    
    length_ = from_int_to_char(length, mem);
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

char* from_int_to_char(int numb, int mem)
{
    char buf[mem];
    char* numb_;

    free_mem(buf, mem);   
    sprintf(buf, "%d", numb);
    numb_ = to_char(buf);

    return numb_;
}

int unpack(char *archiv_path, char *folder)
{
    int archiv, file;
    char* path_length;
    char* file_path = NULL;
    int path_length_, size;
    char buf[1024];

    if ((archiv = open(archiv_path, O_RDONLY)) == -1)
    {
        perror("Не удаётся открыть архив!");
        return -5;
    }

    while(1)
    {
        //read path length
        if (read(archiv, buf, 3) == -1)
        {
            perror("Не удаётся прочитать данные из архива!");
            return -6;
        }

        path_length_ = from_char_to_int(buf)-1;
        free_mem(buf, 3);

        if (read(archiv, buf, path_length_) == -1)
        {
            perror("Не удаётся прочитать данные из архива!");
            return -6;
        }

        //file_path = buf;
        //strcat(file_path, buf);
        free_mem(buf, path_length_);

        if (read(archiv, buf, 1) == -1)
        {
            perror("Не удаётся прочитать данные из архива!");
            return -6;
        }

        size = from_char_to_int(buf);
        free_mem(buf, 3);

        if (read(archiv, buf, size) == -1)
        {
            perror("Не удаётся прочитать данные из архива!");
            return -6;
        }

        file = open("new.txt",O_WRONLY|O_APPEND|O_CREAT, S_IROTH|S_IRGRP|S_IRUSR|S_IWGRP|S_IWOTH|S_IWUSR);

        if (write(file, buf, size) == -1)
        {
            perror("Не удаётся записать данные");
            return -4;
        }


        free_mem(buf, size);
        close(file);
        break;
        
    }

    close(archiv);

    return 0;
}

int from_char_to_int(char str[])
{
    int res;
    char* str_ = to_char(str);
    char s[3];

    strcpy(s, str_);
    res = atoi(str);
    return res;
}