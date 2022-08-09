#ifndef FILE_LOADING
#define FILE_LOADING

int validate_runtime_arguments(int argc, char* argv[]);
int validate_file(int argc, char* argv[]);
int get_file_size(FILE * source_file_fp);

#endif
