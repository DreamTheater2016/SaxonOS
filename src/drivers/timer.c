#include <drivers/timer.h>

void terminal_writestring(const char*);

uint32_t jiffies;

void timer_handler(struct regs *r)
{
	static uint32_t timer_ticks = 0;
	/*if (++timer_ticks % 18 == 0)
	{
	        terminal_writestring("A second passed");
	}*/
	jiffies++;
}

void timer_install()
{
	irq_install_handler(0, timer_handler);
}

void timer_set_interval(uint32_t hz)
{
	int divisor = 1193180 / hz;
	outb(0x43, 0x36);
	outb(0x40, divisor & 0xFF);
	outb(0x40, divisor >> 8);
}
