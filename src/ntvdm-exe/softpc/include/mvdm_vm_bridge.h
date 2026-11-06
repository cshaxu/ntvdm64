#ifndef MVDM_VM_BRIDGE_H
#define MVDM_VM_BRIDGE_H
#include <stdint.h>

#define MVDM_VM_BRIDGE_SIGNATURE UINT32_C(0x4e544d56)
typedef struct {
    uint32_t status, value, serial_low, serial_high;
} mvdm_vm_bridge_result;

/* BOP B1 extension; unrelated original NIDDB operations are not handled. */
int mvdm_vm_bridge_bop(uint32_t signature, uint32_t operation,
    uint32_t serial_low, uint32_t serial_high, uint32_t vm, int protected_ring0,
    mvdm_vm_bridge_result *result);
#endif
