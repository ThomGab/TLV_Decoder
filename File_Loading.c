#pragma warning(disable:4996)
#include <stdio.h>
#include <string.h>
#include "File_Loading.h"

FILE *verify_inputs(int argc, char *argv[]){

	/* Clarifying Inputs */
	/* Does the number of input arguments equal the expected number? */
	FILE *source_file_fp;
	int expctd_nmbr_inputs = 2;
	char *expctd_format = ".txt";

	if(argc != expctd_nmbr_inputs){
		if(argc < expctd_nmbr_inputs){
			printf("Error: Too few arguments.\n");
		}
		else{
			printf("Error: Too many arguments.\n");
		}
		printf("Expected format ./TLV_Parse \"SourceFile.txt\"\n\n");
		return NULL;
	}
	/* Is the filepath input a .txt file and if so, does it exist? */
	else{
		if(strstr(argv[1], expctd_format) == NULL){
			printf("Error: Invalid File format. Please convert to .txt file and reload.\n\n");
			return NULL;
		}
		else if( fopen(argv[1],"r") == NULL ){
			printf("Error: Invalid File path. Could not find file.\n\n");
			return NULL;
		}
		else{
			printf("Opening File...\n\n");
			source_file_fp = fopen(argv[1],"r");
			return source_file_fp;
		}
	}

	printf("Unhandled Error\n");
return NULL;
}

int get_file_size(FILE * source_file_fp){

	int file_size = 0;
	fseek(source_file_fp,0, SEEK_END);
	file_size = ftell(source_file_fp);
	fseek(source_file_fp,0, SEEK_SET);

	//This method returns an extra character than required, is none cardinal.

	return (file_size);
}

