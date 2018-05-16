
#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include "os.h"
#include <util/delay.h>



void led_on()
{
    // set ptr
    asm volatile ("LDI r30, 0x24");
    asm volatile ("CLR r31");
    // set DDBC
    asm volatile ("LD r18, Z");
    asm volatile ("ORI r18, 0x80");
    asm volatile ("ST Z+, r18");
    asm volatile ("LD r18, Z");
    asm volatile ("ORI r18, 0x80");
    asm volatile ("ST Z, r18");
}

void led_off()
{
    // set ptr
    asm volatile ("LDI r30, 0x24");
    asm volatile ("CLR r31");
    // set DDBC
    asm volatile ("LD r18, Z");
    asm volatile ("ORI r18, 0x80");
    asm volatile ("ST Z+, r18");
    asm volatile ("LD r18, Z");
    asm volatile ("ANDI r18, 0x7F");
    asm volatile ("ST Z, r18");
}

void delay_func(uint32_t * delay) {
    uint32_t tempTime = OS.count;
    while(OS.count - tempTime < *delay)
    {
        print_string("");
    }
}
void blinkLED(uint32_t * delay)
{
    while(1)
    {
        led_on();
        delay_func(delay);
        led_off();
        delay_func(delay);
    }
}

void printHelp(uint8_t thread, uint16_t func)
{
    thread_t thd = OS.threads[thread];
    set_color(GREEN);
    print_string("\n\nThread ID: ");
    set_color(CYAN);
    print_int(thd.id);
    set_color(GREEN);
    print_string("\nThread Name: ");
    set_color(CYAN);
    print_string(thd.name);
    set_color(GREEN);
    print_string("\nThread PC ");
    set_color(CYAN);
    print_int((uint16_t) func);
    set_color(GREEN);
    print_string("\nStack Usage: ");
    set_color(CYAN);
    print_int32(((uint16_t) thd.base + thd.stack_size) - (uint16_t) thd.sp);
    set_color(GREEN);
    print_string("\nTotal Stack Size: ");
    set_color(CYAN);
    print_int32(thd.stack_size);
    set_color(GREEN);
    print_string("\nCurrent Top of Stack: ");
    set_color(CYAN);
    print_hex((uint16_t) thd.sp);
    set_color(GREEN);
    print_string("\nStack Base: ");
    set_color(CYAN);
    print_hex((uint16_t) thd.base);
    set_color(GREEN);
    print_string("\nStack End: ");
    set_color(CYAN);
    print_hex((uint16_t) thd.base + thd.stack_size);
}

void printStatus() {
    static uint32_t time = 0;
    while(1){
        set_cursor(1,1);
        set_color(GREEN);
        print_string("OS Time: ");
        set_color(CYAN);
	    print_int32(OS.time);
        set_color(GREEN);
        print_string("\nNumber of Threads: ");
        set_color(CYAN);
        print_int32(numThreads - 1);
        printHelp(0, (uint16_t) printStatus);
        printHelp(1, (uint16_t) blinkLED);
	}
}

int main(void) {
    os_init();
    uint32_t x = 2;
    create_thread("Status", (uint16_t) printStatus, NULL, 64);
    create_thread("LED", (uint16_t) blinkLED, &x, 256);
    create_thread("main", (uint16_t) NULL, NULL, 0);
    clear_screen();
    os_start();

    while(1)
    {

    }
    return 0;
}
