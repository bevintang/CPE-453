#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "ext2.h"

//the block argument is in terms of SD card 512 byte sectors
void read_data(uint32_t block, uint16_t offset, uint8_t* data, uint16_t size) {
   if (offset > 511) {
      printf ("Offset greater than 511.\n");
      exit(0);
   }

   fseek(fp,block*512 + offset,SEEK_SET);	// set fp to a spot in disk
   fread(data,size,1,fp);					// read fp and write to data ptr
}

void read_super_block (uint8_t block_group, struct ext2_super_block* sp) {
	size_t size = sizeof(struct ext2_super_block);

	// Read in super block for a specified Block Group for first half
    read_data((block_group * BLOCK_GROUP_SIZE) + 2, 0, (uint8_t*)sp, size);
}
