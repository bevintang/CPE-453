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
	if (curHeadSize >= size) {
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

void* my_malloc(size_t size) {
	Header* header;
	if ((header = canFit(size)) == NULL){
		/* Ask for more memory using sbrk() */
		header = sbrk(size + sizeof(Header));

		/* Construct this new header */
		header->size = size;
		header->free = 0;
		header->next = NULL;

		/* add this new header to the linked list */
		printf("Trying to append...\n");
		append(header);
	}
	else {
		header->free = 0;
	}
	return ++header;	/* return start of actual data */
}

void my_free(void* ptr) {

}

int main(void) {
	printf("%p\n", my_malloc(6400));
	return 0;
}