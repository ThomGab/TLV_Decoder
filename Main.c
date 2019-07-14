#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Bits_n_Bytes.h"
#include "Tag_List.h"
#include "File_Loading.h"

int main(int argc, char *argv[]){

	
	//Source_File Variables
	FILE * source_file_fp = NULL;
	char * source_file_arr = NULL;
	int file_size = 0;
	int file_pos = 0; // where in the file is in the current Nibble?
	int TLV_Block_pos = 0; // where in the file has the TLV Data been printed up until?

	//Parsing Variables
	
	unsigned int First_Nibble_In_Byte;
	unsigned int* First_Nibble_In_Byte_ptr = &First_Nibble_In_Byte;

	unsigned int TagField_Size_Bytes;
	unsigned int * TagField_Size_Bytes_ptr = &TagField_Size_Bytes;
	
	unsigned int nibble_flags = 0; // Initialising All Processing Flags 0000 0000
	unsigned int * nibble_flags_ptr = &nibble_flags;

	int Reading_Status = 0;

	int Length = 0; //This is the length of the value field element.
	int Length_Field_Size_Bytes = 0;
	int Length_Field_Pos = 0;
	int * Length_Field_Pos_ptr = &Length_Field_Pos;

	int Invalid_Data_Found = 0;
	int Resized_Array_Complete = 0;
	int y = 0;

	char input_nibble_cleaned;
	char input_nibble_str[2];

	//Buffers
	char * Tag_Def = NULL;
	char * Temp_Buffer = NULL;
	char * TLV_Block = NULL;
	char * Output = NULL;
	char * Temp;
	

	//Verify Input File and Establish it's size in Nibbles.
	source_file_fp = verify_inputs(argc, argv);
	if (source_file_fp == NULL){
		printf("Exiting...\n");
		return 0;
	}
	else{
		file_size = get_file_size(source_file_fp); //Null Terminating Character required.
	}

	//Create and write the file content to an array, close the file.
	source_file_arr = malloc( (file_size + 1) * sizeof(char) );

	if (source_file_arr == 0) {
		printf("NAH BRO\n");
		return 0;
	}

	fread(source_file_arr, sizeof(char), file_size, source_file_fp);
	source_file_arr[file_size] = '\0';

	printf("Input:\n %s\n\n", source_file_arr);

	printf("Beginning Tag Processing...\n\n");
	
	TagField_Size_Bytes = 1;
	Reading_Status = 1;
	Set_Bit(Processing_Tag, nibble_flags_ptr);
	Set_Bit(Processing_First_Nibble, nibble_flags_ptr);

	for( file_pos = 0; source_file_arr[file_pos] != '\0'; file_pos++){

		if (file_pos != 0) {
			if (Is_Bit_Set(Processing_First_Nibble, nibble_flags_ptr)) {
				UnSet_Bit(Processing_First_Nibble, nibble_flags_ptr);
			}
			else {
				Set_Bit(Processing_First_Nibble, nibble_flags_ptr);
			}

		}

		if (Length_Field_Pos > ((Length * 2)) && (Length != 0) && !Is_Bit_Set(Processing_Tag,nibble_flags_ptr)) {
			printf("Value Processing Complete.\n");
			Output = TLV_Block_to_Output(Output, TLV_Block);
			TLV_Block_pos = file_pos;
			UnSet_Bit(Processing_Value, nibble_flags_ptr);
			Set_Bit(Processing_Tag, nibble_flags_ptr);
			Temp_Buffer = NULL;
			Length_Field_Pos = 0;
			Length = 0;
			Reading_Status = Determine_Reading_Status(nibble_flags_ptr, Invalid_Data_Found);
			Debug_ReadingStatus(nibble_flags_ptr, Reading_Status);
			TLV_Block = NULL;

		}

		printf("Reading Status Pre_Nibble Read\n");
		Debug_ReadingStatus(nibble_flags_ptr, Reading_Status);
		input_nibble_cleaned = Clean_Input((source_file_arr[file_pos]));
		
		input_nibble_str[0] = input_nibble_cleaned;
		input_nibble_str[1] = '\0';

		printf("INPUT CHARACTER IS CURRENTLY: %c\n", input_nibble_cleaned);
		printf("IS IT FIRST NIBBLE? %d\n\n", Is_Bit_Set(Processing_First_Nibble, nibble_flags_ptr));

		//Invalid Data Check
		if ( (input_nibble_cleaned == '#') || (Invalid_Data_Found == 1) ) {
			
			Reading_Status = 6;
		
		}


		switch(Reading_Status){

			// Tag Not Completely Read, still evaluting Tag Value nibble by nibble. 
			case(1):

				//Setting up Tag Buffer.
				if( (Temp_Buffer == NULL) ){
					Temp_Buffer = malloc( ((sizeof * Temp_Buffer) * ((TagField_Size_Bytes * 2) + 1)) );
					*Temp_Buffer = '\0';
					printf("Initialising TagField Buffer...\n");
				}
									
				else{
					if (Is_Bit_Set(Processing_Subsequent_Field_FirstNibFlag, nibble_flags_ptr) && Is_Bit_Set(Processing_Subsequent_Field, nibble_flags_ptr)) {
						printf("We need to Realloc the Tag Buffer");
						Temp = realloc(Temp_Buffer, ((sizeof(char) * TagField_Size_Bytes * 2) + 1));
						if (Temp != NULL) {
							Temp_Buffer = Temp;
							Temp = NULL;
						}
						else {
							printf("Failed Reallocation of Memory.\n");
							printf("Exiting...\n");
							return 0;
						}
					}
					else {
						printf("No Realloc required Buffer");
					}
					//If the Input Data is not invalid, then pass the character to be Processed.
				}

				Tag_Processing((source_file_arr[file_pos]), nibble_flags_ptr, TagField_Size_Bytes_ptr);
				printf("\nReading Status After Tag Processing\n");
				Debug_ReadingStatus(nibble_flags_ptr, Reading_Status);

				//Append this value to the search Tag Buffer.
				printf("Writing %c ", input_nibble_cleaned);
				printf("To %s\n", Temp_Buffer);

				//If the Size of The Tag Field has increased, we must reallocate the size of the initial Buffer.
				/*if ((Is_Bit_Set(Processing_Tag_FirstByte, nibble_flags_ptr) == 0) && (Is_Bit_Set(Processing_Large_TagField_firstnib, nibble_flags_ptr) == 1)) {
						Tag_Buffer_Start = realloc(Tag_Buffer_Start, ((sizeof * Temp_Buffer) * ((TagField_Size_Bytes * 2) + 1)));
						printf("Expanding TagField Buffer...\n");
						printf("Tag_Buffer_Start %p\n", Tag_Buffer_Start);
						printf("Temp_Buffer %p\n", Temp_Buffer);
					}
				//If it hasn't, we don't need to expand the Tag Buffer.
				else{

				}*/

				//strcat(Temp_Buffer, input_nibble_str);
				strcat(Temp_Buffer, input_nibble_str);
				printf("Temp_Buffer, \t %s\n", Temp_Buffer);

				break;

			case(2):
				printf("Tag recieved, Beginning Lookup %s\n\n", Temp_Buffer);
				
				Tag_Def = Find_Tag_Def(Tag_Def, Temp_Buffer, TagList);
								
				if (Tag_Def != NULL){
					//change this to write into the final TLV_Block one line at a time.
					TLV_Block = malloc( (sizeof(char)) * (strlen(Tag_Def) + strlen(Temp_Buffer) + 2) );
					strcpy(TLV_Block,Temp_Buffer);
					strcat(TLV_Block,Tag_Def);
					strcat(TLV_Block, "\n");
					Tag_Def = NULL;
					Temp_Buffer = NULL;
					printf("TLV_Block:\n%s", TLV_Block);
				}
				else{
					Tag_Def = "- Undefined Tag\n";
					strcpy(TLV_Block, Temp_Buffer);
					strcat(TLV_Block, Tag_Def);
					printf("TLV_Block:\n%s", TLV_Block);	
				}

				/*free(Temp_Buffer);
				free(Tag_Def);*/
	
				Set_Bit(Processing_LengthField, nibble_flags_ptr);
				UnSet_Bit(Processing_Length, nibble_flags_ptr);

				Length_Field_Size_Bytes = LengthField_Processing(input_nibble_str, Length_Field_Size_Bytes, nibble_flags_ptr);
				
				//Need to amend the function above, to pass pointer to Length value. All LengthField Processing should be done within the function. 

				if(Is_Bit_Set(Processing_Length, nibble_flags_ptr)){			

					Length = ( ASCIIHEX_to_DEC(input_nibble_cleaned) * 16);
					Reading_Status = 4;
					UnSet_Bit(Processing_LengthField, nibble_flags_ptr); 
					Set_Bit(Processing_Length, nibble_flags_ptr);

					//Length Field Processing Complete. Realloc the TLV_Block Buffer to a size where Length Can be Written.
					Temp = realloc(TLV_Block, (( (strlen(TLV_Block) + strlen("\tLength: ") + (Length_Field_Size_Bytes * 2) + 1) * sizeof(char) )) );
					
					if (Temp != NULL) {
						TLV_Block = Temp;
					}
					else {
						printf("Reallocation Failed!");
						Reading_Status = 6;
					}

					strcat(TLV_Block, "\tLength: ");
					Temp_Buffer = realloc( Temp_Buffer, ((Length_Field_Size_Bytes * 2) + 1));
					*Temp_Buffer = '\0';
					strcat(Temp_Buffer, input_nibble_str);
					Length_Field_Pos++;

				}

				else {

					//the first nibble of the Length Field is 8 or higher.
					Reading_Status = 3;

				}

				else {

					//the first nibble of the Length Field is 8 or higher.
					Reading_Status = 3;

				}

				//First nibble of Length Field has been determined, move to the second to complete length processing.

				break;

			case(3):
				//Still determining Size of Length Field. 
			
				Length_Field_Size_Bytes = LengthField_Processing(input_nibble_str, Length_Field_Size_Bytes, nibble_flags_ptr);
				printf("Length Field Size %d\n", Length_Field_Size_Bytes);

				if (Is_Bit_Set(Processing_Length, nibble_flags_ptr)) {
					//Length Field Processing Complete, Reallocate the TLV_Block Buffer so it can be prepared to write the length Value.
					Temp = realloc(TLV_Block, (((strlen(TLV_Block) + (Length_Field_Size_Bytes * 2) + strlen("\tLength: ") + 1) * sizeof(char))));
					
					if (Temp != NULL) {
						TLV_Block = Temp;
						strcat(TLV_Block, "\tLength: ");
						Temp_Buffer = realloc( Temp_Buffer, ((Length_Field_Size_Bytes * 2) + 1) *sizeof(char));
						*Temp_Buffer = '\0';
					}
					else {
						printf("Reallocation Failed!");
						Reading_Status = 6;
					}
				}
				else {
				}
				
				break;

			case(4):

				//Determining Size of the Length Value.
					
				if (Temp_Buffer != NULL){

					if(Length_Field_Pos < (Length_Field_Size_Bytes * 2) ){

						strcat(Temp_Buffer, input_nibble_str);

					}

					else{
						
						if (Length_Field_Pos == (Length_Field_Size_Bytes* 2)){

							Temp = realloc(TLV_Block, (((strlen(TLV_Block) + strlen(Temp_Buffer) + strlen("\n\tValue: ") + strlen(input_nibble_str) )* sizeof(char))));
							
							if (Temp != NULL){
								TLV_Block = Temp;
								strcat(TLV_Block, Temp_Buffer);
								strcat(TLV_Block, "\n\tValue: ");
								strcat(TLV_Block, input_nibble_str);
								Length = Length_Processing(Temp_Buffer, nibble_flags_ptr, Length_Field_Size_Bytes);
								UnSet_Bit(Processing_Length, nibble_flags_ptr);
								Set_Bit(Processing_Value, nibble_flags_ptr);
								Length_Field_Pos = 0;
							}
							else{
								printf("Failed to realloc memory\n");
								printf("Exiting...\n");
								return 0;
							}
						}
					}
				}

				else{
					printf("Failed to realloc memory\n");
					printf("Exiting...\n");
					return 0;
				}

				Length_Field_Pos++;

				break;

			
			case(5):
				//Writing the Value data, using the Length Value determined.

				//Length_Field_Pos variable is now used to track the position of the current value Nibble being read.
				if (Length_Field_Pos <= ((Length*2))) {

					strcat(TLV_Block, input_nibble_str);
					printf("Wrote %c to %s\n", input_nibble_cleaned, TLV_Block);
					printf("Result: %s\n", TLV_Block);
					Length_Field_Pos++;

					if (Length_Field_Pos > (Length)) {
						Length = 0;
					}

				}

				else {
				}

				break;

			case(6):

				if (Invalid_Data_Found == 0) {
					printf("Invalid Data Found!\n");
					printf("Creating Output Buffer.\nInvalid data position is marked as \"#\" in output).\n");
					Invalid_Data_Found = 1;
					Output = malloc(sizeof(char) * (file_size + 1));
					Temp = Output;

					for (y = 0; y < file_pos; y++) {
						*Temp = source_file_arr[y];
						Temp++;
					}

					*Temp = input_nibble_cleaned;
					Temp++;
					*Temp = '\0';


					if (TLV_Block != NULL) {
						free(TLV_Block);
					}

				}

				else {
				}
				
				strcat(Output, input_nibble_str);

				break;

			default: 
				printf("Unhandled State.\n");
				break;
		};

		printf("Determining Reading Status After Nibble Read...\n");
		Reading_Status = Determine_Reading_Status(nibble_flags_ptr, Invalid_Data_Found);
		printf("Reading_Status End of Nibble Read : \n");
		Debug_ReadingStatus(nibble_flags_ptr, Reading_Status);
		printf("Moving to Next Nibble...\n\n\n");

	}

	printf("OUTPUTS:\n\n");
	
	free(source_file_arr);
	fclose(source_file_fp);

	if (Reading_Status != 6) {
		
		if (Output == NULL) {
			if (TLV_Block != NULL) {
				Output = malloc((strlen(TLV_Block) + strlen("\n\n") + 1) * sizeof(char));
				strcpy(Output, TLV_Block);
				strcat(Output, "\n\n");
				TLV_Block = NULL;
				free(TLV_Block);
			}
			else {
				printf("WTFFF!\n");
			}
		}

		else {

			if (TLV_Block != NULL || Temp_Buffer != NULL || Tag_Def != NULL) {

				printf("End of File reached before last TLV Block could be established.\n");

				if (TLV_Block != NULL) {
					Output = TLV_Block_to_Output(Output, TLV_Block);
				}

				else {

					if (Temp_Buffer != NULL) {
						Temp = NULL;
						Temp = realloc(Output, (strlen(Output) + strlen(Temp_Buffer) + 1));
						if (Temp != NULL) {
							Output = Temp;
							strcat(Output, Temp_Buffer);
							Temp_Buffer = NULL;
							Temp = NULL;
						}
						else {
							printf("Failed Memory Realloc.\n");
							printf("Exiting...\n");
						}

					}

					else {
						Temp = NULL;
						Temp = realloc(Output, (strlen(Output) + strlen(Tag_Def) + 1));
						if (Temp != NULL) {
							Output = Temp;
							strcat(Output, Tag_Def);
							Tag_Def = NULL;
							Temp = NULL;
						}
						else {
							printf("Failed Memory Realloc.\n");
							printf("Exiting...\n");
						}
					}
				}
			

				UnSet_Bit(Processing_Value, nibble_flags_ptr);
				Set_Bit(Processing_Tag, nibble_flags_ptr);
				Temp_Buffer = NULL;
				TLV_Block = NULL;
				Length_Field_Pos = 0;
				Length = 0;
				
			}

			else {

				printf("End of File reached before last TLV Block could be established.\n");
				Temp = realloc(TLV_Block, (strlen("Unformatted Output:\t") + (strlen(TLV_Block) + strlen(Output) + 1)));

				if (Temp != NULL) {
					TLV_Block = Temp;
					Temp = NULL;
					strcat(Output, "Unformatted Output:\t");
					strcat(Output, TLV_Block);
					TLV_Block = NULL;
				}
				else {
					printf("Failed to Realloc Memory\n");
					return 0;
				}
			}


			if ((TLV_Block_pos == file_size)) {
				printf("Perfect Read!\n");
			}
			else {
				printf("Dump Completed!\n");
			}


		}

		printf("Output: %s\n\n", Output);
	}
	else {
		printf("Invalid Data Triggered: %d\n", Invalid_Data_Found);
		printf("Output:\t %s\n\n", Output);
	}

	printf("Exiting...");

	return 0;
}
