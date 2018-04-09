#include "globals.h"

#define NAME_LEN 10

/*
	Print the current position of the cursor to the 
	top-left portion of the screen (row, col)	
*/
void print_rc(int row, int col){
	set_cursor(0, 0);
	print_string("(");
	print_int(row);
	print_string(", ");
	print_int(col);
	print_string(")\n");
	set_cursor(row, col);
}

/*
	Print my name to the screen
*/
void print_name(int row, int col) {
	clear_screen();
	set_cursor(row, col);
	print_string("Bevin Tang");
}

/*
	Increment row based on inc and cap the position
	to fit within 25 rows
*/
void inc_row(int* row, int inc){
	*row += inc;
	if (*row < 2)
		*row = 2;
	else if (*row > 25)
		*row = 25;
}

/*
	Increment col based on inc and cap the position
	to fit within 80 columns
*/
void inc_col(int* col, int inc){
	*col += inc;
	if (*col < 1)
		*col = 1;
	else if (*col > 80 - NAME_LEN)
		*col = 80 - NAME_LEN;
}

/*
	Change the position of row/col based on key press
*/
void move_cursor(char input, int* row, int* col){
	switch (input){
		case 'a':
			inc_col(col, -1);
			break;
		case 'd':
			inc_col(col, 1);
			break;
		case 'w':
			inc_row(row, -1);
			break;
		case 's':
			inc_row(row, 1);
			break;
		default:
			break;
	}
}

/*
	Change the foreground color of screen based.
	Cycle through: 
	red, green, yellow, blue, magenta, cyan, and white

	Each VT100 code for color is separated by 1 -- 
	starting with RED = 31 and ending with WHITE = 37
*/
void change_color(char input, int* color) {
	if (input != 'c')
		return;

	*color += 1;
	if (*color > WHITE){
		*color = RED;
	}
	set_color(*color);
}

void run() {
	int row = 2;
	int col = 1;
	char input = 'q';
	int color = WHITE;

	while (1){
		if (byte_available()){
			input = read_byte();
			move_cursor(input, &row, &col);
			change_color(input, &color);
			print_name(row, col);
			set_color(WHITE);
			print_rc(row, col);
			set_color(color);
		}
	}
}

int main(void) {
	serial_init();
	clear_screen();
	print_rc(2, 1);
	run();
}