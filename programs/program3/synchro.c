#include <stdio.h>
#include "globals.h"

/* Queue Implementation */
uint8_t q_get_next(uint8_t current){
    return (current+1) % MAX_WAITLIST;
}

// void print_queue(uint8_t* arr){
//  printf("\nQueue:\n");
//  for (int i = 0; i < MAX_WAITLIST; i++)
//      printf("%d: %u\n", i, arr[i]);
//  printf("\n\n");
// }

void q_add(uint8_t* arr, uint8_t* end, uint8_t n) {
    arr[*end] = n;
    *end = q_get_next(*end);
}

void q_place(uint8_t* arr, uint8_t index, uint8_t n) {
    arr[index] = n;
}

uint8_t q_pop(uint8_t* arr, uint8_t* front){
    uint8_t removed = arr[*front];

    // Reset to default
    arr[*front] = DEFAULT_ID;

    // Increment next current index
    *front = q_get_next(*front);

    // return what was removed
    return removed;
}

void yield() {
    // Prepare for context_switch
    cli();
    uint16_t * old_sp = (uint16_t*)&(OS.threads[OS.id].sp);
    uint8_t next = get_next_thread();
    uint16_t * new_sp = (uint16_t*)&(OS.threads[next].sp);
    context_switch(new_sp, old_sp);
    sei();
}

void idle() {
    while(OS.threads[OS.id].state == THREAD_WAITING)
    {
        print_string("");
    }    
}
void mutex_init(mutex_t *m) {
    // set owner_id to DEFAULT_ID
    m->owner_id = DEFAULT_ID;

    // empty waitlist by setting everything to DEFAULT_ID
    for (int i = 0; i < MAX_WAITLIST; i++)
        m->waitlist[i] = DEFAULT_ID;

    // Set default values for struct
    m->front = m->end = m->num_waiting = 0;
}

void mutex_lock(mutex_t *m) {
    // disbale interrupts for 'atomic' operation
    cli();

    // check current OS ID
    uint8_t curID = get_thread_id();

    // check if mutex is open
    // obtain lock and set mutex owner to current os
    // mutex is now locked to this thread until it calls unlock
    if (m->owner_id == DEFAULT_ID){
        m->owner_id = curID;
    }

    // if mutex id is not DEFAULT_ID then mutex is locked
    // check if it is full if it is do nothing
    // Otherwise, add current OS ID to waitlist using queue
    // set thread to waiting
    else if (m->num_waiting < MAX_WAITLIST){
        q_add(m->waitlist, &(m->end), curID);
        m->num_waiting++;
        OS.threads[curID].state = THREAD_WAITING;
        yield();
    }

    // Re-enable interrupts
    sei();
    
}

void mutex_unlock(mutex_t *m) {
    cli();

    // check current OS ID
    uint8_t curID = get_thread_id();
    if (m->owner_id == curID){
        // If there are waiting threads,
        // make the owner the next waiting thread
        if (m->num_waiting > 0){
            m->owner_id = q_pop(m->waitlist, &(m->front));
            OS.threads[m->owner_id].state = THREAD_READY;
            m->num_waiting--;
        }

        // Else, no mutex should be freed
        else {
            m->owner_id = DEFAULT_ID;
        }
    }
    sei();

    // unlock mutex if owner_id == OS ID
        // if waitlist is empty
        // set ownder_id to DEFAULT_ID

        // else get the first id in the waitlist and set that to the owner id
        // wake up that thread

}

void sem_init(semaphore_t *s, int8_t value) {
    // init value to parameter
    s->value = value;

    // init waitlist to DEFAULT_ID
    for (int i = 0; i < MAX_WAITLIST; i++)
        s->waitlist[i] = DEFAULT_ID;

    // Set default values
    s->front = s->end = s->num_waiting = 0;
}

void sem_wait(semaphore_t *s) {
    cli();
    // decrement semaphore value
    s->value--;

    // Add to waitlist if value < 0
    uint8_t curID = OS.id;

    if (s->value < 0 && s->num_waiting < MAX_WAITLIST) {
        q_add(s->waitlist, &(s->end), curID);
        s->num_waiting++;
        OS.threads[curID].state = THREAD_WAITING;
        yield();
    }
    sei();
    
}

void sem_signal(semaphore_t *s) {
    cli();
    // increment semaphore value
    s->value++;

    // check if value is less or equal to zero
    // (Check to see if a thread is waiting)
    if (s->value <= 0 && s->num_waiting != 0) {
        uint8_t popped = q_pop(s->waitlist, &(s->front));
        OS.threads[popped].state = THREAD_READY;
        s->num_waiting--;
    }
    sei();
}

void sem_signal_swap(semaphore_t *s) {
    cli();
    // increment semaphore value
    s->value++;

    // check if value is less or equal to zero
    // (Check to see if a thread is waiting)
    if (s->value <= 0) {
        uint8_t popped = q_pop(s->waitlist, &(s->front));
        OS.threads[popped].state = THREAD_READY;
        s->num_waiting--;

        // Prepare for context_switch
        uint16_t * old_sp = (uint16_t*)&(OS.threads[OS.id].sp);
        uint16_t * new_sp = (uint16_t*)&(OS.threads[popped].sp);
        OS.id = popped;
        context_switch(new_sp, old_sp);
    }
    sei();

    // check if value is less or equal to zero
        // remove thread from waitlist
        // wake up thread and set it to ready
        // context switch in this thread immediatly
        // set OS ID to new id

}
