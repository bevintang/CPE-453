#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "ext2.h"

int main(void) {
   struct ext2_super_block sup_block;


   fp = fopen("testimage.ext2", "r");
   // uint8_t data[sizeof(struct ext2_super_block)];

   read_super_block(3, &sup_block);
   printf("Inodes Count: %u\n", sup_block.s_inodes_count);
   printf("Blocks Count: %u\n", sup_block.s_blocks_count);
   printf("Blocks/Group: %u\n", sup_block.s_blocks_per_group);
   printf("Blocks  Size: %u\n", sup_block.s_log_block_size);

   return 0;
}
