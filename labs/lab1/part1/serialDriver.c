#include "globals.h"

int main(void) {
	// Init serial
	serial_init();

	// Write a string to the screen
	print_string("print_string: (if this line appears, YAY)\n");
	print_string("print_int w/ 1024: ");
	print_int(1024);
	print_string("\nprint_int32 w/ 268435456: ");
	print_int32(268435456);

	// Change color and test hex
	set_cursor(5, 1);	// set to row 10
	set_color(RED);
	print_string("\nprint_hex w/ 1024: ");
	print_hex(1024);
	print_string("\nprint_hex32 w/ 268435456: ");
	print_hex32(268435456);
	print_string("\n");
	

   return 0;
}