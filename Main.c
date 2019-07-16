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
	char * Output_String_Replace_me = NULL; 
	TLV_Block * Head_TLV_Block = NULL;
	TLV_Block * Active_TLV_Block = NULL;
	char * Output = NULL;
	char * Temp;

	//Output_String_Replace_me Element

	
	

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

		/*if (Length_Field_Pos > ((Length * 2)) && (Length != 0) && !Is_Bit_Set(Processing_Tag,nibble_flags_ptr)) {
			printf("Value Processing Complete.\n");
			Output = TLV_Block_to_Output(Output, Output_String_Replace_me);
			TLV_Block_pos = file_pos;
			Temp_Buffer = NULL;
			Output_String_Replace_me = NULL;
			Length_Field_Pos = 0;
			Length = 0;
			Debug_ReadingStatus(nibble_flags_ptr, Reading_Status);

		}*/

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
					//Setting up new Output_String_Replace_me

				}
									
				else{
					//If B5 and B4 of the first Byte of the Tag suggest a 2 Byte Tag, we have to expand the tag buffer.
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
				strcat(Temp_Buffer, input_nibble_str);

				break;

			case(2):
				printf("Tag recieved, Beginning Lookup %s\n\n", Temp_Buffer);
				
				Tag_Def = Find_Tag_Def(Tag_Def, Temp_Buffer, TagList);
				Temp_Buffer = realloc(Temp_Buffer, sizeof(char) * (strlen(Temp_Buffer) + strlen(Tag_Def) + 1));

				if (Temp_Buffer != NULL) {

					Temp_Buffer = strcat(Temp_Buffer, Tag_Def);

					//Initialising New TLV_Block Object.
					if (Active_TLV_Block == NULL) {
						if (Head_TLV_Block == NULL) {
							//If there is no Head_TLV_Block, the newly created one must be a Head_TLV_Block (BaseBlock).
							Active_TLV_Block = Create_New_TLV_Block();
							Head_TLV_Block = Active_TLV_Block;
						}

						else {
							Active_TLV_Block = Create_New_TLV_Block();
							Active_TLV_Block->Head = Head_TLV_Block;
						}
					}
					else {
						printf("Shouldn't be here\n.");
					}
					//Copying Temp into the active TLV Block:

					Active_TLV_Block->Tag = malloc( (sizeof(char) * (strlen(Temp_Buffer) + 1)));
					if (Active_TLV_Block->Tag != NULL) {
						strcpy(Active_TLV_Block->Tag, Temp_Buffer);
						free(Temp_Buffer);
						Temp_Buffer = NULL;
					}
					else{
						printf("Failed to alloc memory.\n");
					}
				}
	
				Set_Bit(Processing_LengthField, nibble_flags_ptr);
				Length_Field_Size_Bytes = LengthField_Processing(input_nibble_str, Length_Field_Size_Bytes, nibble_flags_ptr);
				
				//Need to amend the function above, to pass pointer to Length value. All LengthField Processing should be done within the function. 

				if(Is_Bit_Set(Processing_Length, nibble_flags_ptr)){			

					Length = ( ASCIIHEX_to_DEC(input_nibble_cleaned) * 16);
					Reading_Status = 4;
					UnSet_Bit(Processing_LengthField, nibble_flags_ptr); 
					Set_Bit(Processing_Length, nibble_flags_ptr);

					//Length Field Processing Complete. Realloc the Output_String_Replace_me Buffer to a size where Length Can be Written.
					Temp_Buffer = malloc( ( (strlen("\tLength: ")) + (Length_Field_Size_Bytes * 2) + 1) * sizeof(char) );
					
					if (Temp_Buffer != NULL) {
						strcat(Temp_Buffer, "\tLength: ");
						strcat(Temp_Buffer, input_nibble_str);
					}
					else {
						printf("Allocation Failed!");
						Reading_Status = 6;
					}

					Length_Field_Pos++;

				}

				else {

					//the first nibble of the Length Field is 8 or higher, indicating a length field coded over multiple bytes.
					Reading_Status = 3;

				}
				//First nibble of Length Field has been determined, move to the second to complete length processing.

				break;

			case(3):
				//Still determining Size of Length Field. 
			
				Length_Field_Size_Bytes = LengthField_Processing(input_nibble_str, Length_Field_Size_Bytes, nibble_flags_ptr);

				if (Is_Bit_Set(Processing_Length, nibble_flags_ptr)) {
					//Length Field Processing Complete, Reallocate the Output_String_Replace_me Buffer so it can be prepared to write the length Value.
					
					if (Temp_Buffer != NULL) {
						Temp_Buffer = realloc(Temp_Buffer, (strlen(Temp_Buffer) + (Length_Field_Size_Bytes * 2) + 1) * sizeof(char));

						if (Temp_Buffer != NULL) {
							strcat(Temp_Buffer, input_nibble_str);
						}
						else {
							printf("Reallocation Failed!");
							Reading_Status = 6;
						}


					}
					else {
						Temp_Buffer = malloc( (strlen("\tLength :") + (Length_Field_Size_Bytes * 2) + 1) * sizeof(char) );

						if (Temp_Buffer != NULL) {
							*Temp_Buffer = '\0';
							strcat(Temp_Buffer, "\tLength: ");
						}
						else {
							printf("Reallocation Failed!");
							Reading_Status = 6;
						}

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

							if (Temp != NULL){
								Output_String_Replace_me = Temp;
								strcat(Output_String_Replace_me, Temp_Buffer);

								//Constructed Data Object Processing here.

								strcat(Output_String_Replace_me, "\n\tValue: ");
								strcat(Output_String_Replace_me, input_nibble_str);
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
				if (Length_Field_Pos < (Length) ) {

					strcat(Output_String_Replace_me, input_nibble_str);
					printf("Wrote %c to %s\n", input_nibble_cleaned, Output_String_Replace_me);
					printf("Result: %s\n", Output_String_Replace_me);
					Length_Field_Pos++;

					if (Length_Field_Pos == (Length)) {
						UnSet_Bit(Processing_Value, nibble_flags_ptr);
						Length_Field_Pos = 0;
						TLV_Block_pos = file_pos;
						Output = TLV_Block_to_Output(Output, Output_String_Replace_me);
						Temp_Buffer = NULL;
						Output_String_Replace_me = NULL;
						Length_Field_Pos = 0;
						Length_Field_Size_Bytes = 0;
						Length = 0;
						Debug_ReadingStatus(nibble_flags_ptr, Reading_Status);
						//Need to add in Constructed Data Processing flags here. 
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


					if (Output_String_Replace_me != NULL) {
						free(Output_String_Replace_me);
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
			if (Output_String_Replace_me != NULL) {
				Output = malloc((strlen(Output_String_Replace_me) + strlen("\n\n") + 1) * sizeof(char));
				strcpy(Output, Output_String_Replace_me);
				strcat(Output, "\n\n");
				Output_String_Replace_me = NULL;
				free(Output_String_Replace_me);
			}
			else {
				printf("WTFFF!\n");
			}
		}

		else {

			if (Output_String_Replace_me != NULL || Temp_Buffer != NULL || Tag_Def != NULL) {

				printf("End of File reached before last TLV Block could be established.\n");

				if (Output_String_Replace_me != NULL) {
					Output = TLV_Block_to_Output(Output, Output_String_Replace_me);
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
				Output_String_Replace_me = NULL;
				Length_Field_Pos = 0;
				Length = 0;
				
			}

			else {

				if ((TLV_Block_pos + 1 == file_size)) {
					printf("Perfect Read!\n");
				}
				else {
					printf("Dump Completed!\n");
				}

				
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
