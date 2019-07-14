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

			if( ( (N1_B4 && input_nibble_int) == 1) ) {
				//then it's a Multiple byte length field, the next nibble is the length value in Bytes.
				Set_Bit(Processing_LengthField, nibble_flags);
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

char * TLV_Block_to_Output(char * Output_ptr, char * TLV_Block_ptr) {

	char * Temp = NULL;

	if (Output_ptr == NULL) {
		Temp = malloc( ((strlen(TLV_Block_ptr) + 1) + (strlen("\n\n") + 1)) * sizeof(char) );

		if (Temp != NULL) {
			Output_ptr = Temp;
			Temp = NULL;
			*Output_ptr = '\0';
			strcat(Output_ptr, TLV_Block_ptr);
			strcat(Output_ptr, "\n\n");
			TLV_Block_ptr = NULL;
			return Output_ptr;

		}

		else {
			printf("Failed to Allocate Memory to Output.\n");
			printf("Exiting...\n\n");
			return 0;
		}
	}

	else {
		Temp = realloc(Output_ptr, ((( (strlen(Output_ptr) + 1) + (strlen(TLV_Block_ptr) + 1) + (strlen("\n") + 1) ) * sizeof(char)) ));
		if (Temp != NULL) {
			Output_ptr = Temp;
			Temp = NULL;
			strcat(Output_ptr, TLV_Block_ptr);
			strcat(Output_ptr, "\n");
			TLV_Block_ptr = NULL;
			return Output_ptr;
		}
		else {
			printf("Failed to rellocate Memory.\n");
			printf("Exiting...");
			return 0;
		}
	}
	//Constructed Data Object processing here.

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

char *Find_Tag_Def(char* TagDefOutput, char *SearchTag, Tag * InputList){
	
	int i, Tag_List_Size; 
	
	TagDefOutput = NULL;
	
	Tag_List_Size = SIZE_TAG_LIST;
	//Tag_List_Size = sizeof(Tag_List)/sizeof(Tag);
	printf(" Size of Tag_List is %d\n\n", Tag_List_Size);

	for (i = 0; i <= Tag_List_Size; i++){
	
		if(TagDefOutput == NULL){
	
			if (strcmp( (InputList[i].BER_TLV), SearchTag) == 0){
				TagDefOutput = malloc( sizeof(char) * ( (strlen(InputList[i].Definition)) + 1 ));
				TagDefOutput = InputList[i].Definition;
				printf("Found Tag.\n\t: %s \n", InputList[i].BER_TLV);
				printf("Found Definition.\n\t: %s \n", TagDefOutput);
				printf("Break point 1\n");
				break;
			}
			else{
			}
		}

		else{
			i = Tag_List_Size + 1;
		}
	}

	printf("TagDefOutput %s\n", TagDefOutput);
	return TagDefOutput;
}


int Determine_Reading_Status(unsigned int* nibble_flags, int Invalid_Data_Flag){

	int Reading_Status;
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
					printf("Processing Unknown status. Error.\n");
					printf("nibble_flags %d\n", *nibble_flags);
					Reading_Status = 6;
				}

			}
		}
	}		
	return Reading_Status;
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
