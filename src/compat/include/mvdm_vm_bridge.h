#ifndef MVDM_VM_BRIDGE_H
#define MVDM_VM_BRIDGE_H
#include <stdint.h>
#include <windows.h>

#define MVDM_VM_BRIDGE_SIGNATURE UINT32_C(0x4e544d56)
typedef struct {
    uint32_t status, value, serial_low, serial_high;
} mvdm_vm_bridge_result;

enum { MVDM_VM_DEVICE_BOP = 0xb1 };
/* Startup-only borrowed slot; B retains no pointer into the original vector.
 * Caller selects the product extension slot before guest execution. */
BOOL ntvdm_vm_services_initialize(void (**slot)(void));

/* BOP B1 extension; unrelated original NIDDB operations are not handled. */
int mvdm_vm_bridge_bop(uint32_t signature, uint32_t operation,
    uint32_t serial_low, uint32_t serial_high, uint32_t vm, int protected_ring0,
    mvdm_vm_bridge_result *result);
#endif
