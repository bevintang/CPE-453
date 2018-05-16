/*
Derek Lung
partner: Bevin Tang
CPE 453 Section 7
*/

#ifndef OS_H
#define OS_H

typedef struct thread_t {
    char * name;
    uint8_t id;
    uint16_t * sp;
    uint16_t * base;
    uint16_t stack_size;
    uint8_t state;
    uint16_t ticks;
    uint16_t func_addr;
    uint16_t count;
    uint16_t sched_count;
} thread_t;

typedef struct system_t {
    thread_t * threads;
    uint8_t id;
    uint32_t time;
    uint32_t count;
} system_t;

//This structure defines the register order pushed to the stack on a
//system context switch. Size 22 Bytes
typedef struct regs_context_switch {
    //stack pointer is pointing to 1 byte below the top of the stack
    uint8_t padding;

    //Registers that will be managed by the context switch function
    uint8_t r29;
    uint8_t r28;
    uint8_t r17;
    uint8_t r16;
    uint8_t r15;
    uint8_t r14;
    uint8_t r13;
    uint8_t r12;
    uint8_t r11;
    uint8_t r10;
    uint8_t r9;
    uint8_t r8;
    uint8_t r7;
    uint8_t r6;
    uint8_t r5;
    uint8_t r4;
    uint8_t r3;
    uint8_t r2;
    uint8_t eind;
    uint8_t pch;
    uint8_t pcl;
} regs_context_switch;

// This structure defines how registers are pushed to the stack when
// the system 10ms interrupt occurs.
// This struct is never directly used,
// but instead be sure to account for the size of this struct
// when allocating initial stack space
typedef struct regs_interrupt {
    uint8_t padding; //stack pointer is pointing to 1 byte below the top of the stack

    //Registers that are pushed to the stack during an interrupt service routine
    uint8_t r31;
    uint8_t r30;
    uint8_t r27;
    uint8_t r26;
    uint8_t r25;
    uint8_t r24;
    uint8_t r23;
    uint8_t r22;
    uint8_t r21;
    uint8_t r20;
    uint8_t r19;
    uint8_t r18;
    uint8_t rampz; //rampz
    uint8_t sreg;  //status register
    uint8_t r0;
    uint8_t r1;
    uint8_t eind;
    uint8_t pch;
    uint8_t pcl;
} regs_interrupt;

enum State {
    THREAD_RUNNING,
    THREAD_READY,
    THREAD_SLEEPING,
    THREAD_WAITING
};

void os_init(void);
void os_start(void);
void create_thread(char * name, uint16_t address,
    void * args, uint16_t stack_size);

uint8_t get_thread_id();
uint8_t get_next_thread(void);

__attribute__((naked)) void context_switch(uint16_t * new_sp,
    uint16_t * old_sp);

__attribute__((naked)) void thread_start(void);

void thread_sleep(uint16_t ticks);

#endif
