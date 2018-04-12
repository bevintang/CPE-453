default: malloc

malloc.o: malloc.c
	gcc -std=c89 -Wall -Wno-deprecated-declarations -c malloc.c -o malloc.o

malloc: malloc.o
	gcc -std=c89 -Wall -Wno-deprecated-declarations malloc.o -o my_malloc

clean:
	-rm -f malloc.o
	-rm -f my_malloc
	