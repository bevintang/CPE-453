#include <stdlib.h>

/*
	Crappy driver
*/
int main(void) {
	int* sample1 = (int*)my_malloc(16);
	printLinkedList();
	char* sample2 = (char*)my_malloc(160);
	printLinkedList();

	/* Remalloc sample1 */	
	printf("FREEING SAMPLE1!...\n");
	my_free(sample1);
	printLinkedList();

	printf("MALLOCING SAMPLE1 16...\n");
	sample1 = my_malloc(16);
	printLinkedList();

	printf("FREEING SAMPLE2!...\n");
	my_free(sample2);
	printLinkedList();

	printf("MALLOCING SAMPLE2 4...\n");
	sample2 = my_malloc(4);
	printLinkedList();

	printf("CALLOCING SAMPLE3 64...\n");
	int* sample3 = my_calloc(16, 4);
	printLinkedList();

	printf("CALLOCING SAMPLE4 160...\n");
	int* sample4 = my_calloc(16, 10);
	printLinkedList();

	printf("\n\nFREEING SAMPLE2 4\n");
	my_free(sample2);
	printLinkedList();

	printf("\n\nFREEING SAMPLE3 64\n");
	my_free(sample3);
	printLinkedList();

	printf("\n\nFREEING SAMPLE4 160\n");
	my_free(sample4 + 10);
	printLinkedList();

	printf("\n\nREALLOC SAMPLE1 160\n");
	sample1 = my_realloc(sample1, 160);
	printLinkedList();

	printf("\n\nREALLOC nonexisting pointer 200\n");
	int* trash2 = my_realloc(trash, 20000);
	printLinkedList();

	return 0;
}