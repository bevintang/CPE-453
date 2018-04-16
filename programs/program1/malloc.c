#include "malloc.h"

/**
 *
	Methods for both types size_t and void* to make sure each
	number is divisble by 16.
 *
**/
size_t div16(size_t size) {
	size_t temp = size;
	uint8_t remainder;

	if ((remainder = temp % 16) != 0){
		temp += 16 - remainder;
	}

	return temp;
}

void* div16Ptr(void* ptr) {
	size_t addr = (size_t)ptr + sizeof(Header);
	uint8_t remainder;

	/* Avoid collision of header and data addresses if ptr is divisble by
	   16 */
	if ((remainder = addr % 16) != 0)
		addr += 16 - remainder;

	ptr = (void*) addr;

	return ptr;
}

/**
 *
 	Given a size, create a new, non-free Header struct whose next param is
 	NULL.

 	RETURN VALUE: Header struct pointer to the newly created Header.
 *
**/
Header* newHeader(size_t size) {
	Header* newHeader;
	size_t totalSize = div16(size) + div16(sizeof(Header));

	/* Ask for more memory using sbrk() */
	if ((newHeader = sbrk(totalSize)) == NULL)
		return NULL;

	/* Construct this new header */
	newHeader->size = div16(size);
	newHeader->free = 0;
	newHeader->next = NULL;

	return newHeader;
}

/**
 *
 	Given a Header in the linked list and a given size, insert a new header
 	with the specified size into the linked list.

 	RETURN VALUE: If a new header can fit, return 1. Else, 0.
 *
**/
int insertHeader(Header* current, size_t size) {
	/* Construct new header */
	Header* newHeader = (Header*)((size_t)current + current->size);
	if ((size_t)newHeader + div16(sizeof(Header)) + size >= 
			(size_t)current->next){
		return 0;
	}

	newHeader->size = size;
	newHeader->free = 1;

	/* Insert into Linked List */
	newHeader->next = current->next;
	current->next = newHeader;

	return 1;
}

/**
 *
	If the size can fit in an existing header, return that header's 
	location. If not, return NULL.
 *
**/
void* canFit(size_t size) {
	/* Track the traversal of linked list */
	Header* curHead;
	size_t curHeadSize;
	int isFree;

	/* Check if there are even any entries in the linked list */
	if (linkedHeaders == NULL){
		return NULL;
	}

	/* Initial assignment of variables */
	curHead = linkedHeaders;
	curHeadSize = curHead->size;
	isFree = curHead->free;

	/* Check the size of each header */
	while ((curHeadSize < size || !isFree) && curHead->next != NULL) {
		curHead = curHead->next;
		curHeadSize = curHead->size;
		isFree = curHead->free;
	}

	/* Exit while loop if proper size is found: */
	if (curHeadSize >= size && isFree) {
		/* Make new header to fill gap */
		if (insertHeader(curHead, curHeadSize - size))
			curHead->size = size;
		return curHead;
	}
	/* Or size not found (i.e. next Header is NULL) */
	else{
		return NULL;
	}
}

/**
 *
 	Retrieve that last node of the global linked list and return that
 	node's address. 

 	RETURN VALUE: Return a pointer to the address of the last Header. 
 				  If linked list is empty, return NULL.
 *
**/
void* lastNode() {
	Header* curHead = linkedHeaders;

	if (linkedHeaders == NULL)
		return NULL;

	while (curHead->next != NULL){
		curHead = curHead->next;
	}
	return curHead;
}

/**
 *
 	Append a new header pointer to the linked list
 *
**/
void append(Header* newHeader) {
	Header* tail = lastNode();
	/* Check if the linked list is empty */
	if (tail == NULL){
		linkedHeaders = newHeader;
	}
	else {
		tail->next = newHeader;
	}
}

int overFlow(size_t num1, size_t num2, int op){
	size_t result = num1 + num2;

	if (op == MULTIPLY)
		result = num1 * num2;

	if (result < num1 || result < num2)
		return 1;
	return 0;
}

void combineHeaders(Header* current, Header* next){
	/* Make current header hold new size */
	current->size = current->size + next->size;

	/* Make current's next equal to next's next */
	if (next->next != NULL)
		current->next = next->next;
	else
		current->next = NULL;
}

/**
 *
 	Search through the linked list of Headers to combine any two adjacent,
 	free chunks. 
 *
**/
void defrag(){
	Header* current = linkedHeaders;
	Header* previous = linkedHeaders;
	size_t curSize = 0;
	size_t nextSize = 0;
	int curFree = 0;
	int nextFree = 0;

	/* Return if linked list isn't long enough */
	if (current == NULL)
		return;

	/* Iterate through linked list and defrag */
	while (current->next != NULL){
		curSize = current->size;
		nextSize = current->next->size;
		curFree = current->free;
		nextFree = current->next->free;

		/* if we find adjacent, free headers, combine them */
		if (curFree && nextFree && !overFlow(curSize, nextSize, ADD)){
			combineHeaders(current, current->next);
		}
		else{
			if (current->next != NULL){
				previous = current;
				current = current->next;
			}
		}
	}

	/* Remove a header if it is free and the last in the linked list */
	if(current->next == NULL && current->free){
		previous->next = NULL;
		sbrk(-(curSize));
	}
}

/**
 *
 	Given a pointer, find the header the described address is a part of.

 	RETURN VALUE: Header* who points at the closest Header to ptr. 
 				  If not found, return NULL.
 *
**/
Header* getClosest(void* ptr){
	void* toFree = (void*)ptr;
	Header* current = linkedHeaders;
	Header* next = linkedHeaders;
	size_t endOfData = 0;

	/* Jump out if out of lower bounds of linked list */
	if (ptr < (void*)linkedHeaders){
		return NULL;
	}

	/* Look throw linked list until we find the Header who's address is
	   immediately before the requested one */
	while (current->next != NULL && (void*)current->next < toFree){
		current = current->next;
		next = current->next;
	}

	/* Jump out if out of upper bounds of linked list */
	endOfData = (size_t)current + current->size + div16(sizeof(Header));
	if (ptr > (void*)endOfData){
		return NULL;
	}
	return current;	
}

/**
 *
 	Given a pointer, free the appropriate chunk of data that pointer is
 	a part of.

 	At the end of the call, defrag the memory.
 *
**/
void free(void* ptr) {
	Header* current = linkedHeaders;
	size_t endOfData = 0;

	/* Do nothing if there aren't any headers yet */
	if (ptr == NULL || current == NULL)
		return;

	/* Quit out if there is no associated Header */
	if ((current = getClosest(ptr)) == NULL)
		return;
	
	/* Only free segment if the ptr is within the bounds of the header */
	endOfData = (size_t)current + current->size + div16(sizeof(Header));
	if (ptr < (void*)endOfData)
		current->free = 1;

	/* Defrag after freeing */
	defrag();
}

/**
 *
 	Check the linked list of headers to see if there is an available
 	size. If not, create a new header and set aside new memory with
 	sbrk, and append this new header to the linked list. Otherwise, 
 	set the appropriate header's free flag to 0.

 	RETURN VALUE: A divisble-by-16 address that represents the location
 				  of the start of the data segment.
 *
**/
void* malloc(size_t size) {
	Header* header;
	size_t realSize = div16(size);
	
	if ((header = canFit(realSize)) == NULL){
		if ((header = newHeader(realSize)) != NULL)
			append(header);
		else
			return NULL;
	}
	else {
		header->free = 0;
	}

	/* Consider DEBUG_MALLOC 
	if (strtoul(getenv("DEBUG_MALLOC"), &endptr, 10)){
		fprintf(stderr, "malloc(%lu)     => (ptr=%p, size=%lu)\n", 
			size, div16Ptr(header), realSize);
	}
	*/

	return div16Ptr(header);
}

/**
 *
 	Does the same as malloc, but sets all bytes in memory to 0 using 
 	memset

 	RETURN VALUE: A divisble-by-16 address that represents the location 
 				  of the start of the data segment.
 *
**/
void* calloc(size_t nmemb, size_t size){
	size_t realSize = div16(nmemb * size);
	void* memStart;

	if (overFlow(nmemb, size, MULTIPLY)){
		return NULL;
	}

	memStart = malloc(realSize);
	memset(memStart, 0, realSize);

	return memStart;
}

/**
 *
 	Given a pointer and a specified size, will do one of two things:
 	(1) If the pointer points to a segment of data in the linked list,
 	    the data will be moved to a new location in memory with the size
 	    of 'size'
 	(2) If the pointer does not point to any data, realloc will do the 
 	    same as malloc with the specified size.

 	RETURN VALUE: A divisble-by-16 address that represents the location
 				  of the start of the data segment.
 *
**/
void* realloc(void* ptr, size_t size){
	Header* header;
	void* srcData;
	void* destData;
	size_t copySize = size;

	/* Free if size is 0 */
	if (size == 0){
		free(ptr);
		return ptr;
	}

	/* If the ptr is not already allocated, just malloc like normal */
	if (ptr == NULL || (header = getClosest(ptr)) == NULL)
		return malloc(size);

	/* Otherwise memcpy the data to a new location */
	srcData = (void*)header + div16(sizeof(Header));

	/* If requested size is smaller than the original size,
	   just shrink the data at the current pointer. 
	   Insert a new Header with the remaining data */
	if (size <= header->size){
		destData = srcData;
		if (insertHeader(header, header->size - size))
			header->size = size;
	}
	else{
		destData = malloc(size);
		free(ptr);
	}

	/* Choose appropriate size for copying */
	if (size > header->size)
		copySize = header->size;

	memcpy(destData, srcData, copySize);

	/* Free the old header and defrag any new memory if a new
	   pointer was used
	if (!inserted)
		free(ptr);
	*/

	return destData;
}

void printLinkedList(){
	Header* current = linkedHeaders;
	size_t i = 0;
	while (current != NULL){
		printf("%lu:\tSize: %lu\n\tFree: %d\n\tLoc: %p\n\t",
			i++, current->size, current->free, current);
		printf("Next: %p\n-----------------------\n", current->next);
		current = current->next;
	}
}
