#include <util/delay.h>
#include "globals.h"

#define NAME_LEN 10

void toggle_count(int* isCounting) {
	if (*isCounting)
		*isCounting = 0;
	else
		*isCounting = 1;
}

void print_count(uint32_t* count, int isCounting){
	if (isCounting){
		(*count)++;
	}
	clear_screen();
	set_cursor(1, 1);
	print_string("print_int:   ");
	print_int(*count);
	print_string("\nprint_int32: ");
	print_int32(*count);
	print_string("\nprint_hex:   ");
	print_hex(*count);
	print_string("\nprint_hex32: ");
	print_hex32(*count);
	print_string("\n");
}

void run() {
	int isCounting = 1;
	uint32_t count = 0;
	while (1){
		if (byte_available()){
			read_byte();
			toggle_count(&isCounting);
		}
		print_count(&count, isCounting);
		_delay_ms(1000);
	}
}

int main(void) {
	serial_init();
	run();
}