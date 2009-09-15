#include "hw/hw.h"
#include "hw/boards.h"

extern QEMUMachine moxiesim_machine;

void register_machines(void)
{
  qemu_register_machine(&moxiesim_machine);
}

void cpu_save(QEMUFile *f, void *opaque)
{

}

int cpu_load(QEMUFile *f, void *opaque, int version_id)
{

    return 0;
}
