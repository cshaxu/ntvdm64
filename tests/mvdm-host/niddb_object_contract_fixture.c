/* Exercise the original virtual.obj through its public host entry points.
 * CPU registers and the BIOS virtualising byte are controlled boundaries.
 * No freed allocation is dereferenced. This is not a guest runtime test.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef MVDM_NIDDB_EXPECT_POINTER_BYTES
#define MVDM_NIDDB_EXPECT_POINTER_BYTES 4
#endif
typedef char niddb_pointer_abi_check[
    sizeof(void *) == MVDM_NIDDB_EXPECT_POINTER_BYTES ? 1 : -1];

extern void **NIDDB_Allocate_Instance_Data(int, void (*)(void **), void (*)(void));
extern void NIDDB_Deallocate_Instance_Data(void **);
extern void virtual_device_trap(void);
extern void virtual_swap_instance(void);
extern int NIDDB_is_active(void);

static uint32_t eax_value, ebx_value;
static uint8_t current_virtual_byte;
static int errors;
static void **registered_instance;
static uintptr_t expected_termination;
static unsigned termination_count;

static void termination_callback(void)
{
    if ((uintptr_t)*registered_instance != expected_termination) ++errors;
    ++termination_count;
}

uint32_t c_getEAX(void) { return eax_value; }
uint32_t c_getEBX(void) { return ebx_value; }
uint16_t c_getBX(void) { return (uint16_t)ebx_value; }
uint16_t c_getCX(void) { return 0; }
uint16_t c_getDX(void) { return 0; }
void c_setEAX(uint32_t value) { eax_value = value; }
void c_setEDX(uint32_t value) { (void)value; }
void c_setCF(uint32_t value) { if (value) ++errors; }
uint8_t c_sas_hw_at(uint32_t address) { (void)address; return current_virtual_byte; }
int c_SasRegisterVirtualSelectors(uint16_t a, uint16_t b) { (void)a; (void)b; return 1; }
void mvdm_softpc_report_virtual_device_control(uint32_t a, uint16_t b, uint16_t c, uint16_t d)
{ (void)a; (void)b; (void)c; (void)d; }
void host_error(int code, int action, const char *text, ...)
{ (void)code; (void)action; (void)text; ++errors; }
void ClearInstanceDataMarking(void) { }

static uint32_t control(uint32_t command, uint32_t handle)
{
    eax_value = command;
    ebx_value = handle;
    virtual_device_trap();
    return eax_value;
}

static uintptr_t reuse_instances[3];
static unsigned reuse_seen;

static void reuse_termination_callback(void)
{
    uintptr_t actual = (uintptr_t)*registered_instance;
    unsigned i;
    for (i = 0; i != 3 && reuse_instances[i] != actual; ++i) {}
    if (i == 3 || (reuse_seen & (1u << i))) ++errors;
    else reuse_seen |= 1u << i;
}

static int verify_three_vm_reuse(void)
{
    void **instance = NIDDB_Allocate_Instance_Data(16, NULL,
        reuse_termination_callback);
    uintptr_t snapshot, active, replacement;
    uint32_t ids[3], replacement_id;
    if (!instance || !*instance) return 2;
    registered_instance = instance;
    snapshot = (uintptr_t)*instance;
    ids[0] = control(3, 100); reuse_instances[0] = (uintptr_t)*instance;
    ids[1] = control(9, 200); reuse_instances[1] = (uintptr_t)*instance;
    ids[2] = control(9, 300); reuse_instances[2] = (uintptr_t)*instance;
    if (errors || ids[0] == ids[1] || ids[0] == ids[2] || ids[1] == ids[2]) return 2;
    current_virtual_byte = (uint8_t)ids[1];
    virtual_swap_instance();
    active = (uintptr_t)*instance;
    if (active != reuse_instances[1]) return 2;
    reuse_seen = 0;
    control(11, 100);
    if (errors || reuse_seen != 1u) return 2;
    /* Compare identity only; the retired allocation must not be read. */
    if ((uintptr_t)*instance != active) {
        puts("FAIL: retiring an inactive VM did not preserve the active instance");
        return 1;
    }
    replacement_id = control(9, 400);
    replacement = (uintptr_t)*instance;
    if (errors || replacement_id != ids[0] || !replacement || replacement == active) return 2;
    current_virtual_byte = (uint8_t)ids[1];
    virtual_swap_instance();
    if ((uintptr_t)*instance != active) return 2;
    current_virtual_byte = (uint8_t)replacement_id;
    virtual_swap_instance();
    if ((uintptr_t)*instance != replacement) return 2;
    reuse_instances[0] = replacement;
    reuse_seen = 0;
    control(5, 100);
    if (errors || reuse_seen != 7u || (uintptr_t)*instance != snapshot ||
            NIDDB_is_active()) return 2;
    NIDDB_Deallocate_Instance_Data(instance);
    puts("NIDDB_THREE_VM_REUSE active=1 reused=1 callbacks=3 cleanup=1");
    return 0;
}

int main(void)
{
    void **instance = NIDDB_Allocate_Instance_Data(16, NULL, termination_callback);
    uint32_t system_id, child_id;
    uintptr_t snapshot_address, system_address, child_address;
    if (!instance || !*instance) return 2;
    registered_instance = instance;
    snapshot_address = (uintptr_t)*instance;
    ((unsigned char *)*instance)[0] = 0x11;
    system_id = control(3, 100);
    system_address = (uintptr_t)*instance;
    if (((unsigned char *)*instance)[0] != 0x11) return 2;
    ((unsigned char *)*instance)[0] = 0x22;
    child_id = control(9, 200);
    child_address = (uintptr_t)*instance;
    if (errors || system_id == child_id || system_address == child_address) return 2;
    if (((unsigned char *)*instance)[0] != 0x11) return 2;
    ((unsigned char *)*instance)[0] = 0x33;
    current_virtual_byte = (uint8_t)system_id;
    virtual_swap_instance();
    if ((uintptr_t)*instance != system_address || ((unsigned char *)*instance)[0] != 0x22) return 2;
    expected_termination = child_address;
    control(11, 200);
    if (errors || termination_count != 1) return 2;
    if ((uintptr_t)*instance != system_address) {
        printf("FAIL: retiring inactive child changed master pointer to retired instance\n");
        control(5, 100);
        NIDDB_Deallocate_Instance_Data(instance);
        return 1;
    }
    child_id = control(9, 201);
    child_address = (uintptr_t)*instance;
    if (child_address == system_address || errors) return 2;
    if (((unsigned char *)*instance)[0] != 0x11) return 2;
    expected_termination = child_address;
    control(11, 201);
    if (errors || termination_count != 2) return 2;
    if ((uintptr_t)*instance != system_address || ((unsigned char *)*instance)[0] != 0x22) {
        puts("FAIL: retiring active child did not select surviving system instance");
        return 1;
    }
    expected_termination = system_address;
    control(11, 100);
    if (errors || termination_count != 3) return 2;
    if ((uintptr_t)*instance != snapshot_address || ((unsigned char *)*instance)[0] != 0x11) {
        puts("FAIL: retiring last instance did not restore snapshot pointers");
        return 1;
    }
    if (!NIDDB_is_active()) return 1;
    control(5, 100);
    if (NIDDB_is_active() || (uintptr_t)*instance != snapshot_address) return 1;
    NIDDB_Deallocate_Instance_Data(instance);
    printf("NIDDB_OBJECT_CONTRACT checks=15 failures=0 callbacks=%u\n", termination_count);
    if (errors) return 2;
    return verify_three_vm_reuse();
}
