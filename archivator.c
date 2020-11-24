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
    int objects, type, file;

    if (folder == NULL)
    {
        perror("Не удалось открыть папку");
        return -1;
    }

    while (entry = readdir(folder))
    {
        printf("%s\n", entry->d_name);
        char* full_path = get_filepath(entry->d_name,dir_path);

        if (DT_DIR == entry->d_type)
        {        
            if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
                continue;

            type = 0;

            objects = count_objects_in_dir(full_path);
            rewrite_dir(type, full_path, objects);
            pack(full_path);
        }

        else if (entry->d_type == DT_REG)
        {
            type = 1;
            rewrite_info(type, full_path);           
            file = open(full_path, O_RDONLY);

            if (rewrite_file(file) == 0)
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
    char root[2] = "./";
    char check_root[2];

    free_mem(full_path, mem);

    //strcat(full_path,"/home/anastasia/Archivator/");

    strncat(check_root, path, 2);


    if (strncmp(check_root, root, 2) != 0)
    {
        strncat(full_path,root,2);
    }

    strcat(full_path,path);
    strcat(full_path,"/");
    strcat(full_path,file);
    
    full_path_ = to_char(full_path);

    return full_path_;
}

char* get_newpath(char* dir, char* file)
{
    int mem = strlen(file)+strlen(dir);
    char full_path[mem];
    char* full_path_ = NULL;

    free_mem(full_path, mem);
    strcat(full_path,"/");
    strcat(full_path,dir);
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

int rewrite_dir(int type, char *path, int count)
{
    rewrite_number(type);
    rewrite_number(count);
    return 0;
}

void rewrite_number(int n)
{
    int mem = SIZE;
    int out;
    char* n_;

    out = open("archiv.txt",O_WRONLY|O_APPEND|O_CREAT, S_IROTH|S_IRGRP|S_IRUSR|S_IWGRP|S_IWOTH|S_IWUSR);

    if (out == -1)
    {
        perror("Ошибка: не удаётся создать выходной файл или открыть существующий с таким же именем.");
    }

    n_ = from_int_to_char(n, mem);

    char _n_[mem];

    for (int i = 0; i < mem; i++)
    {
        _n_[i] = *(n_+i);
    }
        
    if (write(out, _n_, mem-1) == -1)
    {
        perror("Не удаётся записать данные");
    } 

    close(out);   
}

int rewrite_file(int in)
{  
    int out, nread, size;
    char block[1024];
    char* size_;
    
    if (in == -1)
    {
        perror("Ошибка: входной файл не существyет или не открывается.");
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
    
    size = SIZE;
    size_ = from_int_to_char(nread, size);
    write(out, size_, size);

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

int rewrite_info(int type, char *path)
{
    int length = strlen(path);
    int out;
    int mem = SIZE;
    char* length_;

    rewrite_number(type);
    
    length_ = from_int_to_char(length, mem);
    out = open("archiv.txt",O_WRONLY|O_APPEND|O_CREAT, S_IROTH|S_IRGRP|S_IRUSR|S_IWGRP|S_IWOTH|S_IWUSR);

    if (out == -1)
    {
        perror("Ошибка: не удаётся создать выходной файл или открыть существующий с таким же именем.");
        return -2;
    }

    // write length of the file
    if (write(out, length_, mem) == -1)
    {
        perror("Ошибка: не удаётся осуществить дозапись.");
        return -4;
    }

    // write path of the file
    if (write(out, path, sizeof(char)*length) == -1)
    {
        perror("Ошибка: не удаётся осуществить дозапись.");
        return -4;
    }

    close(out);

    return 0;
}

int count_objects_in_dir(char* path)
{
    DIR *folder = opendir(path);
    struct stat statbuf;
    struct dirent *entry;
    int count = 0;

    if (folder == NULL)
    {
        perror("Не удалось открыть папку");
        return -1;
    }

    while (entry = readdir(folder))
    {
        if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
                continue;

        printf("%s\n", entry->d_name);
        count++;
    }

    return count;
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
    int archiv, file, dir, path_length_, size, size_file;
    char* path_length;
    char* file_path = NULL;
    char buf[1024];

    if ((archiv = open(archiv_path, O_RDONLY)) == -1)
    {
        perror("Не удаётся открыть архив!");
        return -5;
    }

    size = SIZE;

    if ((dir = mkdir(folder, S_IROTH|S_IRGRP|S_IRUSR|S_IWGRP|S_IWOTH|S_IWUSR)) == -1)
    {
        perror("Не удаётся создать папку.");
        return -5;
    }

    while(read(archiv, buf, size) != -1)
    {
        if (buf[0] == '\000')
        {
            break;
        }

        path_length_ = from_char_to_int(buf);
        free_mem(buf, size);
        
        //read path
        if (read(archiv, buf, path_length_) == -1)
        {
            perror("Не удаётся прочитать данные из архива!");
            return -6;
        }

        file_path = to_char(buf);
        char* full_path_ = get_filepath(file_path, folder); 
        file = open(file_path,O_WRONLY|O_APPEND|O_CREAT, S_IROTH|S_IRGRP|S_IRUSR|S_IWGRP|S_IWOTH|S_IWUSR);

        if (file == -1)
        {
            perror("Ошибка: не удаётся создать выходной файл или открыть существующий с таким же именем.");
            return -2;
        }

        free_mem(buf, path_length_);

        if (read(archiv, buf, size) == -1)
        {
            perror("Не удаётся прочитать данные из архива!");
            return -6;
        }

        size_file = from_char_to_int(buf);
        free_mem(buf, size);

        if (read(archiv, buf, size_file) == -1)
        {
            perror("Не удаётся прочитать данные из архива!");
            return -6;
        }   

        if (write(file, buf, size_file) == -1)
        {
            perror("Не удаётся записать данные");
            return -4;
        }

        free_mem(buf, size_file);
        close(file);  
        //int l = link(full_path_, file_path);  
        break;
    }

    close(archiv);

    return 0;
}

int from_char_to_int(char str[])
{
    int res;
    char* str_ = to_char(str);
    int mem = SIZE;
    char s[mem];

    strcpy(s, str_);
    res = atoi(str);
    return res;
}