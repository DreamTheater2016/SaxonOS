#ifndef __MM_FRAME_H
#define __MM_FRAME_H

#include <common.h>

uint32_t frame_map[0x2000];

static uint32_t nframes;

typedef uint32_t pageframe_t;

/*mem_size should be in kilobytes*/
void init_frames(uint32_t end_kernel, uint32_t mem_size);

pageframe_t kalloc_frame(void);

void kfree_frame(pageframe_t frame);

#endif
