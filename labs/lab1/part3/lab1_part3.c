#include <util/delay.h>
#include "globals.h"

/*
	Enable LED output
*/
void enable_LED() {
	asm volatile("ANDI r31, 0x00");	// clear Z
	asm volatile("ANDI r30, 0x00");	// clear Z
	asm volatile("ORI r30, 0x24");	// go to DDRB
	asm volatile("LD r28, Z");		// load register into r28
	asm volatile("ORI r28, 0x80");	// enable MSB
	asm volatile("ST Z, r28");		// store new values into DDRB
}

/*
	Change frequency of blink based on input
*/
void change_delay(char input, int* delay) {
	switch (input){
		case 'a':
			*delay -= 20;
			break;
		case 'z':
			*delay += 20;
			break;
		default:
			break;
	}
}

/*
	Delay the clock by delay milliseconds
*/
void delay_ms(int delay){
	int i = 0;
	print_int32(delay);
	for (i = 0; i < delay; i++)
		_delay_ms(1);
}

/*
	Toggle ON/OFF State of LED
*/
void toggle_LED(int* isOn) {
	if (*isOn){
		asm volatile("ORI r27, 0x80");	// enable MSB
		*isOn = 0;
	}
	else{
		asm volatile("ANDI r27, 0x7F");	// disable MSB
		*isOn = 1;
	}
}

/*
	Turn on/off LED
*/
void blink_LED(int* isOn) {
	asm volatile("ANDI r31, 0x00");	// clear Z
	asm volatile("ANDI r30, 0x00");	// clear Z
	asm volatile("ORI r30, 0x25");	// go to PORTB reg
	asm volatile("LD r27, Z");		// load register into r28
	toggle_LED(isOn);				// either turn on/off LED
	asm volatile("ST Z, r27");		// store new values into PORTB
}

void run() {
	int delay = 500;	// time between blinks (milliseconds)
	int isOn = 1;
	char input = 'q';

	while (1) {
		if (byte_available()){
			input = read_byte();
			change_delay(input, &delay);
		}
		delay_ms(delay);
		blink_LED(&isOn);
	}
}

int main(void) {
	serial_init();
	enable_LED();
	run();
}