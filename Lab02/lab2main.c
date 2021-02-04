//***********************************************************************************************************
//  Author: Girma Tewolde
//	Last modified: 4-9-2020 @5:52 P.M.
//  Purpose: Framework for CE-426 Lab 2
//
//  Use the array data defined in the global memory space and the main program in this file for testing
//  your two versions, i.e. assembly and C, of the findHighBit function.
//  Test each of your two implementations with the given array data and make sure your answers match 
//  with the ones provided in the comments for each array.
//  You will also compare the two implementations on criteria such as execution speed and size of 
//  memory used by the corresponding functions.
//***********************************************************************************************************

#include <stm32f10x.h> 

uint32_t array0[] = {0x00000001,0x00000020,0x00000400,0x00008000,0x00440000,0x02200000,0x12000000,0x80000000}; // answer = 255; 
uint32_t array1[] = {0x00000001,0x00000020,0x00000400,0x00008000,0x00440000,0x02200000,0x12000000,0x00000000}; // answer = 220; 
uint32_t array2[] = {0x00000001,0x00000020,0x00000400,0x00008000,0x00440000,0x02200000,0x00000000,0x00000000}; // answer = 185; 
uint32_t array3[] = {0x00000001,0x00000020,0x00000400,0x00008000,0x00440000,0x00000000,0x00000000,0x00000000}; // answer = 150; 
uint32_t array4[] = {0x00000001,0x00000020,0x00000400,0x00008000,0x00000000,0x00000000,0x00000000,0x00000000}; // answer = 111; 
uint32_t array5[] = {0x00000001,0x00000020,0x00000400,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000}; // answer = 74; 
uint32_t array6[] = {0x00000001,0x00000020,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000}; // answer = 37; 
uint32_t array7[] = {0x00000001,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000}; // answer = 0; 
volatile uint8_t arrayAnswers[] = {0, 0, 0, 0, 0, 0, 0, 0};

uint32_t *arrays[] = {array0, array1, array2, array3, array4, array5, array6, array7};
uint32_t narrays = sizeof(arrays)/sizeof(uint32_t*);
uint8_t nelements = sizeof(array0)/sizeof(uint32_t);

uint32_t findHighBitASM(uint32_t*, uint32_t);
uint32_t findHighBitC(uint32_t*, uint32_t);

int main(void){
	int i;
	volatile int position;
	for (i = 0; i<narrays; i++)
	{
		position = findHighBitASM(arrays[i], nelements);
		//position = findHighBitC(arrays[i], nelements);
		arrayAnswers[i] = position;
	}
	
	while(1){position= arrayAnswers[__current_pc()%8];} // endless loop to keep micro from crashing
						  // position++ keeps position in scope for easier debugging
}

uint32_t findHighBitC(uint32_t* array, uint32_t nelements)
{
	int i = 0;
	int j = 0;
	uint32_t bitmask = 0x80000000;
	uint32_t returnvalue = 0;
  for(i = nelements; i >= 0; i--)
	{
		bitmask = 0x80000000;
		//bitmask = 1;
		for(j = 0; j <= 32; j++)
		{
 			if(array[i - 1] & bitmask)
			{
				returnvalue = 32*i-1-j; // 255 - ((8 - i) * 32) - j
				bitmask = returnvalue;
				return returnvalue;
			}
			else
			{
				bitmask = bitmask >> 1;
			}
		}
	}
	return returnvalue;
}
