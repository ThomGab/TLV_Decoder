#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Bits_n_Bytes.h"
#include "Tag_List.h"
#include "File_Loading.h"

int main(int argc, char* argv[]) {


	//Source_File Variables
	FILE * source_file_fp = NULL;
	char * source_file_arr = NULL;
	int file_size = 0;
	unsigned int current_file_pos = 0; // where in the file is in the current Nibble?
	int TLV_Block_pos = 0; // where in the file has the TLV Data been printed up until?

	//Parsing Variables

	unsigned int First_Nibble_In_Byte;
	unsigned int * First_Nibble_In_Byte_ptr = &First_Nibble_In_Byte;

	unsigned int TagField_Size_Bytes;
	unsigned int * TagField_Size_Bytes_ptr = &TagField_Size_Bytes;

	unsigned int nibble_flags = 0; // Initialising All Processing Flags 0000 0000
	unsigned int * nibble_flags_ptr = &nibble_flags;

	int Reading_Status = 0;

	int Length = 0; //This is the length of the value field element.
	int Length_Field_Size_Bytes = 0;
	int Length_Field_Pos = 0;
	int* Length_Field_Pos_ptr = &Length_Field_Pos;

	int Invalid_Data_Found = 0;
	int Resized_Array_Complete = 0;
	unsigned int y = 0;
	int ReadyforNextNibble = 0;

	char input_nibble_cleaned;
	char input_nibble_str[2];

	int Depth = 0;

	//Buffers
	char* Temp_Buffer = NULL;
	TLV_Block* Head_TLV_Block = NULL;
	TLV_Block* Active_TLV_Block = NULL;
	TLV_Block* Active_Parent_TLV_Block = NULL;
	TLV_Block* Previous_TLV_Block = NULL;
	char * Output = NULL;
	char * Temp;

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
			Invalid_Data_Found = 1;
		}

		ReadyforNextNibble = 0;

		while (ReadyforNextNibble == 0) {
			switch (Reading_Status) {

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
				ReadyforNextNibble = 1;

				break;

				case(2):
				if (Temp_Buffer != NULL) {

					//Are there any existing TLV Blocks?
					if (Active_TLV_Block == NULL) {
						//This is the first TLV Block, and therefore is the head of the linked list. (Head_TLV_Block).
						Active_TLV_Block = Create_New_TLV_Block(Depth, Head_TLV_Block);
						Head_TLV_Block = Active_TLV_Block;
						Active_TLV_Block->Head = NULL;
					}

					else {
						//Are there any previous TLV Blocks?
						if (Previous_TLV_Block == NULL) {
							//The Active TLV Block is the First TLV Block, there is nothing to inherit characteristics from.
							Previous_TLV_Block = Active_TLV_Block;
							//Active_TLV_Block = Create_New_TLV_Block(Depth, Head_TLV_Block);
						}
					}

					if ((Active_TLV_Block != NULL) && (Previous_TLV_Block != NULL)) {

						if (Active_TLV_Block != Previous_TLV_Block) {
							Previous_TLV_Block = Active_TLV_Block;
							Active_TLV_Block = NULL;
						}

						//An Active TLV Block exists, and a previous TLV Block. there could be characterisitcs that have to be inherited from the previous block.
						//Checking for constructed object Nesting/Inheritance.
						//Is the Active_TLV_Block within the constructed template of the previous TLV Block?
						if ((Previous_TLV_Block->Constructed == 1)) {
							if (current_file_pos <= Previous_TLV_Block->FilePos_at_EndofBlock) {
								//The Active TLV_Block is within the Constructed Object range of the previous block. Therefore has the previous TLV Block as it's Parent.
								Depth = Depth + 1;
								Active_Parent_TLV_Block = Previous_TLV_Block;
								Active_TLV_Block = Create_New_TLV_Block(Depth, Head_TLV_Block);
								Previous_TLV_Block->Child = Active_TLV_Block;
								Active_TLV_Block->Parent = Active_Parent_TLV_Block;
							}
							else {
								//The Active TLV_Block is outside the range of the previous constructed TLV_Block. Are there other constructed objects that it is a part of?
								Depth = Active_Parent_TLV_Block->Depth;
								while (current_file_pos > Active_Parent_TLV_Block->FilePos_at_EndofBlock) {
									if (Active_Parent_TLV_Block->Parent != NULL) {
										//If another parent exists, and the Parent to the Active block has not yet been found, move to the next parent.
										Active_Parent_TLV_Block = Active_Parent_TLV_Block->Parent;

										//Is the Active TLV Block within the constructed range of this object?
										if (current_file_pos <= Active_Parent_TLV_Block->FilePos_at_EndofBlock) {
											//if it is, then this parent is a constructed data object that contains the active TLV Block 
											Active_TLV_Block->Parent = Active_Parent_TLV_Block;
										}
									}
									else {
										//There is no more parents to move to, this TLV_Block must be on the same level as the Head TLV Block (0th level constructed data object).
										Active_Parent_TLV_Block = NULL;
										Depth = 0;
										Head_TLV_Block->Next = Active_TLV_Block;
										Active_TLV_Block->Previous = Head_TLV_Block;
										Head_TLV_Block = Active_TLV_Block;
										break;
									}
								}
							}
						}

						else {
							//The Previous TLV_Block was a Primitive Object, are we within any existing constructed/parent objects?
							if (Active_Parent_TLV_Block != NULL) {
								if (current_file_pos <= Active_Parent_TLV_Block->FilePos_at_EndofBlock) {
									//These two TLV Blocks share the same Parent TLV_Block.
									Active_TLV_Block = Create_New_TLV_Block(Depth, Head_TLV_Block);
									Active_TLV_Block->Parent = Active_Parent_TLV_Block;
									Previous_TLV_Block->Next = Active_TLV_Block;
									Active_TLV_Block->Previous = Previous_TLV_Block;
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
												//if it does have a child, then find the next->Null of that child, and insert the Active_Block to indicate a shared constructed level.
												while ((Active_Parent_TLV_Block->Child)->Next != NULL) {
													Active_TLV_Block->Previous = (Active_Parent_TLV_Block->Child)->Next;
												}

												Active_TLV_Block->Previous = (Active_Parent_TLV_Block->Child)->Next;
												(Active_Parent_TLV_Block->Child)->Next = Active_TLV_Block;
												break;
											}
										}
									}

									//if no parent has been found, this is a header level primiive
									if (Active_Parent_TLV_Block->Parent == NULL) {

									}
								}
							}
							//The Active_Parent_TLV_Block is NULL, this Active_TLV_Block is not contained by any Constructed Data objects, must be a Headblock.
							else {

								Active_TLV_Block->Next = Create_New_TLV_Block(Depth, Head_TLV_Block);
								Active_TLV_Block = Active_TLV_Block->Next;
								Active_TLV_Block->Previous = Previous_TLV_Block;
								Head_TLV_Block = Active_TLV_Block;
							}
						}

					}

					else {
						printf("Narda\n.");
					}
				}

				else{
							printf("Temp Buffer is equal to NULL!\n");
							printf("Exiting...\n");
							return 0;
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
			Reading_Status = 3;
			ReadyforNextNibble = 0;

			//Need to amend the function above, to pass pointer to Length value. All LengthField Processing should be done within the function. 
			break;

			//Determining Length_Field_Size
			case(3):
				Length_Field_Size_Bytes = LengthField_Processing(input_nibble_str, Length_Field_Size_Bytes, nibble_flags_ptr);

				if (Is_Bit_Set(Processing_Length, nibble_flags_ptr)) {

					ReadyforNextNibble = 0;
					Reading_Status = 4;

				}

				else {

					//the first nibble of the Length Field is 8 or higher, indicating a length field coded over multiple bytes. Move to the next Nibble to determine how much.
					Length_Field_Pos++;
					ReadyforNextNibble = 1;
					Reading_Status = 3;

				}

				break;

			case(4):

				//Length Field Processing Complete. Realloc the Output_String_Replace_me Buffer to a size where Length Can be Written.
				if (Temp_Buffer == NULL){
					Temp_Buffer = malloc(((Length_Field_Size_Bytes * 2) + 1) * sizeof(char));

					if (Temp_Buffer != NULL) {
						*Temp_Buffer = '\0';
					}
					else {
						printf("Failed to alloc Memory.\n");
						printf("Exiting...\n");
						return 0;
					}

				}

				//Determining Size of the Length Value.
				else {

					if (Length_Field_Pos < (Length_Field_Size_Bytes * 2)) {

						strcat(Temp_Buffer, input_nibble_str);
						Length_Field_Pos++;
						ReadyforNextNibble = 1;

					}

					else {

						if (Length_Field_Pos == (Length_Field_Size_Bytes * 2)) {

							if (Temp_Buffer != NULL) {
								Active_TLV_Block->Length = malloc((sizeof(char) * (strlen(Temp_Buffer) + 1)));

								if (Active_TLV_Block->Length != NULL) {

									strcpy(Active_TLV_Block->Length, Temp_Buffer);
									Length = Length_Processing(Temp_Buffer, nibble_flags_ptr, Length_Field_Size_Bytes);
									Active_TLV_Block->FilePos_at_EndofBlock = (current_file_pos + Length) - 1;

									if (Is_Bit_Set(Processing_Constructed_Data_Object, nibble_flags_ptr)) {
										UnSet_Bit(Processing_Length, nibble_flags_ptr);
										Set_Bit(Processing_Tag, nibble_flags_ptr);
										Reading_Status = 1;
										Length_Field_Pos = 0;
										Length_Field_Size_Bytes = 0;
										Length = 0;
										ReadyforNextNibble = 0;
									}
									else {
										UnSet_Bit(Processing_Length, nibble_flags_ptr);
										Set_Bit(Processing_Value, nibble_flags_ptr);
										Reading_Status = 5;
										ReadyforNextNibble = 0;
									}

									Length_Field_Pos = 0;

									free(Temp_Buffer);
									Temp_Buffer = NULL;

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

			break;


			case(5):
				//Writing the Value data, using the Length Value determined.

				if (Temp_Buffer == NULL) {

					Temp_Buffer = malloc(sizeof(char) * (Length + 1));

					if (Temp_Buffer != NULL) {
						*Temp_Buffer = '\0';
					}

					else {

						printf("Failed to allocate for Value writing after Length written.\n");
						printf("Exiting...\n");
						return 0;

					}
				}

				//Length_Field_Pos variable is now used to track the position of the current value Nibble being read.
				if (Length_Field_Pos < (Length)) {

					strcat(Temp_Buffer, input_nibble_str);
					Length_Field_Pos++;
					ReadyforNextNibble = 1;

					if (Length_Field_Pos == (Length)) {
						UnSet_Bit(Processing_Value, nibble_flags_ptr);

						Active_TLV_Block->Value = malloc(sizeof(char) * (Length + 1));
						//Writing to Active_TLV_Block
						if (Active_TLV_Block->Value != NULL) {

							*(Active_TLV_Block->Value) = '\0';
							strcat(Active_TLV_Block->Value, Temp_Buffer);
							free(Temp_Buffer);
							Temp_Buffer = NULL;

						}

						else {

							printf("Failed to Allocate ACTIVE_TLV_BLOCK->VALUE.\n");
							return 0;

						}
						Debug_ReadingStatus(nibble_flags_ptr, Reading_Status);
						//Need to add in Constructed Data Processing flags here. 

						Length_Field_Pos = 0;
						Length_Field_Size_Bytes = 0;
						Length = 0;

					}

				}

				else {
				}

				break;

			case(6):
				if (y == 0) {

					if (Output == NULL) {
						Output = malloc((strlen(source_file_arr) + 1) * sizeof(char));
					}

					else {
						free(Output);
						Output = NULL;
						Output = malloc((strlen(source_file_arr) + 1) * sizeof(char));
					}

					for (y = 0; y < current_file_pos; y++) {
						Output[y] = source_file_arr[y];
					}

					Output[y] = '\0';
					strcat(Output, input_nibble_str);
					
				}
				else {
					if (input_nibble_cleaned == '#') {
						strcat(Output, "#");
					}
					else {
						strcat(Output, input_nibble_str);
						printf("%s", Output);
					}
				}

				ReadyforNextNibble = 1;

				break;

			default:
				printf("Unhandled State.\n");
				break;
			}
		}

		printf("Determining Reading Status After Nibble Read...\n");
		Reading_Status = Determine_Reading_Status(nibble_flags_ptr, Invalid_Data_Found);
		printf("Reading_Status End of Nibble Read : \n");
		Debug_ReadingStatus(nibble_flags_ptr, Reading_Status);
		printf("Moving to Next Nibble...\n\n\n");

	}

	printf("OUTPUTS:\n\n");

	if (Invalid_Data_Found == 1) {
		printf("Invalid Data Found, Invalid characters marked as ""#""\n\n");
		printf("%s\n\n", Output);
		printf("Exiting...");
	}

	else {
		Output[0] = '\0';
		Output = Print_Output(Active_TLV_Block, Output);
		printf("%s", Output);
	}

	free(source_file_arr);
	fclose(source_file_fp);

	printf("Exiting...");

	return 0;
}
