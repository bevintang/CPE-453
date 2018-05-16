#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "globals.h"
#include "os.h"

//This interrupt routine is automatically run every 10 milliseconds
ISR(TIMER0_COMPA_vect) {
    //At the beginning of this ISR, the registers r0, r1, and r18-31 have
    //already been pushed to the stack

    //The following statement tells GCC that it can use registers r18-r31
    //for this interrupt routine.  These registers (along with r0 and r1)
    //will automatically be pushed and popped by this interrupt routine.
    asm volatile ("" : : : "r18", "r19", "r20", "r21", "r22", "r23", "r24", \
                 "r25", "r26", "r27", "r30", "r31");

    //Insert your code here
    //Call get_next_thread to get the thread id of the next thread to run
    //Call context switch here to switch to that next thread
    uint16_t * old_sp = (uint16_t*)&(OS.threads[OS.id].sp);
    uint8_t next_thread = get_next_thread();
    uint16_t * new_sp = (uint16_t*)&(OS.threads[next_thread].sp);

    OS.count++;
    if(OS.count % 100 == 0)
        OS.time++;
    if(OS.count == 1000000)
        OS.count = 0;
    context_switch(new_sp, old_sp);

    //At the end of this ISR, GCC generated code will pop r18-r31, r1,
    //and r0 before exiting the ISR
}

//Call this to start the system timer interrupt
void start_system_timer() {
    TIMSK0 |= _BV(OCIE0A);  //interrupt on compare match
    TCCR0A |= _BV(WGM01);   //clear timer on compare match

    //Generate timer interrupt every ~10 milliseconds
    TCCR0B |= _BV(CS02) | _BV(CS00) | _BV(CS02);    //prescalar /1024
    OCR0A = 156;             //generate interrupt every 9.98 milliseconds
}

__attribute__((naked)) void context_switch(uint16_t * new_sp, uint16_t * old_sp) {
    // store all current data into old sp stack (r2 - r17, r28-r29)
    // when old becomes new SP will point to the top of the stack
    // into Z, to push current context registers

    /** Assume that old_sp points to r2 in the thread **/
    asm volatile("PUSH r2");
    asm volatile("PUSH r3");
    asm volatile("PUSH r4");
    asm volatile("PUSH r5");
    asm volatile("PUSH r6");
    asm volatile("PUSH r7");
    asm volatile("PUSH r8");
    asm volatile("PUSH r9");
    asm volatile("PUSH r10");
    asm volatile("PUSH r11");
    asm volatile("PUSH r12");
    asm volatile("PUSH r13");
    asm volatile("PUSH r14");
    asm volatile("PUSH r15");
    asm volatile("PUSH r16");
    asm volatile("PUSH r17");
    asm volatile("PUSH r28");
    asm volatile("PUSH r29");

    // Overwrite old_sp with HW SP
    asm volatile("CLR R30");        // clear Z
    asm volatile("CLR R31");
    asm volatile("ORI R30, 0x5D");  // Point to HW SP
    asm volatile("LD R10, Z+");     // Load HW SP into 10 and 11
    asm volatile("LD R11, Z");
    asm volatile("MOV r31, r23");   // Make Z point to old sp
    asm volatile("MOV r30, r22");
    asm volatile("ST Z+, r10");     // overwrite old_sp with HW SP
    asm volatile("ST Z, r11");

    // Load new sp into HW SP
    asm volatile("MOV r31, r25");   // point Z reg at new sp
    asm volatile("MOV r30, r24");
    asm volatile("LD r10, Z+");     // load new sp into 10 and 11
    asm volatile("LD r11, Z");
    asm volatile("CLR R30");        // Change Z reg to point to HW SP
    asm volatile("CLR R31");
    asm volatile("ORI R30, 0x5D");
    asm volatile("ST Z+, r10");     // Store new sp into hardware sp
    asm volatile("ST Z, r11");

    // Pop context registers of new thread
    asm volatile("POP r29");
    asm volatile("POP r28");
    asm volatile("POP r17");
    asm volatile("POP r16");
    asm volatile("POP r15");
    asm volatile("POP r14");
    asm volatile("POP r13");
    asm volatile("POP r12");
    asm volatile("POP r11");
    asm volatile("POP r10");
    asm volatile("POP r9");
    asm volatile("POP r8");
    asm volatile("POP r7");
    asm volatile("POP r6");
    asm volatile("POP r5");
    asm volatile("POP r4");
    asm volatile("POP r3");
    asm volatile("POP r2");

    // Return to pop the PC of the thread
    asm volatile("RET");
}

__attribute__((naked)) void thread_start(void) {
    // enable interrupts - leave as the first statement in thread_start()
    sei();

    // Move args from r5 and r4 into r25 and r24
    asm volatile("MOV r25, r5");
    asm volatile("MOV r24, r4");

    // jump to function that is saved in r3 and r2
    asm volatile("MOV r31, r3"); // Fill Upper Z reg
    asm volatile("MOV r30, r2");
    asm volatile("IJMP");        // Jump to Z reg
}

void os_init(void) {
    serial_init();
    OS.threads = threads;
    OS.id = 0;
    OS.time = 0;
    OS.count = 0;
    numThreads = 0;
}

void os_start(void) {
    uint16_t * old_sp = (uint16_t*)&(OS.threads[OS.id - 1].sp);
    uint8_t next_thread = get_next_thread();
    uint16_t * new_sp = (uint16_t*)&(OS.threads[next_thread].sp);
    start_system_timer();
    context_switch(new_sp, old_sp);
}

void create_thread(char * name, uint16_t address,
    void * args, uint16_t stack_size) {

        // fills in static struct that is globaly declared
        // Create thread creates a stack for this thread
        // load function arguments into stack
        // in c
        thread_t * thread = threads + numThreads;

        // need to determine which regs holds args and address
        uint8_t * stack = (uint8_t *) malloc(stack_size + sizeof(regs_context_switch));
        thread->base = (uint16_t *) stack;
        stack += stack_size;

        // Assign values to thread struct
        thread->name = name;
        thread->id = OS.id;
        thread->sp = (uint16_t *) stack;
        thread->stack_size = stack_size + sizeof(regs_context_switch);

        // Cast stack pointer to struct type for easy assignment
        regs_context_switch * reg = (regs_context_switch *) stack;

        // r3 holds upper byte address
        // r2 holds lower byte address
        reg->r3 = address >> 8;
        reg->r2 = address & 0x00FF;

        // r5 holds upper byte args
        // r4 holds lower byte args
        reg->r5 = (uint16_t) args >> 8;
        reg->r4 = (uint16_t) args & 0x00FF;

        // Move PC of thread_start into PC bytes
        reg->eind = 0;
        reg->pch = (uint16_t) thread_start >> 8;
        reg->pcl = (uint16_t) thread_start & 0x00FF;

        // Update current id
        OS.id++;
        numThreads++;
}

uint8_t get_next_thread(void) {
    uint8_t next = OS.id + 1;

    // Loop back if incrementing past last thread
    if (next >= numThreads - 1){
        next = 0;
    }

    // Update OS struct
    OS.id = next;

    return next;
}
