#include <stdio.h>
#include <avr/io.h>
#include "globals.h"

// Private function headers
int mapHex(uint32_t c);
void print_base32(uint32_t i, int base);

/*
 * Initialize the serial port.
 */
void serial_init() {
   uint16_t baud_setting;

   UCSR0A = _BV(U2X0);
   baud_setting = 16; //115200 baud

   // assign the baud_setting
   UBRR0H = baud_setting >> 8;
   UBRR0L = baud_setting;

   // enable transmit and receive
   UCSR0B |= (1 << TXEN0) | (1 << RXEN0);

   // clear screen
   clear_screen();
}

/*
 * Return 1 if a character is available else return 0.
 */
uint8_t byte_available() {
   return (UCSR0A & (1 << RXC0)) ? 1 : 0;
}

/*
 * Unbuffered read
 * Return 255 if no character is available otherwise return available character.
 */
uint8_t read_byte() {
   if (UCSR0A & (1 << RXC0)) return UDR0;
   return 255;
}

/*
 * Unbuffered write
 *
 * b byte to write.
 */
uint8_t write_byte(uint8_t b) {
   //loop until the send buffer is empty
   while (((1 << UDRIE0) & UCSR0B) || !(UCSR0A & (1 << UDRE0))) {}

   //write out the byte
   UDR0 = b;
   return 1;
}

/*
 * Send VTC100 ESC Code to clear screen
 */
void clear_screen() {
    // Clear contents
    write_byte(0x1B);
    write_byte('[');

    // Move cursor back to home position
    write_byte('H');
}

/*
 * Print a string to the screen
 */
void print_string(char* s) {
    int numChars = 0;
    char curChar = 'a';

    // Get number of chars
    while ((curChar = s[numChars++]) != '\0'){
        if (curChar == '\n'){
            write_byte(0x1B);
            write_byte('E');    // write \n and move cursor to next line
        }
        else{
            write_byte(curChar);
        }
    }
}

/*
 * print an unsigned int to the screen
 */
void print_int32(uint32_t i) {
    print_base32(i, 10);
}
void print_int(uint16_t i) {
    print_base32(i, 10);
}

/*
 * print an unsigned int to the screen as hex
 */
void print_hex32(uint32_t i) {
    print_base32(i, 16);
}
void print_hex(uint16_t i) {
    print_base32(i, 16);
}

/*
 * Move the position of the cursor to the specified row, col
 * Utilize VTC100 ESC code
 */
void set_cursor(uint8_t row, uint8_t col) {
    write_byte(0x1B);
    write_byte('[');
    print_int(row);
    write_byte(';');
    print_int(col);
    write_byte('f');
}

/*
 * Set foreground color
 * Utilize VTC100 ESC code
 */
void set_color(uint8_t color) {
    write_byte(0x1B);
    write_byte('[');
    print_int(color);
    write_byte('m');
}


/** --------------- HELPER FUNCTIONS ---------------- **/
/*
 * Print an unsigned 32-bit int with any base (2 - bin, 10 - dec, etc)
 */
void print_base32(uint32_t i, int base){
    int numDigs = 0;
    uint32_t temp = i;
    char digits[33];

    while (temp > 0){
        numDigs++;
        temp = temp / base;
    }

    temp = i;
    digits[numDigs] = '\0';
    for (int j = 1; j <= numDigs; j++) {
        digits[numDigs - j] = '0' + mapHex(temp % base);
        temp = temp / base;
    }
    if (base == 16)
        print_string("0x");
    print_string(digits);
}

/*
 * Return the necessary offset from ASCII 0 to get to the desired hex number
 */
int mapHex(uint32_t c) {
    if (c < 10){
        return c;
    }
    else {
        return c + 16;
    }
}