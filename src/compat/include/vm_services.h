#ifndef COMPAT_VM_SERVICES_H
#define COMPAT_VM_SERVICES_H
#include <stdint.h>
#include <windows.h>

#define MVDM_VM_BRIDGE_SIGNATURE UINT32_C(0x4e544d56)
typedef struct {
    uint32_t status, value, serial_low, serial_high;
} mvdm_vm_bridge_result;

/* The product owns VM/IRQ and shutdown state. This process-lifetime table
 * only connects original callers to the installed product capability. */
typedef struct compat_vm_services {
    int (*bop)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, int,
        mvdm_vm_bridge_result *);
    void (*clear_instance_data)(void);
} compat_vm_services;
BOOL compat_vm_install(const compat_vm_services *services);
const compat_vm_services *compat_vm_current_services(void);
VOID ClearInstanceDataMarking(VOID);
#endif
