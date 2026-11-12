/* NIDDB-only fixtures never issue the independent frontend bridge signature.
 * Reject that separate test scope rather than fabricate a successful bridge. */
#include "../../src/ntvdm-exe/softpc/include/mvdm_vm_bridge.h"

uint32_t c_getECX(void) { return 0; }
uint32_t c_getEDX(void) { return 0; }
uint32_t c_getESI(void) { return 0; }
int32_t c_getPE(void) { return 0; }
int32_t c_getVM(void) { return 0; }
int32_t c_getCPL(void) { return 0; }
void c_setEBX(uint32_t value) { (void)value; abort(); }
void c_setESI(uint32_t value) { (void)value; abort(); }
int mvdm_vm_bridge_bop(uint32_t signature, uint32_t operation,
    uint32_t low, uint32_t high, uint32_t vm, int ring0,
    mvdm_vm_bridge_result *result)
{
    (void)operation; (void)low; (void)high;
    (void)vm; (void)ring0; (void)result;
    if (signature == MVDM_VM_BRIDGE_SIGNATURE) abort();
    return 0;
}
