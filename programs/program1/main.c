#include <stdlib.h>
#include <stdio.h>

/*
	Crappy driver
*/
int main(void) {
	int* sample1 = (int*)realloc(NULL, 16);
	printLinkedList();
	char* sample2 = (char*)realloc(NULL, 160);
	printLinkedList();

	/* Remalloc sample1 */	
	printf("FREEING SAMPLE1!...\n");
	sample1 = realloc(sample1, 0);
	printLinkedList();

	printf("MALLOCING SAMPLE1 16...\n");
	sample1 = realloc(NULL, 16);
	printLinkedList();

	printf("FREEING SAMPLE2!...\n");
	sample2 = realloc(sample2, 0);
	printLinkedList();

	printf("MALLOCING SAMPLE2 4...\n");
	sample2 = realloc(NULL, 4);
	printLinkedList();

	printf("REALLOCING SAMPLE3 64...\n");
	int* sample3 = realloc(NULL, 64);
	printLinkedList();

	printf("REALLOCING SAMPLE4 160...\n");
	int* sample4 = realloc(NULL, 160);
	printLinkedList();

	printf("\n\nREALLOCING SAMPLE2 4 -> 64\n");
	sample2 = realloc(sample2, 64);
	printLinkedList();

	printf("\n\nREALLOCING SAMPLE3 64 -> 16\n");
	sample3 = realloc(sample3, 16);
	printLinkedList();

	printf("\n\nFREEING SAMPLE4 160\n");
	sample4 = realloc(sample4 + 10, 0);
	printLinkedList();

	printf("\n\nREALLOC SAMPLE1 160\n");
	sample1 = realloc(sample1, 160);
	printLinkedList();

	printf("\n\nREALLOC SAMPLE1 pointer 160 -> 20000\n");
	sample1 = realloc(sample1, 20000);
	printLinkedList();

	printf("\n\nREALLOC SAMPLE1 pointer 20000 -> 16\n");
	sample1 = realloc(sample1, 16);
	printLinkedList();

	return 0;
}
