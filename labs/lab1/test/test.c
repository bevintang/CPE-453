#include <stdio.h>
#include <stdint.h>

int main(void) {
	int temp = 1024;
    int numDigs = 0;
    char digits[17];

    while (temp > 0){w
        digits[numDigs++] = '0' + temp % 10;
        temp = temp / 10;
    }

    for (int i = 1; i <= numDigs; i++) {
    	printf("%c\n", digits[numDigs - i]);
    }
}