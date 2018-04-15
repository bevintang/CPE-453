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

	/* Avoid collision of header and data addresses if ptr is divisble by 16 */

	if ((remainder = addr % 16) != 0)
		addr += 16 - remainder;

	ptr = (void*) addr;

	return ptr;
}

/**
 *
 	Given a size, create a new, non-free Header struct whose next param is NULL.

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
 *
**/
void insertHeader(Header* current, size_t size) {
	/* Construct new header */
	Header* newHeader = div16Ptr(div16Ptr(current) + current->size);
	if (size < 16 || newHeader == current->next)
		return;

	printf("HEADER BEFORE IS AT %p\n", current);
	printf("INSERTING NEW HEADER AT: %p\n\n", newHeader);
	newHeader->size = size;
	newHeader->free = 1;

	/* Insert into Linked List */
	newHeader->next = current->next;
	current->next = newHeader;
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
		/* Make new header to fill gap*/
		insertHeader(curHead, curHeadSize - size);
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

 	RETURN VALUE: Return a pointer to the address of the last Header. If linked
 				  list is empty, return NULL.
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

int overFlow(size_t num1, size_t num2){
	size_t sum = num1 + num2;
	if (sum < num1 || sum < num2)
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
 	Search through the linked list of Headers to combine any two adjacent, free
 	chunks. 

 	Performance: O(n).
 *
**/
void defrag(){
	Header* current = linkedHeaders;
	size_t curSize = 0;
	size_t nextSize = 0;
	int curFree = 0;
	int nextFree = 0;

	/* Return if linked list isn't long enough */
	if (current == NULL)
		return;

	/* Iterate through linked list and combine adjacent, free headers */
	while (current->next != NULL){
		curSize = current->size;
		nextSize = current->next->size;
		curFree = current->free;
		nextFree = current->next->free;
		if (curFree && nextFree && !overFlow(curSize, nextSize)){
			printf("Defragging...\n");
			combineHeaders(current, current->next);
		}
		else{
			if (current->next != NULL)
				current = current->next;
		}
	}
}

/**
 *
 	Given a pointer, free the appropriate chunk of data that pointer is
 	a part of. To do this, find the header whose address is immediately
 	before the desired pointer, and free it.

 	At the end of the call, defrag the memory.
 *
**/
void my_free(void* ptr) {
	Header* toFree = (Header*)ptr;
	Header* current = linkedHeaders;
	Header* next = linkedHeaders;

	if (current == NULL || next == NULL)
		return;

	while (next != NULL && current->next < toFree){
		current = current->next;
		next = current->next;
	}
	current->free = 1;
	defrag();
}

/**
 *
 	Check the linked list of headers to see if there is an available
 	size. If not, create a new header and set aside new memory with sbrk, and
 	append this new header to the linked list. Otherwise, set the appropriate
 	header's free flag to 0.

 	RETURN VALUE: A divisble-by-16 address that represents the location of the
 				  start of the data segment.
 *
**/
void* my_malloc(size_t size) {
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
	printf("NEW HEADER: %p\n", header);
	printf("DATA STARTS AT: %p\n\n", div16Ptr(header));
	return div16Ptr(header);
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

/*
	Crappy driver
*/
int main(void) {
	int* sample1 = (int*)my_malloc(16);
	printLinkedList();
	char* sample2 = (char*)my_malloc(160);
	printLinkedList();

	/* Remalloc sample1 */	
	my_free(sample1);
	sample1 = my_malloc(16);

	my_free(sample2);
	printLinkedList();

	sample2 = my_malloc(4);
	printLinkedList();

	int* sample3 = my_malloc(64);
	printLinkedList();

	int* sample4 = my_malloc(160);
	printLinkedList();

	printf("\n\nFREEING SAMPLE2\n");
	my_free(sample2);
	printLinkedList();

	printf("\n\nFREEING SAMPLE3\n");
	my_free(sample3);
	printLinkedList();
	/*
	my_free(sample2);
	sample1 = my_malloc(32);
	sample2 = my_malloc(4);
	printLinkedList();
	*/

	return 0;
}