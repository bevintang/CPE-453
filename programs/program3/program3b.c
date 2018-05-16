#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include "globals.h"



uint8_t owner = 0;
uint8_t done = 0;

// uint8_t left_size;
// uint8_t right_size;
//
//
// uint8_t left[64];
// uint8_t right[64];

uint8_t * left1;
uint8_t * right1;

void merge(uint8_t * arr, int l, int m, int r) {
    // split array into two halves
    // find size of two arrays
    uint8_t left_size = m - l + 1;
    uint8_t right_size = r - m;

    uint8_t left[left_size];
    uint8_t right[right_size];

    if(get_thread_id() == owner) {
        left1 = left;
        right1 = right;
    }

    int i,j,k;

    // fill left array
    for(int i = 0; i < left_size; i++) {
        left[i] = arr[l + i];
    }

    // fill right array
    for(int j = 0; j < right_size; j++) {
        right[j] = arr[m + 1 + j];
    }
    // sort the two arrays into the original array

    i = j = 0;
    k = l;
    while(i < left_size && j < right_size) {
        // if left is smaller then add it to array
        if(left[i] < right[j]) {
            arr[k] = left[i];
            i++;
        }
        // if right is smaller then add it to array
        else {
            arr[k] = right[j];
            j++;
            // slow down sort to visualize it
        }
        k++;
        // slow down sort to visualize it
        thread_sleep(5);
    }

    // if left is not empty put it into the array
    while(i < left_size) {
        arr[k] = left[i];
        i++;
        k++;
        // slow down sort to visualize it
        thread_sleep(5);
    }
    // if right is not empty put it into the array
    while(j < right_size) {
        arr[k] = right[j];
        j++;
        k++;
        // slow down sort to visualize it
        thread_sleep(5);
    }

}

void mergeSort(uint8_t * arr, int l, int r) {
    if (l < r) {
        // find middle
        int m = l+(r-l)/2;

        // Sort first and second halves
        mergeSort(arr, l, m);
        mergeSort(arr, m+1, r);

        // merge the halves
        merge(arr, l, m, r);
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
    print_int_digs(((uint16_t) thd.base + thd.stack_size) - (uint16_t) thd.sp, 3);
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

void print_Help_array2(uint8_t * array) {
	print_string("[");
	for(int i = 0; i < 32; i++)
	{
		print_int_digs(array[i], 3);
		print_string(" ");
	}
	print_string("]");
}

void print_Help_array3(uint8_t * array) {
	print_string("[");
	for(int i = 32; i < 64; i++)
	{
		print_int_digs(array[i], 3);
		print_string(" ");
	}
	print_string("]");
}

void print_Help_array(uint8_t * array, uint8_t section) {
	uint8_t max = section * 32;
	print_string("[");
	for(int i = max - 32; i < max; i++)
	{
		print_int_digs(array[i], 3);
		print_string(" ");
	}
	print_string("]");
}

void display_array(uint8_t * array) {
	while(1) {
        mutex_lock(&cursor);
		// 1st quarter
		set_cursor(45,1);
        set_color(CYAN);
		print_Help_array(array, 1);
		// 2nd quarter
		set_cursor(46,1);
        set_color(RED);
		print_Help_array(array, 2);
		// 3rd quarter
		set_cursor(47,1);
        set_color(MAGENTA);
		print_Help_array(array, 3);
		// 4th quarter
		set_cursor(48,1);
        set_color(GREEN);
		print_Help_array(array, 4);

        set_cursor(50,1);
        set_color(GREEN);
        print_Help_array2(left1);

        set_cursor(51,1);
        set_color(GREEN);
        print_Help_array3(left1);

        set_cursor(52,1);
        set_color(RED);
        print_Help_array2(right1);

        set_cursor(53,1);
        set_color(RED);
        print_Help_array3(right1);

        //status
        set_cursor(54,1);
        set_color(GREEN);
        print_string("Status: ");
        if(done) {
            clear_line();
            print_string(" DONE");
        }
        else {
            print_string(" SORTING");
        }
        mutex_unlock(&cursor);
	}

}

void split_array(uint8_t * left_ind, uint8_t * right_ind) {
	*right_ind = get_thread_id() * 32 - 1;
	*left_ind = *right_ind - 31;

}

void populate(uint8_t * array) {
	for(int i = 0; i < 128; i++)
		array[i] = rand() % (255 + 1);
}

void mt_sort(uint8_t * array) {

	uint8_t left_ind, right_ind;
    while(1) {

        if(owner == 0) {
    		owner = get_thread_id();

            // master locks thread so all threads must
            // wait for master to unlock
            mutex_lock(&sort_reset);
    	}

    	// set left and right to a quarter of the array
    	split_array(&left_ind, &right_ind);
    	// use merge_sort
    	mergeSort(array, left_ind, right_ind);

    	// sorting quarter done signal that this thread is done
    	sem_signal(&sort_wait);

    	// if owner thread do this:
    	// semaphore with a value of negative 4 to signal all sorting is done

    	if(owner == get_thread_id()) {
    		// wait for all threads to finish sorting
    		sem_wait(&sort_wait);

    		// merge all arrays
    		merge(array, 0, 31, 63);
    		merge(array, 64, 95, 127);
    		merge(array, 0, 63, 127);

            done = 1;
            // sleep for 300 seconds
    		thread_sleep(500);

            // randomize array
            populate(array);

            thread_sleep(250);

            done = 0;

            owner = 0;

            // master unlocks so all threads can reset
            mutex_unlock(&sort_reset);
    	}
        // all threads need to wait for master
        // to finish merging before resetting

        mutex_lock(&sort_reset);
        mutex_unlock(&sort_reset);
    }

}

int main(void) {

	uint8_t array[128];
	populate(array);
    mutex_init(&cursor);
	sem_init(&sort_wait, -3);
    mutex_init(&sort_reset);

	os_init();
    write_byte(0x1B);
    print_string("[?25l");
    create_thread("Idle", (uint16_t) NULL, NULL, 0);
	create_thread("Sort_1", (uint16_t) mt_sort, &array, 512);
    create_thread("Sort_2", (uint16_t) mt_sort, &array, 512);
    create_thread("Sort_3", (uint16_t) mt_sort, &array, 512);
    create_thread("Sort_4", (uint16_t) mt_sort, &array, 512);
	create_thread("Display_Array", (uint16_t) display_array, &array, 128);
    create_thread("Status", (uint16_t) printStatus, NULL, 256);
	clear_screen();

	// Start OS
	os_start();

	while(1) {

	}
	return 0;
}
