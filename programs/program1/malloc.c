#include <unistd.h>
#include <stdint.h>
#include <stdio.h>

/** 
 *
	Struct of a Header (Linked List) 
 *
**/
typedef struct h{
	size_t size;
	uint8_t free;
	struct h* next;
} Header;

/**
 *
	Global Linked list of Headers 
 *
**/
Header* linkedHeaders = NULL;

/**
 *
	Methods for both types size_t and void* to make sure each
	number is divisble by 16.
 *
**/
void* div16Ptr(void* ptr) {
	size_t addr = (size_t)ptr + sizeof(Header);
	uint8_t remainder;

	/* Avoid collision of header and data addresses if ptr is divisble by 16 */

	if ((remainder = addr % 16) != 0)
		addr += 16 - remainder;

	ptr = (void*) addr;

	return ptr;
}

/**
 *
	If the size can fit in an existing header, return that header's location. 
	If not, return NULL.
 *
**/
void* canFit(size_t size) {
	printf("Checking for requested size...\n");

	/* Track the traversal of linked list */
	Header* curHead;
	size_t curHeadSize;
	int isFree;

	/* Check if there are even any entries in the linked list */
	if (linkedHeaders == NULL){
		printf("\tLinked list is empty. Will append new header.\n");
		return NULL;
	}

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
		printf("\tFound proper size!\n");
		return curHead;
	}
	/* Or size not found (i.e. next Header is NULL) */
	else{
		printf("\tNo matches!\n");
		return NULL;
	}
}

/**
 *
 	Retrieve that last node of the gloval linked list and return that
 	node's address. Return NULL if the linkedlist is empty.
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
	printf("\tAppending new header...\n");
	Header* tail = lastNode();
	/* Check if the linked list is empty */
	if (tail == NULL){
		linkedHeaders = newHeader;
	}
	else {
		tail->next = newHeader;
	}
}

void my_free(void* ptr) {
	/* find address in Linked list that is immediately before the desired ptr */
	Header* toFree = (Header*)ptr;
	Header* previous = linkedHeaders;
	Header* current = linkedHeaders;

	while (current->next < toFree){
		previous = current;
		current = current->next;
	}
	previous->free = 1;
}

void* my_malloc(size_t size) {
	Header* header;
	size_t size16 = size + sizeof(Header);
	if ((header = canFit(size)) == NULL){
		/* Ask for more memory using sbrk() */
		header = sbrk(size16);

		/* Construct this new header */
		header->size = size;
		header->free = 0;
		header->next = NULL;

		/* add this new header to the linked list */
		append(header);
	}
	else {
		header->free = 0;
	}
	printf("NEW HEADER: %p\n", header);
	printf("DATA STARTS AT: %p\n\n\n", div16Ptr(header));
	return div16Ptr(header);	/* return start of actual data */
}

int main(void) {
	int* sample1 = my_malloc(16);
	int* sample2 = my_malloc(16);

	/* These two addresses should be different */
	printf("!!MAIN: Before free\n"); 
	printf("!!MAIN: Sample1 %p\n", sample1);
	printf("!!MAIN: Sample2 %p\n", sample2);

	/* Remalloc sample1 */
	my_free(sample1);
	sample1 = my_malloc(160);
	printf("!!MAIN: After free\n"); 
	printf("!!MAIN: Sample1: %p\n", sample1);	/* Should be same address as original */

	/* Add new headers to the end of linked list */
	int* sample3 = my_malloc(65);
	printf("!!MAIN: Sample3: %p\n\n\n", sample3);

	return 0;
}