#include <avr/io.h>
#include "globals.h"

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
   set_cursor(1,1);
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

// prints a string to serial COM
void print_string(char * s)
{
    uint8_t i = 0;
    while(1)
    {
        if(s[i] == '\n')
        {
            write_byte(0x1B);
            write_byte('E');
        }
        else if(s[i] == '\0')
            break;
        else
            write_byte(s[i]);
        ++i;
    }
}

// prints a 8 bit or 16 bit unsigned integer to serial COM3
void print_int(uint16_t i)
{
    uint8_t num[5];
    int8_t index = 0;
    if(i == 0)
        write_byte('0');
    while(i > 0)
    {
        num[index] = i % 10;
        i /= 10;
        index++;
    }
    for(index = index - 1; index >= 0; index--)
        write_byte(num[index] + '0');
}

// prints a 32 bit unsigned int to serial COM
void print_int32(uint32_t i)
{
    uint8_t num[10];
    int8_t index = 0;
    if(i == 0)
        write_byte('0');
    while(i > 0)
    {
        num[index] = i % 10;
        i /= 10;
        index++;
    }
    for(index = index - 1; index >= 0; index--)
        write_byte(num[index] + '0');
}

// prints a 16 bit unsigned int in hex to serial COM
void print_hex(uint16_t i)
{
    uint8_t hexCode;
    uint8_t shift = 12;

    print_string("0x");
    for(uint8_t ind = 0; ind < 4; ind++)
    {
        hexCode = (i >> shift) & 0x0F;
        if(hexCode > 9)
            write_byte('F' - (15 - hexCode));
        else
            write_byte(hexCode + '0');
        shift -= 4;
    }
}

void print_hex32(uint32_t i)
{
    uint8_t hexCode;
    uint8_t shift = 28;

    print_string("0x");
    for(uint8_t ind = 0; ind < 8; ind++)
    {
        hexCode = (i >> shift) & 0x0F;
        if(hexCode > 9)
            write_byte('F' - (15 - hexCode));
        else
            write_byte(hexCode + '0');
        shift -= 4;
    }
}

void set_cursor(uint8_t row, uint8_t col)
{
    write_byte(0x1B);
    print_string("[");
    print_int(row);
    print_string(";");
    print_int(col);
    print_string("H");
}

void set_color(uint8_t color)
{
    write_byte(0x1B);
    write_byte('[');
    print_int(color);
    write_byte('m');
}
void clear_screen(void)
{
    write_byte(0x1B);
    print_string("[2J");
}
