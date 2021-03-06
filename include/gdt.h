#ifndef GDT_H
#define GDT_H

#include <common.h>
#include <tss.h>

#define KERNEL_CODE_SEG 0x08
#define KERNEL_DATA_SEG 0x10
#define USER_CODE_SEG 0x1B
#define USER_DATA_SEG 0x23

struct gdt_entry
{
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access;
    unsigned char granularity;
    unsigned char base_high;
} __attribute__((packed));

struct gdt_ptr
{
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));

void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);
void gdt_install();

extern void gdt_flush();
extern void tss_flush();

#endif
