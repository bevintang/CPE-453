#include <unistd.h>
#include <stdint.h>
#include <stdio.h>

/* Struct of a Header (Linked List) */
typedef struct h{
	size_t size;
	uint8_t free;
	struct h* next;
} Header;

/* Global Linked list of Headers */
Header* linkedHeaders = NULL;

size_t div16(size_t size) {
	uint8_t remainder;
	if ((remainder = size % 16) !=0 )
		size += 16 - remainder;
	return size;
}

Header* div16Ptr(Header* ptr) {
	uintptr_t addr = (uintptr_t)ptr;
	uint8_t remainder;
	if ((remainder = addr % 16) != 0)
		addr += 16 - remainder;

	printf("%p\n", ptr);
	printf("%p\n", (Header*)addr);
	return (Header *)addr;
}

/**
 *
	If the size can fit in an existing header, return that header's location. 
	If not, return NULL.
 *
**/
void* canFit(size_t size) {

	/* Track the traversal of linked list */
	Header* curHead;
	size_t curHeadSize;
	int isFree;

	/* Check if there are even any entries in the linked list */
	if (linkedHeaders == NULL){
		printf("Linked list is empty\n");
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
		printf("Found proper size!\n");
		return curHead;
	}
	/* Or size not found (i.e. next Header is NULL) */
	else{
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
	/* find address in Linked list that is immediately before the specified ptr */
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
	printf("\nMalloc called!\n");
	Header* header;
	size_t size16 = div16(size+16) + div16(sizeof(Header));
	if ((header = canFit(size)) == NULL){
		/* Ask for more memory using sbrk() */
		header = div16Ptr(sbrk(size16));

		/* Construct this new header */
		header->size = size;
		header->free = 0;
		header->next = NULL;

		/* add this new header to the linked list */
		printf("No matches! Appending new header...\n");
		append(header);
	}
	else {
		header->free = 0;
	}
	printf("Header @ %p\n", header);
	return ++header;	/* return start of actual data */
}

int main(void) {
	int* sample = my_malloc(16);
	int* sample2 = my_malloc(16);
	printf("Before free %p\n", sample);
	printf("%p\n", sample2);
	my_free(sample);
	sample = my_malloc(16);
	printf("After free %p\n", sample);

	return 0;
}