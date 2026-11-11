/* Test-only SAS/CPU boundary: no guest, USER or NT kernel emulation. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "ntvdm-exe/softpc/include/mvdm_softpc_fast_bop.h"

unsigned long IntelBase;
unsigned long Cpu40LdtShadowAddress;
static uint32_t ldt[0x1fff * 2];
static uint32_t state, cr0;
static unsigned writes;

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "FAIL line %d\n", __LINE__); exit(1); } } while (0)
uint32_t c_getCR0(void) { return cr0; }
uint32_t c_sas_dw_at(uint32_t address) { CHECK(address == 0x714u); return state; }
void c_sas_storedw(uint32_t address, uint32_t value)
{ CHECK(address == 0x714u); state = value; ++writes; }
long DpmiSetWowLdtEntry(unsigned long selector, unsigned long low, unsigned long high)
{ ldt[(selector >> 3) * 2] = low; ldt[(selector >> 3) * 2 + 1] = high; return 0; }
void mvdm_softpc_report_cpu40_descriptor_publish(unsigned short selector,
    unsigned long a, unsigned long b, unsigned long c, unsigned long d,
    unsigned long e, unsigned long f) { }
void mvdm_softpc_report_wow_fast_callback_binding(const char *stage, unsigned long address) { }

int main(void)
{
    uint16_t offset, selector;
    state = 0x02460200u;
    mvdm_softpc_fast_bop_sync_mode();
    CHECK(writes == 0); /* Disabled profile cannot change ordinary DOS. */
    CHECK(!mvdm_softpc_fast_bop_prepare(NULL, &selector));
    CHECK(!mvdm_softpc_fast_bop_prepare(&offset, &selector));
    Cpu40LdtShadowAddress = 0x1000u;
    IntelBase = (unsigned long)(uintptr_t)ldt - Cpu40LdtShadowAddress;
    CHECK(mvdm_softpc_fast_bop_prepare(&offset, &selector));
    CHECK(state == 0x02461200u); /* Real mode: set only RM_BIT. */
    CHECK(mvdm_softpc_fast_bop_redirect(selector, offset) == MVDM_FAST_BOP_INLINE);
    CHECK(mvdm_softpc_fast_bop_redirect(selector, mvdm_softpc_fast_bop_dispatch_offset()) == MVDM_FAST_BOP_WOW);
    CHECK(mvdm_softpc_fast_bop_redirect(selector, mvdm_softpc_fast_bop_callback_offset()) == MVDM_FAST_BOP_RETURN);
    CHECK(!mvdm_softpc_fast_bop_redirect(selector, 0x40));
    CHECK(!mvdm_softpc_fast_bop_redirect(selector - 8, offset));
    cr0 = 0x80000011u;
    mvdm_softpc_fast_bop_sync_mode();
    CHECK(state == 0x02460200u); /* Protected mode: clear only RM_BIT. */
    cr0 = 0x10u;
    mvdm_softpc_fast_bop_sync_mode();
    CHECK(state == 0x02461200u); /* Return to real mode restores the gate. */
    puts("FAST_BOP_MODE_BOUNDARY_OK (mock SAS/CPU; not guest acceptance)");
    return 0;
}
