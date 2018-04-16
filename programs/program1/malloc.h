#ifndef MALLOC_H
#define MALLOC_H

#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define ADD 68
#define MULTIPLY 69

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
size_t div16(size_t size);
void* div16Ptr(void* ptr);

/**
 *
 	Given a size, create a new, non-free Header struct whose next param
 	is NULL.

 	RETURN VALUE: Header struct pointer to the newly created Header.
 *
**/
Header* newHeader(size_t size);

/**
 *
 	Given a Header in the linked list and a given size, insert a new header
 	with the specified size into the linked list.

 	RETURN VALUE: If a new header can fit, return 1. Else, 0.
 *
**/
int insertHeader(Header* current, size_t size)

/**
 *
	If the size can fit in an existing header, return that header's 
	location. If not, return NULL.
 *
**/
void* canFit(size_t size);

/**
 *
 	Retrieve that last node of the global linked list and return that
 	node's address. 

 	RETURN VALUE: Return a pointer to the address of the last Header. 
 	If linked list is empty, return NULL.
 *
**/
void* lastNode();

/**
 *
 	Append a new header pointer to the linked list
 *
**/
void append(Header* newHeader);

/**
 *
 	Check either the addition or multiplication of two numbers for 
 	overflow
 *
**/
int overFlow(size_t num1, size_t num2, int op);

/**
 *
 	Part of defrag. Create a new header that is the size of two adjacent,
 	free headers. And add this to the linked list.
 *
**/
void combineHeaders(Header* current, Header* next);

/**
 *
 	Search through the linked list of Headers to combine any two adjacent,
 	free chunks. 

 	Performance: O(n).
 *
**/
void defrag();

/**
 *
 	Given a pointer, find the header the described address is a part of.

 	RETURN VALUE: Header* who points at the closest Header to ptr.
 				  If not found, return NULL.
 *
**/
Header* getClosest(void* ptr);

/**
 *
 	Given a pointer, free the appropriate chunk of data that pointer is
 	a part of.

 	At the end of the call, defrag the memory.
 *
**/
void free(void* ptr);

/**
 *
 	Check the linked list of headers to see if there is an available
 	size. If not, create a new header and set aside new memory with sbrk,
 	and append this new header to the linked list. Otherwise, 
 	set the appropriate header's free flag to 0.

 	RETURN VALUE: A divisble-by-16 address that represents the location
 				  of the start of the data segment.
 *
**/
void* malloc(size_t size);

/**
 *
 	Does the same as malloc, but sets all bytes in memory to 0 using
 	memset

 	RETURN VALUE: A divisble-by-16 address that represents the location
 				  of the start of the data segment.
 *
**/
void* calloc(size_t nmemb, size_t size);

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
void* realloc(void* ptr, size_t size);

void printLinkedList();

#endif