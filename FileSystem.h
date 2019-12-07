#pragma once
#include <stdint.h>
extern bool isMounted;
struct Inode{
	char name[6];        // Name of the file or directory
	uint8_t used_size;   // Inode state and the size of the file or directory
	uint8_t start_block; // Index of the start file block
	uint8_t dir_parent;  // Inode mode and the index of the parent inode
};

struct SuperBlock{
	char free_block_list[16];
	Inode inode[126];
};

struct Data {
	SuperBlock supBlock;
	char data[127][1024];
};

void fs_mount(const char *new_disk_name);
void fs_create(const char name[5], int size);
void fs_delete(const char name[5]);
void fs_read(const char name[5], int block_num);
void fs_write(const char name[5], int block_num);
void fs_buff(uint8_t buff[1024]);
void fs_ls(void);
void fs_resize(const char name[5], int new_size);
void fs_defrag(void);
void fs_cd(const char name[5]);