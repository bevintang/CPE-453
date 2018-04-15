#include <stdlib.h>
#include <stdio.h>

/*
	Crappy driver
*/
int main(void) {
	int* sample1 = (int*)malloc(16);
	printLinkedList();
	char* sample2 = (char*)malloc(160);
	printLinkedList();

	/* Remalloc sample1 */	
	printf("FREEING SAMPLE1!...\n");
	free(sample1);
	printLinkedList();

	printf("MALLOCING SAMPLE1 16...\n");
	sample1 = malloc(16);
	printLinkedList();

	printf("FREEING SAMPLE2!...\n");
	free(sample2);
	printLinkedList();

	printf("MALLOCING SAMPLE2 4...\n");
	sample2 = malloc(4);
	printLinkedList();

	printf("CALLOCING SAMPLE3 64...\n");
	int* sample3 = calloc(16, 4);
	printLinkedList();

	printf("CALLOCING SAMPLE4 160...\n");
	int* sample4 = calloc(16, 10);
	printLinkedList();

	printf("\n\nFREEING SAMPLE2 4\n");
	free(sample2);
	printLinkedList();

	printf("\n\nFREEING SAMPLE3 64\n");
	free(sample3);
	printLinkedList();

	printf("\n\nFREEING SAMPLE4 160\n");
	free(sample4 + 10);
	printLinkedList();

	printf("\n\nREALLOC SAMPLE1 160\n");
	sample1 = realloc(sample1, 160);
	printLinkedList();

	printf("\n\nREALLOC nonexisting pointer 200\n");
	sample1 = realloc(sample1, 20000);
	printLinkedList();

	return 0;
}
