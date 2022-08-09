#pragma warning(disable:4996)
#include <stdio.h>
#include <string.h>
#include "File_Loading.h"


void print_runtime_arguments(int argc, char** argv) {

	for (int i = 1; i <= argc; i++)
	{
		printf("Compile time argument %d/%d -> %s \n", i, argc, argv[i-1]);
	};
	return;
}

int get_file_size(FILE* source_file_fp) {

	int file_size = 0;
	fseek(source_file_fp, 0, SEEK_END);
	file_size = ftell(source_file_fp);
	rewind(source_file_fp);
	//This method returns an extra character than required, is none cardinal.

	return (file_size);
}

int validate_runtime_arguments(int argc, char* argv[]) {
	
	int expctd_nmbr_inputs = 2;

	if (argc != expctd_nmbr_inputs) {
		if (argc < expctd_nmbr_inputs) {
			printf("Error: Too few runtime arguments.\n");
		}
		else {
			printf("Error: Too many runtime arguments.\n");
		}

		print_runtime_arguments(argc, argv);

		printf("Expected arguments  => 1)./TLV_Parse 2)\"FileToParse.txt\"\n\n");
		return 0;
	}

	return 1;
}

int validate_file(int argc, char *argv[]){

	/* verifying file format */
	FILE *source_file_fp;
	char *expctd_format = ".txt";

	/* Is the filepath input a .txt file and if so, does it exist? */
	if (strstr(argv[1], expctd_format) == NULL) {
		printf("Error: Invalid File format. Please convert to .txt file and reload.\n\n");
		return 0;
	}

	// open file, with read privileges only, if it does not exist default to the example file.
	if(fopen(argv[1], "r") == NULL) {
		printf("Error: Invalid File path. Could not find file %s).\n\n", argv[1]);
		return 0;
	}

	return 1;
}

