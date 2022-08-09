#ifndef Bits_n_Bytes
	
	#define Bits_n_Bytes
	
	/*Defining Bitwise operator for Nibble_Flags Variable, contains all the flags needed for TLV parsing*/

	#define B1_B8 128 
	#define B1_B7 64
	#define B1_B6 32
	#define B1_B5 16

	#define B1_B4 8
	#define B1_B3 4
	#define B1_B2 2
	#define B1_B1 1

	#define N1_B4 8
	#define N1_B3 4
	#define N1_B2 2
	#define N1_B1 1

	#define N2_B4 8
	#define N2_B3 4
	#define N2_B2 2
	#define N2_B1 1

	void Set_Bit(int BitToSet, unsigned int *TargetByte);
	void UnSet_Bit(int BitToUnSet, unsigned int *TargetByte);
	int Is_Bit_Set(int BitToTest, unsigned int * TargetByte);


#endif
