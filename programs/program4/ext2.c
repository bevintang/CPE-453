#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "ext2.h"

//the block argument is in terms of SD card 512 byte sectors
void read_data(uint32_t block, uint16_t offset, uint8_t* data, uint16_t size, FILE* fp) {
   if (offset > 511) {
      printf ("Offset greater than 511.\n");
      exit(0);
   }

   fseek(fp,block*512 + offset,SEEK_SET);
   fread(data,size,1,fp);
}

void open_file_system(int* fd, char* boot_block){
   *fd = open("testimage.ext2", O_RDONLY);
   read(*fd, boot_block, 1024);
}

void read_super_block (int* fd, struct ext2_super_block* sp) {
   read(*fd, sp, sizeof(struct ext2_super_block));
}
