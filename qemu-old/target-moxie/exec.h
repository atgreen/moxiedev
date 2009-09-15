#ifndef EXEC_MOXIE_H
#define EXEC_MOXIE_H 1
#include "config.h"
#include "dyngen-exec.h"

register struct CPUMOXIEState *env asm(AREG0);

#include "cpu.h"
#include "exec-all.h"

#if !defined(CONFIG_USER_ONLY)
#include "softmmu_exec.h"
#endif /* !defined(CONFIG_USER_ONLY) */

static inline void env_to_regs(void)
{
}

static inline void regs_to_env(void)
{
}

static inline int cpu_halted(CPUState *env) {
    if (!env->halted)
        return 0;
    return EXCP_HALTED;
}

static inline int cpu_has_work(CPUState *env)
{
  return (env->interrupt_request & CPU_INTERRUPT_HARD);
}
#endif
