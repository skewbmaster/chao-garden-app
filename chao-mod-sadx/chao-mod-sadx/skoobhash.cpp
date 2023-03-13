#include "pch.h"
#include "skoobhash.h"

uint32_t SkoobHashOnMem(void* pointerToMem, size_t sizeOfMem, uint32_t hashSeed)
{
	unsigned char* memPointer = (unsigned char*)pointerToMem;

	for (int i = 0; i < sizeOfMem; i++)
	{
		hashSeed *= MAGICPRIME; // Multiply by the prime, this will always mod(2^32) since the integer is a 32 bit size
		hashSeed ^= *memPointer; // XOR the seed with the current byte
		memPointer++; // Move onto the next byte
	}

	return hashSeed;
}