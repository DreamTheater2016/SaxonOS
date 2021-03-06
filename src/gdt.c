#include <gdt.h>

#define ENTRY_COUNT 3

struct gdt_entry gdt[ENTRY_COUNT];
struct gdt_ptr gp;

void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran)
{
	gdt[num].base_low = (base & 0xFFFF);
	gdt[num].base_middle = (base >> 16) & 0xFF;
	gdt[num].base_high = (base >> 24) & 0xFF;

	gdt[num].limit_low = (limit & 0xFFFF);
	gdt[num].granularity = ((limit >> 16) & 0x0F);

	gdt[num].granularity |= (gran & 0xF0);
	gdt[num].access = access;
}

void gdt_install()
{
	/* Setup the GDT pointer and limit */
	gp.limit = (sizeof(struct gdt_entry) * 6) - 1;
	gp.base = (uint32_t)&gdt;

	//NULL descriptor
	gdt_set_gate(0, 0, 0, 0, 0);

	//Kernel code segment
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

	//Kernel data segment
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

	//User mode code segment
	gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);

	//User mode data segment
	gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

	write_tss();

	gdt_flush();
	tss_flush();
}
