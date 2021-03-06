#ifndef __FS__MBR_H
#define __FS__MBR_H

#include <common.h>
#include <mm/heap.h>
#include <drivers/ata.h>

/* Master boot record is found in the first
   sector of the hard drive. Skip the first
   436 bytes because it's just bootstrap code.
*/
typedef struct __attribute__((packed))mbr
{
	uint8_t bootstrap[436];
	uint8_t disk_id[10];
	uint8_t partition_table_entry0[16];
	uint8_t partition_table_entry1[16];
	uint8_t partition_table_entry2[16];
	uint8_t partition_table_entry3[16];
        uint16_t signature;
} mbr_t;

typedef struct partition
{
	//uint8_t active;
	//uint8_t system_id;
	uint32_t start_lba;
	uint32_t part_size;
} partition_t;

void parse_mbr(bdev_t *dev);

#endif
