/*
Derek Lung
partner: Bevin Tang
CPE 453 Section 7
*/

#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include "globals.h"


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
        if (tos < 10){
            led_on();
            thread_sleep(produce_rate);
        }

        led_off();
        thread_sleep(produce_rate);
    }
}

void updateRates(uint8_t key) {
    switch (key) {
        case 'r' :
            produce_rate += 5;
            break;
        case 'f' :
            produce_rate -= 5;
            if(produce_rate < 5 || produce_rate > 9999)
                produce_rate = 0;
            break;
        case 'u' :
            consume_rate += 5;
            break;
        case 'j' :
            consume_rate -= 5;
            if(consume_rate < 5 || consume_rate > 9999)
                consume_rate = 0;
            break;
        default:
            break;
    }
}

void captureButton() {
    if(byte_available()) {
        uint8_t key = read_byte();
        updateRates(key);
    }
}

void display_bounded_buffer() {
    while(1) {
        captureButton();
        mutex_lock(&cursor);


        // move to right side of screen
        set_color(WHITE);
        set_cursor(4, BUF_PADDING + 2);
        print_string("Bounded Buffer Stack:");

        for (int i = 0; i < BBUF_SIZE; i++ ) {
            set_cursor(BBUF_SIZE - i + 5, BUF_PADDING);
            print_string("  "); // delete +/- from animation
            print_int(i);
            print_string(": ");
            print_int(bbuf[i]);
        }
        set_cursor(17, BUF_PADDING + 2);
        print_string("Producer Delay: ");
        print_int_digs(produce_rate, 3);
        print_string("0");

        set_cursor(18, BUF_PADDING + 2);
        print_string("Consumer Delay: ");
        print_int_digs(consume_rate, 3);
        print_string("0");

        mutex_unlock(&cursor);
    }
}

void draw_animation(uint8_t sender){
    char* symbol = "+";     // default symbol is for producer

    // Change symbol if consumer is calling
    if (sender == CONSUMER)
        symbol = "-";

    // Lock cursor control
    mutex_lock(&cursor);

    // Print symbol at proper spot. Clear rest
    for (int i = 0; i < BBUF_SIZE; i++){
        if (i == tos){
            if (sender == CONSUMER){
                set_color(RED);
                set_cursor(BBUF_SIZE - i + 5, BUF_PADDING + 7);
                print_string(symbol);
                set_cursor(BBUF_SIZE - i + 5, BUF_PADDING - 1);
                print_string(" ");
            }
            else {
                set_color(YELLOW);
                set_cursor(BBUF_SIZE - i + 5, BUF_PADDING - 1);
                print_string(symbol);
                set_cursor(BBUF_SIZE - i + 5, BUF_PADDING + 7);
                print_string(" ");
            }
        }
        else{
            set_cursor(BBUF_SIZE - i + 5, BUF_PADDING - 1);
            print_string(" ");
            set_cursor(BBUF_SIZE - i + 5, BUF_PADDING + 7);
            print_string(" ");
        }
    }

    // Give back cursor control
    mutex_unlock(&cursor);
}

void producer() {
    while(1) {
        thread_sleep(produce_rate);
        // wait for bounded buffer to have space
        sem_wait(&empty);

        // wait for producer's turn to act on buffer
        // this is to avoid producing and consuming at the same time
        sem_wait(&add_remove);

        // Draw animation before write to avoid post-decrement
        draw_animation(PRODUCER);

        // Add item to buffer
        bbuf[tos++] = 1;

        // Allow consumer to take from buffer
        sem_signal(&add_remove);

        // Signify that there is an item to take from buffer
        sem_signal(&full);
    }
}

void consumer() {
    while(1) {
        thread_sleep(consume_rate);
        // Do not access buffer until there is something to grab
        sem_wait(&full);

        // Do not access buffer while it is in use
        sem_wait(&add_remove);

        // remove item from buffer
        // if(tos - 1 > 10) tos = 10;
        bbuf[--tos] = 0;

        // Draw animation after because of preincrement
        draw_animation(CONSUMER);

        // relinquish lock on bounded buffer
        sem_signal(&add_remove);

        // Signify that there is a new empty slot in bounded buffer
        sem_signal(&empty);
    }
}

char* map_state(thread_t thd) {
    char* result;
    switch (thd.state) {
        case THREAD_RUNNING:
            result = "THREAD_RUNNING";
            break;
        case THREAD_READY:
            result = "THREAD_READY   ";
            break;
        case THREAD_SLEEPING:
            result = "THREAD_SLEEPING";
            break;
        case THREAD_WAITING:
            result = "THREAD_WAITING ";
            break;
        default:
            result = "FOO            ";
            break;
    }

    return result;
}

void printHelp(uint8_t thread, uint16_t func)
{
    thread_t thd = OS.threads[thread];

    set_cursor((thread)*SECTION_SIZE + 2, 1);
    set_color(MAGENTA);
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

    // Second column
    set_cursor((thread)*SECTION_SIZE + 4, COL_PADDING);
    set_color(GREEN);
    print_string("Top of Stack: ");
    set_color(CYAN);
    print_hex((uint16_t) thd.sp);

    set_cursor((thread)*SECTION_SIZE + 5, COL_PADDING);
    set_color(GREEN);
    print_string("Stack Base: ");
    set_color(CYAN);
    print_hex((uint16_t) thd.base);

    set_cursor((thread)*SECTION_SIZE + 6, COL_PADDING);
    set_color(GREEN);
    print_string("Stack End: ");
    set_color(CYAN);
    print_hex((uint16_t) thd.base + thd.stack_size);

    // Other Specifications
    set_cursor((thread)*SECTION_SIZE + 7, COL_PADDING);
    set_color(GREEN);
    print_string("Thread Status: ");
    set_color(CYAN);
    print_string(map_state(thd));

    set_cursor((thread)*SECTION_SIZE + 8, COL_PADDING);
    set_color(GREEN);
    print_string("Sched_count:");
    set_color(CYAN);
    print_int_digs(thd.sched_count, 3);
}

void printStatus() {
    static uint32_t time = 0;
    while(1){
        // Lock cursor to prevent cursor hijacking
        mutex_lock(&cursor);
            set_cursor(1,1);
            set_color(GREEN);
            print_string("OS Time: ");
            set_color(CYAN);
    	    print_int32(OS.time);
            set_color(GREEN);
            print_string("\nNumber of Threads: ");
            set_color(CYAN);
            print_int32(numThreads);
        mutex_unlock(&cursor);


        for(int i = 0; i < numThreads; i++) {
            mutex_lock(&cursor);
            printHelp(i, (uint16_t) OS.threads[i].func_addr);
            mutex_unlock(&cursor);
        }
	}
}

void print_mutex(mutex_t* m){
    print_string("    \nMutex:\n    Owner: ");
    print_int(m->owner_id);
    print_string("    \n    Front: ");
    print_int(m->front);
    print_string("    \n    End: ");
    print_int(m->end);
    print_string("    \n    Num_Waiting: ");
    print_int(m->num_waiting);

    print_string("    \n    Waitlist:\n");
    for (int i = 0; i < MAX_WAITLIST; i++){
        print_string("    ");
        print_int(i);
        print_string(": ");
        print_int(m->waitlist[i]);
        print_string("\n");
    }
    print_string("\n\n");
}

void print_semaphore(semaphore_t* m){
    print_string("    \nSemaphore:\n    Value: ");
    print_int(m->value);
    print_string("    \n    Front: ");
    print_int(m->front);
    print_string("    \n    End: ");
    print_int(m->end);
    print_string("    \n    Num_Waiting: ");
    print_int(m->num_waiting);

    print_string("    \n    Waitlist:\n");
    for (int i = 0; i < MAX_WAITLIST; i++){
        print_string("    ");
        print_int(i);
        print_string(": ");
        print_int(m->waitlist[i]);
        print_string("\n");
    }
    print_string("\n\n");
}

void bb_init() {
    tos = 0;
    sem_init(&add_remove, 1);
    sem_init(&empty, BBUF_SIZE);
    sem_init(&full, 0);
    mutex_init(&cursor);
    produce_rate = 50;
    consume_rate = 50;

    // initialize Bounded Buffer contents to 0
    for (int i = 0; i < BBUF_SIZE; i++)
        bbuf[i] = 0;
}

int main(void) {
    // Setup OS
    os_init();
    uint32_t delay = 50; // timer count delay
    create_thread("Idle", (uint16_t) NULL, NULL, 0);
    create_thread("Status", (uint16_t) printStatus, NULL, 256);
    create_thread("Blink LED", (uint16_t) blinkLED, &delay, 64);
    create_thread("Producer", (uint16_t) producer, NULL, 64);
    create_thread("Consumer", (uint16_t) consumer, NULL, 64);
    create_thread("Display Buff", (uint16_t) display_bounded_buffer,
        NULL, 128);

    clear_screen();

    // Setup Bounded Buffer
    bb_init();

    // Start OS
    os_start();

    while(1)
    {


    }
    return 0;
}
