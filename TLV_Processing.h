#ifndef TLV_Processing
	
	#define TLV_Processing

	//Defining Nibble Processing Tag Values
	
	#define Processing_Tag B1_B8  
	#define Processing_LengthField B1_B7
	#define Processing_Length B1_B6
	#define Processing_Value B1_B5

	#define Processing_Subsequent_Field B1_B4
	#define Processing_Subsequent_Field_FirstNibFlag B1_B3
	#define Processing_First_Nibble B1_B2
	#define Processing_Constructed_Data_Object B1_B1
	
	#define MAX_TAG_SIZE_BYTES 2

	typedef struct Tag{
		char * BER_TLV;
		char * Definition;
	}Tag;

	typedef struct TLV_Bloc {
		char * Tag_Name;
		char * Tag_Def;
		char * Length;
		char * Value;
		struct TLV_Bloc * Children;
		struct TLV_Bloc * Parent;
		struct TLV_Bloc * Head;
	}TLV_Bloc;

	int Determine_Reading_Status(unsigned int * nibble_flags_ptr, int Invalid_Data_Flag);
	
	void Tag_Processing(char input_nibble, unsigned int * nibble_flags_ptr, unsigned int * TagField_Size_Bytes_ptr);

	int LengthField_Processing(char * input_nibble_str, int Length_Field_Size_Bytes, unsigned int * nibble_flags_ptr);
	int Length_Processing(char * Temp_Buffer, unsigned int *nibble_flags_ptr, int Length_Field_Size_Bytes);
	void Value_Processing(char input_nibble, unsigned int * nibble_flags_ptr);
	char * TLV_Block_to_Output(char* Output_ptr, char* TLV_Block_ptr);

	unsigned ASCIIHEX_to_DEC(char c);
	char Clean_Input(char c);
	char *Find_Tag_Def(char * TagDefOutput, char *SearchTag, Tag InputList[]);

	//debug functions
	void Debug_ReadingStatus(unsigned int* nibble_flags_ptr, int Reading_Status);

#endif
