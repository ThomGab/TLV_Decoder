#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Bits_n_Bytes.h"
#include "TLV_Processing.h"
#include "Tag_List.h"
	
void Tag_Processing(char input_nibble_raw, unsigned int * nibble_flags, unsigned int * TagField_Size_Bytes){

	unsigned int flags = (*nibble_flags);
	unsigned int input_nibble = ASCIIHEX_to_DEC(input_nibble_raw);
	unsigned int * input_nibble_ptr = &input_nibble;
	
	//Confirm we're Processing a Tag Byte.
	if (Is_Bit_Set(Processing_Tag, nibble_flags) == 1) {

		//Are we processing a subsequent Byte of the Tag Field, or the inital Byte of the Tag Field?

		//Initial Byte
		if (Is_Bit_Set(Processing_Subsequent_Field, nibble_flags) == 0) {

			//Processing First Nibble in the Initial Byte.
			if ((Is_Bit_Set(Processing_First_Nibble, nibble_flags) == 1)) {

				//is b5 of initial Tag Byte Set (subsequent byte flag first nib)?
				if (Is_Bit_Set(N1_B1, input_nibble_ptr)) {
					Set_Bit(Processing_Subsequent_Field_FirstNibFlag, nibble_flags);
					printf("Set Firstnibble Subsequent Flags...\n");
				}
				else {
					UnSet_Bit(Processing_Subsequent_Field_FirstNibFlag, nibble_flags);
					*TagField_Size_Bytes = 1;
					printf("No More Subsequent Tag Bytes to Come.\n");
					printf("Write next Nibble and Finish Tag Processing.\n");
				}

				//is b6 of initial Tag Byte Set (Primitive or Constructed Object)?
				if (Is_Bit_Set(N1_B2, input_nibble_ptr)) {
					Set_Bit(Processing_Constructed_Data_Object, nibble_flags);
					printf("Constructed Data Object Detected\n");
					printf("Nibble_Flags Set %d\n", *nibble_flags);
				}
				else {
					UnSet_Bit(Processing_Constructed_Data_Object, nibble_flags);
					printf("Primitive Data Object Detected\n");
				}

			}


			//Processing Second Nibble in the Initial Byte.
			else {
				//Is the Subsequent Bytes Flag Set?

				if (Is_Bit_Set(Processing_Subsequent_Field_FirstNibFlag, nibble_flags)) {
					if (15 && *input_nibble_ptr) {
						Set_Bit(Processing_Subsequent_Field, nibble_flags);
						(*TagField_Size_Bytes)++;
					}
					else {
					}
				}
				else {
					*TagField_Size_Bytes = 1;
				}


				//just write the nibble value, might need to implement "(Part of) Tag Number" processing, p156 Annex B Book 3. Don't know what for.
			}
		}

		//Subsequent Tag Byte
		else {

			//Processing First Nibble in Subsequent Byte
			if (Is_Bit_Set(Processing_First_Nibble, nibble_flags)) {

				if (N1_B4 & input_nibble) {
					Set_Bit(Processing_Subsequent_Field_FirstNibFlag, nibble_flags);
					TagField_Size_Bytes++;
				}
				else {
					UnSet_Bit(Processing_Subsequent_Field_FirstNibFlag, nibble_flags);
				}
				

			}

			else {
			}

		}
	}

	else {
		printf("Reading Status Error, we are not processing a Tag!\n");
	}
	   	   
	printf("Tag_Size is %d\n", *TagField_Size_Bytes);
	
	return;	
}

int LengthField_Processing(char * input_nibble_str, int LengthFieldValue, unsigned int *nibble_flags){

	//Is the input_nibble a first nibble in a byte?

	int input_nibble_int = atoi(input_nibble_str);

	if( Is_Bit_Set(Processing_First_Nibble, nibble_flags) == 1 ){
		
		//Is the current nibble in the first byte of the Lengthfield?

		if( LengthFieldValue == 0 && (!Is_Bit_Set(Processing_Subsequent_Field,nibble_flags)) ){

			if( ( (N1_B4 & input_nibble_int) == 1) ) {
				//then it's a Multiple byte length field, the next nibble is the length value in Bytes.
				Set_Bit(Processing_Subsequent_Field, nibble_flags); // Using this as a Marker for Length Field Values with bit 8 set.
				//Value "7F" in first byte of length field translates to One Bye Length Field, with 127 BYTES in value field.
				//The current nibble needs to be passed to the length processing function, LengthField Processing has been completed at this point.
			}
			else{
				UnSet_Bit(Processing_LengthField, nibble_flags);
				Set_Bit(Processing_Length, nibble_flags);
				UnSet_Bit(Processing_Subsequent_Field, nibble_flags);
				LengthFieldValue = 1;
			}
		}

		//Your in first nibble, but there is a prexisting Length Field Value. (indicating a prior byte in the length field) eg 7F.
		else{

			printf("Unhandled LengthField Case reached.\n");

		}
	}
	
	else{

		//in A second Nibble of a Byte Field.
		LengthFieldValue = input_nibble_int;
		Set_Bit(Processing_Length, nibble_flags);
		UnSet_Bit(Processing_LengthField, nibble_flags);

	}

return LengthFieldValue;

}

int Length_Processing(char * Temp_Buffer, unsigned int *nibble_flags_ptr, int Length_Field_Size_Bytes){

	int i, raw_input, Length, shift;
	
	i = 0;
	raw_input = 0;
	Length = 0;
	shift = 0;

	for( i = 0; Temp_Buffer[i] != '\0'; i++){
		raw_input = ASCIIHEX_to_DEC(Temp_Buffer[i]);
		shift = ( 1 << ( ( ((Length_Field_Size_Bytes * 2) - i) - 1 ) * 4));
		Length = Length + (raw_input * shift); //Length in Bytes
	}

	return (Length * 2); // Length in Nibbles
}
	
	
void Value_Processing(char input_nibble, unsigned int *nibble_flags);


//Only Enters if the active TLV_Object is marked as "Constructed" in the nibble_flags.
/*void Constructed_Data_Object_Processing(unsigned int * nibble_flags_ptr, TLV_Block *  Active_TLV_Block, TLV_Block * Previous_TLV_Block) {

	//nibble_flags_ptr relate to the active TLV Block

	if (Active_TLV_Block->Head != NULL) {
		//the Active_TLV_Block is not a Head TLV_Block.

		if (Is_Bit_Set(Processing_Constructed_Data_Object, nibble_flags_ptr) {
			if (Previous_TLV_Block->Parent != NULL) {
				//The Active_TLV_Block is an unnested Constructed Object.
			}
			}
		}

		(Active_TLV_Block->Previous)->Child = Active_TLV_Block;
		Active_TLV_Block->Parent = Previous_TLV_Block;
	}

	else {
		//The Head_TLV_Block has an existing Constructed Data Object Child attached to it.
		
		//Does the
		if()
	}


}*/

char * Print_Output(TLV_Block * Final_TLV_Block_ptr, char * Output_ptr) {

	//Navigate to the first HeadBlock

	TLV_Block* Active_TLV_Block = Final_TLV_Block_ptr;
	unsigned int i = 0;
	int Finished = 0;
	char format_string[5];

	format_string[0] = '\0';

	while (Active_TLV_Block->Head != NULL) {
		Active_TLV_Block = Active_TLV_Block->Head;
	}

	//First Headblock reached, begin writing to output.
	while (Finished != 1) {

		if (i != Active_TLV_Block->Depth) {
			for (i = 0; i < Active_TLV_Block->Depth; i++) {
				format_string[i] = '\t';
			}
		}

		strcat(Output_ptr, format_string);
		strcat(Output_ptr, "Tag:");
		strcat(Output_ptr, Active_TLV_Block->Tag);
		strcat(Output_ptr, Active_TLV_Block->Tag_Def);
		strcat(Output_ptr, "\n");
		strcat(Output_ptr, format_string);
		strcat(Output_ptr, "Length:");
		strcat(Output_ptr, Active_TLV_Block->Length);
		strcat(Output_ptr, "\n");
		strcat(Output_ptr, format_string);
		
		if (Active_TLV_Block->Constructed == 1) {
			if (Active_TLV_Block->Child != NULL) {
				Active_TLV_Block = Active_TLV_Block->Child;
			}
			else {
				printf("Error, Constructed Object has no Children. Moving to next constructed Object instead.\n");
				if (Active_TLV_Block->Next != NULL) {
					Active_TLV_Block = Active_TLV_Block->Next;
				}

				else {
					printf("Error, Constructed Object has no Next. Moving to parent Object instead.\n");
					if (Active_TLV_Block->Parent != NULL) {
						Active_TLV_Block = Active_TLV_Block->Parent;
					}

					else {
						printf("File Processing Complete!\n");
						Finished = 1;
					}
				}
			}
		}
		else {
			strcat(Output_ptr, "Value:");
			strcat(Output_ptr, Active_TLV_Block->Value);
			strcat(Output_ptr, "\n");

			if ( Active_TLV_Block->Next != NULL ){
				Active_TLV_Block = Active_TLV_Block->Next;
			}
			else {
				if (Active_TLV_Block->Parent != NULL) {
					if ((Active_TLV_Block->Parent)->Next != NULL) {
						Active_TLV_Block = Active_TLV_Block->Parent;
					}
					else {
						printf("File Processing Compelete!\n");
						Finished = 1;
					}
				}
				else {
					printf("File Processing Compelete!\n");
					Finished = 1;
				}
			}
		}

		strcat(Output_ptr, "\n");
		
		memset(format_string, '\0', sizeof(format_string));
		i = 0;

	}

	return Output_ptr;
}

unsigned int ASCIIHEX_to_DEC(char c){

	if( (c <= 57) && (c >=48) ){
		return (c - 48);
	}
	else{
		if ( (c <= 90) && (c >= 65) ){
			return (c - 55);
		}
		else{
			return 43; //Hashtag, Marker for Invalid Data.
		}
	}
}

char Clean_Input(char c){
		
	if( ((c <= '9') && (c >= '0')) || ((c <= 'F') && (c >= 'A')) ){
		printf("Input in range %c\n", c);
		return c;
	}
	
	else{
		printf("Input out of range %c\n", c);
		return '#';
	}

}

void Find_Tag_Def(char ** TagDefOutput, char *SearchTag, Tag * InputList){
	
	int i, Tag_List_Size; 
	
	*TagDefOutput = NULL;
	
	Tag_List_Size = SIZE_TAG_LIST;
	//Tag_List_Size = sizeof(Tag_List)/sizeof(Tag);
	printf(" Size of Tag_List is %d\n\n", Tag_List_Size);

	for (i = 0; i < Tag_List_Size; i++){
	
		if(*TagDefOutput == NULL){
	
			if (strcmp( (InputList[i].BER_TLV), SearchTag) == 0){
				*TagDefOutput = malloc( sizeof(char) * ( (strlen(InputList[i].Definition)) + 1 ));
				if (*TagDefOutput != NULL) {
					*TagDefOutput = InputList[i].Definition;
					printf("Found Tag.\n\t: %s \n", InputList[i].BER_TLV);
					printf("Found Definition.\n\t: %s \n", *TagDefOutput);
					printf("Break point 1\n");
					break;
				}
				else {
					printf("Could not allocated memory for Tag_Def_Output.\n");
					printf("Exiting...\n");
					return;
				}
			}
			else{
			}
		}

	}

	if (*TagDefOutput == NULL) {
		*TagDefOutput = malloc(sizeof(char) * ( (strlen(" - Undefined Tag\n")) + 1));
		if (*TagDefOutput != NULL) {
			*TagDefOutput = " - Undefined Tag\n";
		}
		else {
			printf("Could not allocated memory for Tag_Def_Output.\n");
			printf("Exiting...\n");
			return ;
		}
	}

	else {
	}

	return;

}


int Determine_Reading_Status(unsigned int* nibble_flags, int Invalid_Data_Flag){

	int Reading_Status = 0;
	printf("Determining Reading Status...\n\n");
	
	if (Invalid_Data_Flag == 1) {
		Reading_Status = 6;
		return Reading_Status;
	}

	else {
		Reading_Status = 0;
	}
	   
	//Processing Tag Check.
	if (Is_Bit_Set(Processing_Tag, nibble_flags)) {

		//If these checks are satisfied, move to Length Processing
		
		//If we're Processing a 2nd Nibble
		if (! Is_Bit_Set(Processing_First_Nibble, nibble_flags) ) {

			//Does the first Nibble Indicate a Subsequent Field?
			if (! Is_Bit_Set(Processing_Subsequent_Field_FirstNibFlag, nibble_flags) ) {
				
				//Last Nibble In Byte, No Subsequent Field Marker in First Nibble= No more Tag Nibbles to Read.
				//Read and Write, then move to Length Processing.
				UnSet_Bit(Processing_Subsequent_Field, nibble_flags);
				UnSet_Bit(Processing_Tag, nibble_flags);
				Set_Bit(Processing_LengthField, nibble_flags);
				Reading_Status = 2;

			}
			else {
				//Last Nibble in Byte, With Subsequent Field Marker in First Nibble = More Tag Nibbles to Read.
				Reading_Status = 1;
			}

		}

		//We're processing the first Nibble of a Byte.
		else {
			//Is it in a Subsequent Field? Eg 9F "3" 7?
			if ( Is_Bit_Set(Processing_Subsequent_Field, nibble_flags) && Is_Bit_Set(Processing_Subsequent_Field_FirstNibFlag, nibble_flags) ) {
				Reading_Status = 1;
			}
			else {
				Reading_Status = 1;
			}
		}
	}

	else {
		//Processing Length Field Check
		if (Is_Bit_Set(Processing_LengthField, nibble_flags) == 1) {

			Reading_Status = 3;

		}


		else {

			//Processing Length Check.
			if ( (Is_Bit_Set(Processing_Length, nibble_flags) == 1) ) {

					printf("Processing_Length = %d\n", 1);
					Reading_Status = 4;
			}

			else {

				//Processing Value Check.
				if ((Is_Bit_Set(Processing_Value, nibble_flags)) == 1) {
					UnSet_Bit(Processing_Subsequent_Field, nibble_flags);
					printf("Processing Value.\n");
					Reading_Status = 5;
				}

				else {

					//Processing Value Complete check, First Nibble of Processing_nibbles flags should = 0.

					if ( ( !Is_Bit_Set(Processing_Value,nibble_flags) && !Is_Bit_Set(Processing_LengthField, nibble_flags) && !Is_Bit_Set(Processing_Length, nibble_flags) && !Is_Bit_Set(Processing_Value, nibble_flags) )) {

						printf("Processing Value Complete.\n");
						printf("Moving to Next Tag\n");
						Set_Bit(Processing_Tag, nibble_flags);

						Reading_Status = 1;						

						/*
						printf("Determining next state, are we in a constructed data object?\n");
						if (Is_Bit_Set(Processing_Constructed_Data_Object, nibble_flags) == 1) {				   				
						}*/


					}

					else {

						printf("Processing Unknown status. Error.\n");
						printf("nibble_flags %d\n", *nibble_flags);
						Reading_Status = 6;

					}

				}

			}
		}
	}		
	return Reading_Status;
}

TLV_Block * Create_New_TLV_Block(int Depth, TLV_Block * Head_TLV_Block) {

	TLV_Block * New_TLV_Block_ptr = malloc(sizeof(TLV_Block));

	if (New_TLV_Block_ptr != NULL) {

		New_TLV_Block_ptr->Tag = NULL;
		New_TLV_Block_ptr->Tag_Def = NULL;
		New_TLV_Block_ptr->Length = NULL;
		New_TLV_Block_ptr->Value = NULL;
		New_TLV_Block_ptr->Constructed = 0;
		New_TLV_Block_ptr->FilePos_at_EndofBlock = 0;
		New_TLV_Block_ptr->Depth = Depth;
		New_TLV_Block_ptr->Head = Head_TLV_Block;
		New_TLV_Block_ptr->Parent = NULL;
		New_TLV_Block_ptr->Child = NULL;
		New_TLV_Block_ptr->Next = NULL;
		New_TLV_Block_ptr->Previous = NULL;

		return New_TLV_Block_ptr;
	}

	else {
		printf("Unable to allocate memory to TLV Block");
		return NULL;
	}

}

void Debug_ReadingStatus(unsigned int* nibble_flags_ptr, int Reading_Status){

	printf("_______________________________________\n");
	printf("Processing Tag = %d\n", (Is_Bit_Set(Processing_Tag, nibble_flags_ptr)) );
	printf("Processing LengthField= %d\n", (Is_Bit_Set(Processing_LengthField, nibble_flags_ptr)) );
	printf("Processing Length = %d\n", (Is_Bit_Set(Processing_Length, nibble_flags_ptr)) );
	printf("Processing Value = %d\n", (Is_Bit_Set(Processing_Value, nibble_flags_ptr)) );
	
	printf("Processing Subsequent Field = %d\n", (Is_Bit_Set(Processing_Subsequent_Field, nibble_flags_ptr)) );
	printf("Processing Subsequent Field FirstNibFlag = %d\n", (Is_Bit_Set(Processing_Subsequent_Field_FirstNibFlag, nibble_flags_ptr)) );
	printf("Processing First Nibble = %d\n", (Is_Bit_Set(Processing_First_Nibble, nibble_flags_ptr)) );
	printf("Processing Constructed Data Object = %d\n", (Is_Bit_Set(Processing_Constructed_Data_Object, nibble_flags_ptr)) );
	printf("_______________________________________\n");
	printf("Reading Status %d\n", Reading_Status);
	printf("_______________________________________\n\n");

	return;
}
