#include "include/mvdm_vm_command_entry.h"

/* CCPU cpu4gen.h / call.h and host nt_eoi.h production ABI. */
extern int c_getVM(void);
extern unsigned short c_getIP(void);
extern void c_setIP(unsigned short);
extern void CALLF(unsigned long target[2]);
extern void host_ica_lock(void);
extern void host_ica_unlock(void);

static uint32_t command_callback;

DWORD mvdm_vm_command_install_callback(uint32_t callback)
{
    if (command_callback) return ERROR_ALREADY_EXISTS;
    command_callback = callback;
    return ERROR_SUCCESS;
}

void mvdm_vm_ingress_reset(void)
{
    host_ica_lock();
    command_callback = 0;
    host_ica_unlock();
}

int mvdm_vm_command_enter(uint32_t service)
{
    uint32_t callback;
    unsigned long target[2];
    if ((service != 8 && service != 10) || !c_getVM()) return 0;
    host_ica_lock();
    callback = command_callback;
    host_ica_unlock();
    if (!callback) return 0;
    /* CCPU owns the segment/stack/paging checks and real FAR return frame.
     * VMM establishes the calling VM before a product request exists. */
    c_setIP((uint16_t)(c_getIP() + 1));
    target[0] = (uint16_t)callback;
    target[1] = callback >> 16;
    CALLF(target);
    return 1;
}
