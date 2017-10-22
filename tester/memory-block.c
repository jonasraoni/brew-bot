/*
 * BrewBot: API tester for devices using the platform Qualcomm BREW.
 * Jonas Raoni Soares da Silva <http://raoni.org>
 * https://github.com/jonasraoni/brew-bot
 */

#ifndef __MEMORYBLOCK__
#define __MEMORYBLOCK__

//-- includes -----------------------------------------------
#include "AEEStdLib.h"

//-- data types ---------------------------------------------
typedef struct _MemoryBlock {
	void *data; //real data pointer
	size_t
		capacity, //current capacity of items
		used, //used items
		size; //size of each item
}* MemoryBlock;

//-- functions ----------------------------------------------
//creates a new memory block, size is the size of each data item, capacity will determine how much memory should be initially reserved
MemoryBlock getMemoryBlock(register const size_t size, register const size_t capacity){
	MemoryBlock mb = (MemoryBlock)MALLOC(sizeof(struct _MemoryBlock));
	if(mb) {
		mb->used = 0;
		mb->data = NULL;
		mb->size = size;
		mb->capacity = capacity && (mb->data = (void *)MALLOC(capacity * size)) ? capacity : 0;
	}
	return mb;
}

//frees up the memory block altogether with its data
void freeMemoryBlock(register const MemoryBlock mb){
	FREE(mb->data);
	FREE(mb);
}

//frees up only the memory block
void freeMemoryBlockEx(register const MemoryBlock mb){
	FREE(mb);
}

//resizes the memory block to accommodate the "capacity" amount of items
int resizeMemoryBlock(register const size_t capacity, register const MemoryBlock mb){
	void *data = (void *)REALLOC(mb->data, capacity * mb->size);
	if(data){
		mb->data = data;
		return mb->capacity = capacity;
	}
	return 0;
}

//grows the memory block's capacity dinamically according to its current capacity
int growMemoryBlock(register const MemoryBlock mb){
	size_t delta = mb->capacity > 64 ? mb->capacity / 4 : mb->capacity > 8 ? 16 : 4;
	void *data = (void *)REALLOC(mb->data, (mb->capacity + delta) * mb->size);
	if(data){
		mb->data = data;
		return mb->capacity += delta;
	}
	return 0;
}

//grows the memory block's capacity dinamically according to its current capacity, until the extra neededBlocks requirement is satisfied
int growMemoryBlockEx(register const size_t neededBlocks, register const MemoryBlock mb){
	while(mb->used + neededBlocks > mb->capacity)
		if(!growMemoryBlock(mb))
			return 0;
		return 1;
}

//check if there's room for one more item, if not "grows" the memory block
int incMemoryBlock(register const MemoryBlock mb){
	return mb->used + 1 <= mb->capacity || growMemoryBlock(mb);
}

#endif
