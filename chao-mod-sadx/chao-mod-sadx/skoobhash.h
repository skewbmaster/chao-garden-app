#pragma once

// Prime number apparently good for hashing
#define MAGICPRIME (uint32_t)0x01000193
#define INITSEED (uint32_t)0x23

uint32_t SkoobHashOnMem(void* pointerToMem, size_t sizeOfMem, uint32_t hashSeed);