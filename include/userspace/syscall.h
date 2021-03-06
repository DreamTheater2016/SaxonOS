#ifndef __USERSPACE__SYSCALL_H
#define __USERSPACE__SYSCALL_H

#include <common.h>
#include <irq.h>

#define DECL_SYSCALL0(fn) int syscall_##fn();
#define DECL_SYSCALL1(fn,arg1) int syscall_##fn(arg1);
#define DECL_SYSCALL2(fn,arg1,arg2) int syscall_##fn(arg1,arg2);
#define DECL_SYSCALL3(fn,arg1,arg2,arg3) int syscall_##fn(arg1,arg2,arg3);
#define DECL_SYSCALL4(fn,arg1,arg2,arg3,arg4) int syscall_##fn(arg1,arg2,arg3,arg4);
#define DECL_SYSCALL5(fn,arg1,arg2,arg3,arg4,arg5) int syscall_##fn(arg1,arg2,arg3,arg4,arg5);

#define DEFN_SYSCALL0(fn, num) \
int syscall_##fn() \
{ \
 int a; \
 asm volatile("int $0x80" : "=a" (a) : "0" (num)); \
 return a; \
}

#define DEFN_SYSCALL1(fn, num, ARG1) \
int syscall_##fn(ARG1 arg1) \
{ \
 int a; \
 asm volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((int)arg1)); \
 return a; \
}

#define DEFN_SYSCALL2(fn, num, ARG1, ARG2) \
int syscall_##fn(ARG1 arg1, ARG2 arg2) \
{ \
 int a; \
 asm volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((int)arg1), "c" ((int)arg2)); \
 return a; \
}

#define DEFN_SYSCALL3(fn, num, ARG1, ARG2, ARG3)	\
	int syscall_##fn(ARG1 arg1, ARG2 arg2, ARG3 arg3)		\
{ \
 int a; \
 asm volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((int)arg1), "c" ((int)arg2), "d" ((int)arg3)); \
 return a; \
}

#define DEFN_SYSCALL4(fn, num, ARG1, ARG2, ARG3, ARG4)			\
	int syscall_##fn(ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4)		\
{ \
 int a; \
 asm volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((int)arg1), "c" ((int)arg2), "d" ((int)arg3), "e" ((int)arg4));	\
 return a; \
}

#define DEFN_SYSCALL5(fn, num, ARG1, ARG2, ARG3, ARG4, ARG5)			\
	int syscall_##fn(ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5)	\
{ \
 int a; \
 asm volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((int)arg1), "c" ((int)arg2), "d" ((int)arg3), "e" ((int)arg4), "f" ((int)arg5));	\
 return a; \
}

DECL_SYSCALL0(sys_read) 
DECL_SYSCALL1(sys_write, const char*)
DECL_SYSCALL1(sys_time, time_t *)

void syscall_install(void);

void syscall_handler(struct regs *r);

extern int sys_read();
extern int sys_write();
extern int sys_time();

#endif
