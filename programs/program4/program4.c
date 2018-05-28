#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "ext2.h"

#define BOOT_BLOCK_SIZE 2

int main(void) {
	struct ext2_super_block sup_block;
	struct ext2_group_desc desc;


	fp = fopen("testimage.ext2", "r");
	// uint8_t data[sizeof(struct ext2_super_block)];

	// Read first super block
	read_data(0 + BOOT_BLOCK_SIZE, 0, (uint8_t*)&sup_block, sizeof(struct ext2_super_block));
	printf("Inodes Count: %u\n", sup_block.s_inodes_count);
	printf("Blocks Count: %u\n", sup_block.s_blocks_count);
	printf("Blocks/Group: %u\n", sup_block.s_blocks_per_group);

	// Read First Group Descriptor
	uint32_t firstGroupDesc = 2 * 1 + BOOT_BLOCK_SIZE;
	read_data(firstGroupDesc, 0, (uint8_t*)&desc, sizeof(struct ext2_group_desc));
	printf("Inodes Table Block: %u\n", desc.bg_inode_table);

	// Obtain root inode
	uint32_t inode_table_block = desc.bg_inode_table * 2 + BOOT_BLOCK_SIZE;
	struct ext2_inode root;
	read_data(inode_table_block, 0, (uint8_t*)&root, sizeof(struct ext2_inode));
	printf("Root File Mode: %u\n", root.i_mode);

   return 0;
}
