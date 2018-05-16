#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "ext2.h"

int main(void) {
   int fd;        // fd of file system
   char boot_block[1024];
   struct ext2_super_block super_block;

   open_file_system(&fd, boot_block);
   read_super_block(&fd, &super_block);
   printf("Blocks Count: %u\n", super_block.s_blocks_count);

   return 0;
}
