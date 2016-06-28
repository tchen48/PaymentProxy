
#include "stdafx.h"
#include "controlChar.h"
#include <stdlib.h>  
#ifndef  NULL
#define NULL 0
#endif // ! NULL



class ControlCharOperator {
private:
	size_t _countBit(char bit) {
		if (bit == NULL)
			return 0;
		int count = BIT_ONE & bit;
		for (int i = 0; i < 8; i++)
			count += (bit = bit >> 1) & BIT_ONE;
		return count;
	}

public:
	void AddParity(char &bit) {
		if ((bit & BIT_EIGHT) != 0)
			return;
		if (bit == NULL)
			return;
		if (_countBit(bit) % 2 == 1)
			bit = bit | BIT_EIGHT;
		return;
	}
	void RemoveParity(char &bit)
	{
		bit = bit & ~BIT_EIGHT;
	}
	char* ComputeLRC(char* message, unsigned int size)
	{
		char* lrcBuff = (char*)malloc(1);
		char lrc = 0;
		for (int i = 0; i < size; i++)
			lrc = lrc ^ message[i];
		lrcBuff[0] = lrc;
		return lrcBuff;
	}
	//MAY USE A POOL TO MANAGE, SO THAT DON'T NEED TO APPLY FOR NEW SPACE EVERY TIME
	char* combine(char* a, size_t a_size, char* b, size_t b_size)
	{
		char* newBuff = (char*)malloc(a_size + b_size);
		for (int i = 0; i < a_size; i++)
			newBuff[i] = a[i];
		free(a);
		a = newBuff;
		for (int i = 0; i < b_size; i++)
			newBuff[a_size + i] = b[i];
		free(b);
		b = &newBuff[a_size];
		return newBuff;
	}
};