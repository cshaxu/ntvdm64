#ifndef MVDM_VM_BRIDGE_H
#define MVDM_VM_BRIDGE_H
#include "compat/include/vm_services.h"

/* BOP B1 extension; unrelated original NIDDB operations are not handled. */
int mvdm_vm_bridge_bop(uint32_t signature, uint32_t operation,
    uint32_t serial_low, uint32_t serial_high, uint32_t vm, int protected_ring0,
    mvdm_vm_bridge_result *result);
#endif
