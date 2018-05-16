/*
Derek Lung
partner: Bevin Tang
CPE 453 Section 7
*/

#ifndef GLOBALS_H
#define GLOBALS_H

#include <avr/io.h>
#include "os.h"
#include "synchro.h"
//place defines and prototypes here

// OS
system_t OS;
thread_t threads[9];
uint8_t numThreads;

// Bounded Buffer
uint8_t bbuf[10];	// implemented as a stack
uint8_t tos;

uint16_t produce_rate;
uint16_t consume_rate;

// Producer/Consumer Control
semaphore_t add_remove;
semaphore_t empty;
semaphore_t full;

semaphore_t sort_wait;
mutex_t sort_reset;

mutex_t cursor;
#define COL_PADDING 30
#define BUF_PADDING 70
#define SECTION_SIZE 6

#define BBUF_SIZE 10
#define PRODUCER 0
#define CONSUMER 1

#define RED 31
#define GREEN 32
#define YELLOW 33
#define BLUE 34
#define MAGENTA 35
#define CYAN 36
#define WHITE 37

void serial_init();
uint8_t byte_available();
uint8_t read_byte();
uint8_t write_byte(uint8_t b);

// implemented functions
void print_string(char * s);
void print_int(uint16_t i);
void print_int_digs(uint16_t i, uint8_t digs);
void print_int32(uint32_t i);
void print_hex(uint16_t i);
void print_hex32(uint32_t i);
void set_cursor(uint8_t row, uint8_t col);
void set_color(uint8_t color);
void clear_line(void);
void clear_screen(void);
#endif
