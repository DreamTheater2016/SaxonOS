#ifndef __FS__DEVFS__BUFFER_H
#define __FS__DEVFS__BUFFER_H

#include <common.h>

#include <mm/heap.h>
#include <fs/devfs/devfs.h>
#include <drivers/ata.h>

#define MAX_BUFFER 100

#define CLASS_REG_FILE 0
#define CLASS_DIR 1

struct buffer_head
{
	uint8_t flags;
	uint32_t block_num;
	bool dirty;
	device_t dev;
	size_t count;
	struct buffer_head *next;
	struct buffer_head *prev;
	struct buffer_head *next_free;
	struct buffer_head *prev_free;
	char buff[BLOCK_SIZE];
};

void init_buffer(void);
struct buffer_head *get_block(uint32_t block, device_t dev);
void put_block(struct buffer_head *block);

#endif
