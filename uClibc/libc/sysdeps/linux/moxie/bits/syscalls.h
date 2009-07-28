#ifndef _BITS_SYSCALLS_H
#define _BITS_SYSCALLS_H
#ifndef _SYSCALL_H
# error "Never use <bits/syscalls.h> directly; include <sys/syscall.h> instead."
#endif

#define INTERNAL_SYSCALL_NCS(name, err, nr, args...)	\
({							\
  0;							\
})

#endif /* _BITS_SYSCALLS_H */

