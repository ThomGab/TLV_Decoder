#include <stdio.h>
#include "Bits_n_Bytes.h"

void Set_Bit(int BitToSet, unsigned int *TargetByte){

	if(TargetByte != NULL){
		*TargetByte = ((*TargetByte) | BitToSet);
	}
	else{
	}
		
	return;
}

void UnSet_Bit(int BitToUnSet, unsigned int *TargetByte){
	
	if(TargetByte != NULL){
		*TargetByte = ((*TargetByte) & (~BitToUnSet));
	}
	else{
	}

	return;
}

int Is_Bit_Set(int BitToTest, unsigned int * TargetByte){

//	printf("Inputs: BitToTest %d\t TargetByte: %d\n", BitToTest, *TargetByte);

	if(TargetByte != NULL){
		
		if ( (BitToTest == (BitToTest & (*TargetByte)) ) ){
			return 1;
		}
		else{
			return 0;
		}
	}
	else{
		return -1;
	}
		
}

