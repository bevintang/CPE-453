/*
Derek Lung
partner: Bevin Tang
CPE 453 Section 7
*/

#ifndef SYNCHRO_H
#define SYNCRHO_H

#include <avr/interrupt.h>

#define DEFAULT_ID 255
#define MAX_WAITLIST 10

typedef struct mutex_t {
    uint8_t owner_id;
    uint8_t waitlist[MAX_WAITLIST];
    uint8_t front;
    uint8_t end;
    uint8_t num_waiting;
} mutex_t;

typedef struct semaphore_t {
    int8_t value;
    uint8_t waitlist[MAX_WAITLIST];
    uint8_t front;
    uint8_t end;
    uint8_t num_waiting;
} semaphore_t;

void mutex_init(mutex_t *m);
void mutex_lock(mutex_t *m);
void mutex_unlock(mutex_t *m);

void sem_init(semaphore_t *s, int8_t value);
void sem_wait(semaphore_t *s);
void sem_signal(semaphore_t *s);
void sem_signal_swap(semaphore_t *s);
void yield();




#endif
