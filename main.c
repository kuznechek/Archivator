#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include </home/anastasia/Archivator/archivator.c>

int main()
//int main(int argc, char* arg[])
{
    int argc = 3;

    if (argc < 3)
    {
        printf("Требуется больше аргументов: не введена функция или имя директории.\n");
        return 1;
    }

    //char* func = arg[1]; // what to do
    //char* dir = arg[2]; // with it
    char* func = "repack"; // what to do
    //char* dir = "Test"; // with it
    char* dir = "archiv.txt";

    printf("%s\n",func);
    printf("%s\n",dir);
    printf("\n");

    if (strcmp("pack", func) == 0)
    {
	    if (pack (dir) == 0)
            printf("Архивация прошла успешно.\n");

        else
            printf("Архивация прошла неуспешно.\n");
    }

    if (strcmp("repack", func) == 0)
    {
	    if (repack (dir) == 0)
            printf("Разархивация прошла успешно.\n");

        else
            printf("Разархивация прошла неуспешно.\n");
    }


    else
    {
        printf("Не удалось определить функцию.\n");
    }

    return 0;
}
