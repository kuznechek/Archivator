#define SIZE 4;

int rewrite_file(int file);
void print_dir(char *dir, int depth);
int pack(char *input);
int unpack(char *archiv_path, char *folder);
char* get_filepath(char* file, char* path);
char* get_newpath(char* dir, char* file);
int free_mem(char array[],int n);
int rewrite_info(int type, char *path);
int rewrite_dir(int type, char *path, int count);
char* to_char(char buf[]);
char* from_int_to_char(int numb, int mem);
int from_char_to_int(char str[]);
int count_objects_in_dir(char* path);
void rewrite_type(int type);