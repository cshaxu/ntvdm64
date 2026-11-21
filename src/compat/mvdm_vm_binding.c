#include "include/mvdm_vm_bridge.h"

/* The original CCPU consumes the existing BIOS vector and owns all original
 * operations. The worker assembles exactly the VxD extension slot. */
extern void (*BIOS[])(void);
extern void virtual_device_trap(void);
extern uint32_t c_getEAX(void), c_getECX(void), c_getEDX(void), c_getESI(void), c_getEBX(void);
extern int32_t c_getPE(void), c_getVM(void);
extern uint32_t c_getCPL(void);
extern void c_setEAX(uint32_t), c_setEBX(uint32_t), c_setEDX(uint32_t), c_setESI(uint32_t);
extern void c_setCF(int32_t);

enum { VM_DEVICE_BOP = 0xb1 };

static void vm_device_bop(void)
{
    mvdm_vm_bridge_result result;
    if (!mvdm_vm_bridge_bop(c_getEAX(), c_getECX(), c_getEDX(), c_getESI(), c_getEBX(),
            c_getPE() && !c_getVM() && c_getCPL() == 0, &result)) {
        virtual_device_trap();
        return;
    }
    c_setEAX(result.status);
    c_setEBX(result.value);
    c_setEDX(result.serial_low);
    c_setESI(result.serial_high);
    c_setCF(result.status != 0);
}

BOOL ntvdm_vm_services_initialize(void)
{
    if (BIOS[VM_DEVICE_BOP] == vm_device_bop) return TRUE;
    if (BIOS[VM_DEVICE_BOP] != virtual_device_trap) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    BIOS[VM_DEVICE_BOP] = vm_device_bop;
    return TRUE;
}
