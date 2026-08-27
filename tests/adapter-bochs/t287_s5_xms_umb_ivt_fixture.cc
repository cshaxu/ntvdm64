#include "adapter-bochs/machine_facade.h"

extern "C" {
#include "adapter-mvdm-host-out/softpc/include/mvdm_command_registers.h"

void xmsInitUMB(void);
void xmsRequestUMB(void);
void xmsReleaseUMB(void);
void xmsReleaseUMBNotify(void *, unsigned long);
void xmsSysPageSize(void);
void xmsQueryExtMem(void);
void xmsNotifyHookI15(void);
int ReserveUMB(unsigned short, void **, unsigned long *);
unsigned long xmsMemorySize = 640u;
}

static unsigned short observed_int15_segment;
static unsigned short observed_int15_offset;
static unsigned int observed_int15_calls;

/* This fixture deliberately has no physical UMB owner: it proves the exact
 * original B1/B2 unavailable paths and does not manufacture a usable span. */
extern "C" int ReserveUMB(unsigned short, void **address, unsigned long *size)
{
    if (address != 0) *address = 0;
    if (size != 0) *size = 0;
    return 0;
}

/* Test-only carrier for the external firmware interface.  Production
 * UpdateKbdInt15 remains owned by the separately admitted firmware package. */
extern "C" void UpdateKbdInt15(unsigned short segment, unsigned short offset)
{
    observed_int15_segment = segment;
    observed_int15_offset = offset;
    ++observed_int15_calls;
}

static int copy_ax(unsigned short expected)
{
    unsigned short value = 0u;
    return machine_facade_copy_ax16(&value) && value == expected;
}

static int copy_bx(unsigned short expected)
{
    unsigned short value = 0u;
    return machine_facade_copy_bx16(&value) && value == expected;
}

static int copy_cx(unsigned short expected)
{
    unsigned short value = 0u;
    return machine_facade_copy_cx16(&value) && value == expected;
}

int main()
{
    if (!machine_facade_machine_begin(0x200000u, 0x200000u)) return 1;

    /* Empty ReserveUMB retains xmsInitUMB source ordering. */
    if (!machine_facade_set_ax16(0x0200u) || !machine_facade_set_bx16(0x0010u))
        return 2;
    xmsInitUMB();
    if (!machine_facade_set_dx16(1u)) return 3;
    xmsRequestUMB();
    if (!copy_ax(0u) || !copy_bx(0x00b1u)) return 4;
    if (!machine_facade_set_dx16(0xe000u)) return 5;
    xmsReleaseUMB();
    if (!copy_ax(0u) || !copy_bx(0x00b2u)) return 6;
    xmsReleaseUMBNotify((void *)0, 0u);

    /* Directly composable original system and XMS-size source bodies. */
    xmsSysPageSize();
    if (!copy_ax(4096u)) return 7;
    xmsQueryExtMem();
    if (!copy_ax(640u)) return 8;

    /* Original xmsNotifyHookI15 passes CS:AX and then returns XMS size in CX.
     * This is source-order proof only, not a claim that keyboard optimization
     * has been enabled. */
    machine_facade_apply_real_mode_entry(0x1234u, 0u);
    if (!machine_facade_set_ax16(0x5678u)) return 9;
    xmsNotifyHookI15();
    if (observed_int15_calls != 1u || observed_int15_segment != 0x1234u ||
        observed_int15_offset != 0x5678u || !copy_cx(640u)) return 10;

    return machine_facade_machine_cleanup() ? 0 : 11;
}
