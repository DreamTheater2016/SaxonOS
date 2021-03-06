#include <userspace/syscall.h>

static void *syscalls[21] =
{
        &sys_read, &sys_write, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, &sys_time, 0, 0, 0, 0, 0, 0, 0,
};

DEFN_SYSCALL0(sys_read, 0)
DEFN_SYSCALL1(sys_write, 1, const char*)
DEFN_SYSCALL1(sys_time, 1, time_t *);

uint32_t syscall_count = 21;

void syscall_install(void)
{
	irq_install_handler(0x60, &syscall_handler);
}

void syscall_handler(struct regs *r)
{
	if (r->eax >= syscall_count)
		return;

	void *handler = syscalls[r->eax];

	if (handler == 0)
		return;

	int ret;
	asm volatile ("	\
     push %1; \
     push %2; \
     push %3; \
     push %4; \
     push %5; \
     call *%6; \
     pop %%ebx; \
     pop %%ebx; \
     pop %%ebx; \
     pop %%ebx; \
     pop %%ebx; \
   " : "=a" (ret) : "r" (r->edi), "r" (r->esi), "r" (r->edx), "r" (r->ecx), "r" (r->ebx), "r" (handler));
   r->eax = ret;

   //r->rax = (syscalls[r->rdi])(r->r9, r->8, r->ecx, r->edx, r->esi);
}
