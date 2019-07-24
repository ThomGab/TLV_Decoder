#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Bits_n_Bytes.h"
#include "Tag_List.h"
#include "File_Loading.h"

int main(int argc, char* argv[]) {


	//Source_File Variables
	FILE* source_file_fp = NULL;
	char* source_file_arr = NULL;
	int file_size = 0;
	unsigned int current_file_pos = 0; // where in the file is in the current Nibble?
	int TLV_Block_pos = 0; // where in the file has the TLV Data been printed up until?

	//Parsing Variables

	unsigned int First_Nibble_In_Byte;
	unsigned int* First_Nibble_In_Byte_ptr = &First_Nibble_In_Byte;

	unsigned int TagField_Size_Bytes;
	unsigned int* TagField_Size_Bytes_ptr = &TagField_Size_Bytes;

	unsigned int nibble_flags = 0; // Initialising All Processing Flags 0000 0000
	unsigned int* nibble_flags_ptr = &nibble_flags;

	int Reading_Status = 0;

	int Length = 0; //This is the length of the value field element.
	int Length_Field_Size_Bytes = 0;
	int Length_Field_Pos = 0;
	int* Length_Field_Pos_ptr = &Length_Field_Pos;

	int Invalid_Data_Found = 0;
	int Resized_Array_Complete = 0;
	unsigned int y = 0;

	char input_nibble_cleaned;
	char input_nibble_str[2];

	//Buffers
	char* Temp_Buffer = NULL;
	char* Output_String_Replace_me = NULL;
	TLV_Block* Head_TLV_Block = NULL;
	TLV_Block* Active_TLV_Block = NULL;
	TLV_Block* Active_Parent_TLV_Block = NULL;
	TLV_Block* Previous_TLV_Block = NULL;
	char* Output = NULL;
	char* Temp;

	//Verify Input File and Establish it's size in Nibbles.
	source_file_fp = verify_inputs(argc, argv);
	if (source_file_fp == NULL) {
		printf("Exiting...\n");
		return 0;
	}
	else {
		file_size = get_file_size(source_file_fp); //Null Terminating Character required.
	}

	//Create and write the file content to an array, close the file.
	source_file_arr = malloc((file_size + 1) * sizeof(char));

	if (source_file_arr == NULL) {
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

	for (current_file_pos = 0; source_file_arr[current_file_pos] != '\0'; current_file_pos++) {

		if (current_file_pos != 0) {
			if (Is_Bit_Set(Processing_First_Nibble, nibble_flags_ptr)) {
				UnSet_Bit(Processing_First_Nibble, nibble_flags_ptr);
			}
			else {
				Set_Bit(Processing_First_Nibble, nibble_flags_ptr);
			}

		}

		printf("Reading Status Pre_Nibble Read\n");
		Debug_ReadingStatus(nibble_flags_ptr, Reading_Status);
		input_nibble_cleaned = Clean_Input((source_file_arr[current_file_pos]));

		input_nibble_str[0] = input_nibble_cleaned;
		input_nibble_str[1] = '\0';

		printf("INPUT CHARACTER IS CURRENTLY: %c\n", input_nibble_cleaned);
		printf("IS IT FIRST NIBBLE? %d\n\n", Is_Bit_Set(Processing_First_Nibble, nibble_flags_ptr));

		//Invalid Data Check
		if ((input_nibble_cleaned == '#') || (Invalid_Data_Found == 1)) {
			Reading_Status = 6;
		}


		switch (Reading_Status) {

			// Tag Not Completely Read, still evaluting Tag Value nibble by nibble. 
		case(1):

			//Setting up Tag Buffer.
			if ((Temp_Buffer == NULL)) {
				Temp_Buffer = malloc(((sizeof * Temp_Buffer) * ((TagField_Size_Bytes * 2) + 1)));

				if (Temp_Buffer != NULL) {
					*Temp_Buffer = '\0';
				}

				else {
					printf("Failed to allocate Memory!\n");
					printf("Exiting like a champ...\n");
					return 0;
				}


			}

			else {
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

			Tag_Processing(input_nibble_cleaned, nibble_flags_ptr, TagField_Size_Bytes_ptr);
			printf("\nReading Status After Tag Processing\n");
			Debug_ReadingStatus(nibble_flags_ptr, Reading_Status);
			strcat(Temp_Buffer, input_nibble_str);

			break;

		case(2):
			if (Temp_Buffer != NULL) {

				//Initialising New TLV_Block Object.
				if (Active_TLV_Block == NULL) {
					//If this is the first TLV Block, the newly created one must be the first Head_TLV_Block (BaseBlock).
					Head_TLV_Block = Create_New_TLV_Block();
					Active_TLV_Block = Head_TLV_Block;
				}

				else {
					//This isn't the first TLV_Block. Should the new one inherit any characteristics from the previous one?
					if (Previous_TLV_Block == NULL) {
						Previous_TLV_Block = Active_TLV_Block;
						Active_TLV_Block = Create_New_TLV_Block();
					}

					else {
						//is the Active_TLV_Block within the constructed template of the previous TLV Block?
						if ((Previous_TLV_Block->Constructed == 1)) {
							if (current_file_pos <= Previous_TLV_Block->FilePos_at_EndofBlock) {
								//The Active TLV_Block has the previous TLV Block as it's Parent.
								Active_Parent_TLV_Block = Previous_TLV_Block;
								Previous_TLV_Block->Child = Active_TLV_Block;
								Active_TLV_Block->Parent = Previous_TLV_Block;
							}
							else {
								//The Active TLV_Block is outside the range of the previous constructed TLV_Block. It must inherit a Parent of a previous block.
								while (current_file_pos > Active_Parent_TLV_Block->FilePos_at_EndofBlock) {
									if (Active_Parent_TLV_Block->Parent != NULL) {
										//If another parent exists, and the Parent to the Active block has not yet been found, move to the next parent.
										Active_Parent_TLV_Block = Active_Parent_TLV_Block->Parent;
									}
									else {
										//There is no more parents to move to, this TLV_Block must be on the same level as the Head TLV Block (0th level constructed data object).
										Active_Parent_TLV_Block = NULL;
										Head_TLV_Block->Next = Active_TLV_Block;
										Active_TLV_Block->Previous = Head_TLV_Block;
										Head_TLV_Block = Active_TLV_Block;
										break;
									}
								}

								if (Active_TLV_Block != Head_TLV_Block) {
									//The Active Parent Object has been found. Current_file_pos is not greater than Active_Parent_TLV_Block.
									Active_Parent_TLV_Block = Active_Parent_TLV_Block->Parent;
									Active_TLV_Block->Parent = Active_Parent_TLV_Block;
								}

								else {
									printf("We're still in the same constructed Object -> %s\n\n", Active_Parent_TLV_Block->Tag);
								}


							}

						}

						else {
							//Previous TLV_Block was a Primitive Object, are we still in range of the active_parent block?
							if (Active_Parent_TLV_Block != NULL) {
								if (current_file_pos <= Active_Parent_TLV_Block->FilePos_at_EndofBlock) {
									//we're still within this Parent TLV_Block.
									Active_TLV_Block->Parent = Active_Parent_TLV_Block;
								}
								else {
									//we're outside of this Parent TLV_Block. Are there other parent TLVBlocks/Constructed Data objects to inherit from?
									while (Active_Parent_TLV_Block->Parent != NULL) {
										Active_Parent_TLV_Block = Active_Parent_TLV_Block->Parent;
										if (current_file_pos > Active_Parent_TLV_Block->FilePos_at_EndofBlock) {
											//we're outside of the range of this constructed object, move to the next parent block
											Active_Parent_TLV_Block = Active_Parent_TLV_Block->Parent;
										}
										else {
											//we'ere inside the range of this constructed object, inherit the properties of this Active_Parent_Block.
											Active_TLV_Block->Parent = Active_Parent_TLV_Block;

											//Does this parent block have any existing children?
											if (Active_Parent_TLV_Block->Child == NULL) {
												Active_Parent_TLV_Block->Child = Active_TLV_Block;
												break;
											}

											else {
												//if it does have a child, then find the next->Null of that child, and insert the Active_Block to indicate a shared constructed level/
												while ((Active_Parent_TLV_Block->Child)->Next != NULL) {
													Active_TLV_Block->Previous = (Active_Parent_TLV_Block->Child)->Next;
												}

												Active_TLV_Block->Previous = (Active_Parent_TLV_Block->Child)->Next;
												(Active_Parent_TLV_Block->Child)->Next = Active_TLV_Block;
												break;
											}
										}
									}
								}
							}
						}

					}

				}

				//Writing Tag into TLV_Block newly constructed Active_TLV_Block:
				Active_TLV_Block->Tag = malloc(sizeof(Temp_Buffer));
				if ((Active_TLV_Block->Tag) != NULL) {
					strcpy((Active_TLV_Block->Tag), Temp_Buffer);
					free(Temp_Buffer);
					Temp_Buffer = NULL;
				}

				//Determining Corresponding Tag Definition and writing it to Active_TLV_BLock:
				Find_Tag_Def(&(Active_TLV_Block->Tag_Def), Active_TLV_Block->Tag, TagList);

				//Writing Constructed Data Object information to Active Tag_Block:
				Active_TLV_Block->Constructed = Is_Bit_Set(Processing_Constructed_Data_Object, nibble_flags_ptr);

				Set_Bit(Processing_LengthField, nibble_flags_ptr);
				Length_Field_Size_Bytes = LengthField_Processing(input_nibble_str, Length_Field_Size_Bytes, nibble_flags_ptr);

				//Need to amend the function above, to pass pointer to Length value. All LengthField Processing should be done within the function. 

				if (Is_Bit_Set(Processing_Length, nibble_flags_ptr)) {

					Length = (ASCIIHEX_to_DEC(input_nibble_cleaned) * 16);
					Reading_Status = 4;
					UnSet_Bit(Processing_LengthField, nibble_flags_ptr);
					Set_Bit(Processing_Length, nibble_flags_ptr);

					//Length Field Processing Complete. Realloc the Output_String_Replace_me Buffer to a size where Length Can be Written.
					Temp_Buffer = malloc(((Length_Field_Size_Bytes * 2) + 1) * sizeof(char));

					if (Temp_Buffer != NULL) {
						*Temp_Buffer = '\0';
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

			}

			else {
				printf("Temp Buffer is equal to NULL!\n");
				printf("Exiting...\n");
				return 0;
			}

			break;

		case(3):
			//Still determining Size of Length Field. 

			Length_Field_Size_Bytes = LengthField_Processing(input_nibble_str, Length_Field_Size_Bytes, nibble_flags_ptr);

			if (Is_Bit_Set(Processing_Length, nibble_flags_ptr)) {
				//Length Field Processing Complete, Reallocate the Output_String_Replace_me Buffer so it can be prepared to write the length Value.

				if (Temp_Buffer != NULL) {

					Temp_Buffer = realloc(Temp_Buffer, (strlen(Temp_Buffer) + (Length_Field_Size_Bytes * 2) + 1) * sizeof(char));

					if (Temp_Buffer != NULL) {
						*Temp_Buffer = '\0';
						strcat(Temp_Buffer, input_nibble_str);
					}
					else {
						printf("Reallocation Failed!");
						Reading_Status = 6;
					}


				}
				else {
					Temp_Buffer = malloc(((Length_Field_Size_Bytes * 2) + 1) * sizeof(char));

					if (Temp_Buffer != NULL) {
						*Temp_Buffer = '\0';
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

			if (Temp_Buffer != NULL) {

				if (Length_Field_Pos < (Length_Field_Size_Bytes * 2)) {

					strcat(Temp_Buffer, input_nibble_str);

				}

				else {

					if (Length_Field_Pos == (Length_Field_Size_Bytes * 2)) {

						if (Temp_Buffer != NULL) {
							Active_TLV_Block->Length = malloc((sizeof(char) * (strlen(Temp_Buffer) + 1)));

							if (Active_TLV_Block->Length != NULL) {

								strcpy(Active_TLV_Block->Length, Temp_Buffer);
								Length = Length_Processing(Temp_Buffer, nibble_flags_ptr, Length_Field_Size_Bytes);

								UnSet_Bit(Processing_Length, nibble_flags_ptr);
								Set_Bit(Processing_Value, nibble_flags_ptr);
								Active_TLV_Block->FilePos_at_EndofBlock = (current_file_pos + Length) - 1;
								Length_Field_Pos = 0;

								free(Temp_Buffer);
								Temp_Buffer = NULL;

								Temp_Buffer = malloc(sizeof(char) * (Length + 1));

								if (Temp_Buffer != NULL) {
									*Temp_Buffer = '\0';
									strcat(Temp_Buffer, input_nibble_str);
								}

								else {

									printf("Failed to allocate for Value writing after Length written.\n");
									printf("Exiting...\n");
									return 0;

								}

							}

							else {

								printf("Failed to allocate for Value writing after Length written.\n");
								printf("Exiting...\n");
								return 0;
							}

						}
						else {
							printf("Failed to realloc memory for Temp_Buffer\n");
							printf("Exiting...\n");
							return 0;
						}
					}
				}
			}

			else {
				printf("Failed to realloc memory\n");
				printf("Exiting...\n");
				return 0;
			}

			Length_Field_Pos++;

			break;


		case(5):
			//Writing the Value data, using the Length Value determined.

			//Length_Field_Pos variable is now used to track the position of the current value Nibble being read.
			if (Length_Field_Pos < (Length)) {

				strcat(Temp_Buffer, input_nibble_str);
				Length_Field_Pos++;

				if (Length_Field_Pos == (Length)) {
					UnSet_Bit(Processing_Value, nibble_flags_ptr);
					Length_Field_Pos = 0;
					Length_Field_Size_Bytes = 0;
					Length = 0;

					Active_TLV_Block->Value = malloc(sizeof(char) * (Length + 1));

					//Writing to Active_TLV_Block
					if (Active_TLV_Block->Value != NULL) {

						strcpy(Active_TLV_Block->Value, Temp_Buffer);
						free(Temp_Buffer);
						Temp_Buffer = NULL;

					}

					else {

						printf("Failed to Allocate ACTIVE_TLV_BLOCK->VALUE.\n");
						return 0;

					}

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

				if (Output == NULL) {
					printf("Error Allocating memory to Output...\n");
					printf("Exiting...\n");
					return 0;
				}

				Temp = Output;

				for (y = 0; y < current_file_pos; y++) {
					*Temp = source_file_arr[y];
					Temp++;
				}

				*Temp = input_nibble_cleaned;
				Temp++;
				*Temp = '\0';


				if (Output_String_Replace_me != NULL) {
					free(Output_String_Replace_me);
				}

				else {
				}

			}

			else {
			}

			strcat(Output, input_nibble_str);

			break;

		default:
			printf("Unhandled State.\n");
			break;
		}

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

			if (Output_String_Replace_me != NULL || Temp_Buffer != NULL) {

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
						Temp = realloc(Output, (strlen(Output) + 1));
						if (Temp != NULL) {
							Output = Temp;
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
