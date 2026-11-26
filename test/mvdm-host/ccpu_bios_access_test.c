/* Actual CCPU/SAS BIOS access paths with synthetic page tables.
 * No replacement paging, TLB, physical memory or BIOS implementation. */
#include <stdio.h>
#include <stdint.h>
#include <windows.h>
#include "non-mvdm/public/sdk/inc/nt_vdd.h"
#include "ntvdm-exe/session/session.h"
extern void sas_init(uint32_t), sas_term(void), c_cpu_init(void);
extern void c_setPE(int), c_setPG(int), c_setVM(int), c_setCPL(uint32_t), c_setCR3(uint32_t);
extern void c_setGDT_BASE_LIMIT(uint32_t, unsigned short);
extern unsigned char *Sim32pGetVDMPointer(uint32_t, unsigned char);
extern void phy_w32(uint32_t, uint32_t);
extern uint32_t phy_r32(uint32_t), c_sas_dw_at(uint32_t);
extern unsigned short c_sas_w_at(uint32_t);
extern unsigned char c_sas_hw_at(uint32_t);
extern void c_sas_store(uint32_t, unsigned char), c_sas_storew(uint32_t, unsigned short);
extern void c_sas_storedw(uint32_t, uint32_t);
extern int mvdm_softpc_guest_memory_begin(session *);
extern void mvdm_softpc_guest_memory_end(session *);
static unsigned cases, failures;
static void check(int ok, const char *name, unsigned width, unsigned cpl, unsigned flags)
{
    ++cases;
    if (!ok) {
        ++failures;
        fprintf(stderr, "FAIL %s width=%u CPL=%u PTE=%x\n", name, width, cpl, flags);
    }
}
static void prepare(unsigned cpl, unsigned flags)
{
    c_cpu_init();
    c_setPG(0); c_setPE(1); c_setVM(0);
    /* 386 combines U/S with OR, so let the PTE select user access. */
    phy_w32(0x1000, 0x2003);
    phy_w32(0x2010, 0x30000 | flags);
    phy_w32(0x30000, 0xa1b2c3d4);
    c_setCR3(0x1000); c_setCPL(cpl); c_setPG(1);
}
int main(void)
{
    session owner;
    unsigned width, cpl, flags;
    session_initialize(&owner, 1);
    if (!session_activate(&owner) || !session_thread_bind(&owner)) return 2;
    sas_init(0x200000);
    if (!mvdm_softpc_guest_memory_begin(&owner)) return 3;
    for (width = 1; width <= 4; width *= 2)
        for (cpl = 0; cpl <= 3; cpl += 3)
            for (flags = 1; flags <= 7; flags += 2) {
                uint32_t mask = width == 1 ? 0xff : width == 2 ? 0xffff : 0xffffffff;
                uint32_t actual;
                int allowed = !cpl || (flags & 6) == 6;
                prepare(cpl, flags);
                {
                    unsigned char *mapped = VdmMapFlat(0x400, 0, VDM_V86);
                    check(mapped && *mapped == 0xd4, "host-map-supervisor-read",
                        width, cpl, flags);
                }
                /* Read only permissions that are valid. The original unregistered
                 * PM fallback has no defined read value; do not bless that path. */
                if (!cpl || (flags & 4)) {
                    actual = width == 1 ? c_sas_hw_at(0x4000) :
                        width == 2 ? c_sas_w_at(0x4000) : c_sas_dw_at(0x4000);
                    check(actual == (0xa1b2c3d4 & mask), "read", width, cpl, flags);
                }
                prepare(cpl, flags);
                {
                    guest_memory_lease *lease = NULL;
                    uint8_t *bytes = NULL;
                    int ok = session_guest_memory_acquire(&owner, 0x4000, width,
                        GUEST_MEMORY_ACCESS_READ, &lease, &bytes);
                    check(ok == (!cpl || (flags & 4)), "lease-read", width, cpl, flags);
                    if (ok) (void)session_guest_memory_release(&owner, lease, 0);
                    lease = NULL; bytes = NULL;
                    ok = session_guest_memory_acquire(&owner, 0x4000, width,
                        GUEST_MEMORY_ACCESS_WRITE, &lease, &bytes);
                    /* Acquisition copies the readable bytes; write permission is
                     * checked on commit, not on acquiring the bounce buffer. */
                    if (ok) ok = session_guest_memory_release(&owner, lease, 1);
                    check(ok == allowed, "lease-write", width, cpl, flags);
                }
                prepare(cpl, flags);
                if (width == 1) c_sas_store(0x4000, 0x78);
                else if (width == 2) c_sas_storew(0x4000, 0x5678);
                else c_sas_storedw(0x4000, 0x12345678);
                actual = phy_r32(0x30000);
                check(actual == (allowed ? ((0xa1b2c3d4 & ~mask) | (0x12345678 & mask))
                    : 0xa1b2c3d4), "write", width, cpl, flags);
                c_setPG(0);
            }
    prepare(3, 0);
    check(VdmMapFlat(0x400, 0, VDM_V86) == NULL,
        "unmapped-page-is-not-physical-zero", 0, 3, 0);
    c_setPG(0);
    check(VdmMapFlat(0, 0x200000, VDM_V86) == NULL, "outside-memory", 0, 0, 0);
    check(VdmMapFlat(0xffff, 0xfffffff0, VDM_V86) == NULL, "wrapped-address", 0, 0, 0);
    check(VdmMapFlat(0, 0, (VDM_MODE)99) == NULL, "invalid-mode", 0, 0, 0);
    c_setGDT_BASE_LIMIT(0x18000, 15);
    phy_w32(0x18008, 0x0000ffff);
    phy_w32(0x1800c, 0x00009203); /* 16-bit data, base 0x30000. */
    phy_w32(0x30004, 0x44332211);
    {
        unsigned char *flat = VdmMapFlat(8, 4, VDM_PM);
        unsigned char *short_form = Sim32pGetVDMPointer(0x00080004, TRUE);
        check(flat && flat == short_form && *flat == 0x11,
            "original-short-and-extended-conversion", 1, 0, 0);
        check(VdmMapFlat(0x10, 0, VDM_PM) == NULL, "selector-outside-gdt", 0, 0, 0);
    }
    mvdm_softpc_guest_memory_end(&owner);
    sas_term();
    check(VdmMapFlat(0, 0, VDM_V86) == NULL, "released-map", 0, 0, 0);
    sas_init(0x200000);
    {
        unsigned char *base = VdmMapFlat(0, 0, VDM_V86);
        phy_w32(0x30000, 0x12345678);
        check(base && base[0x30000] == 0x78, "reinitialized-map", 1, 0, 0);
        check(VdmMapFlat(0, 0, VDM_PM) == NULL, "null-selector", 0, 0, 0);
    }
    sas_term();
    (void)session_thread_unbind(&owner);
    (void)session_dispose(&owner);
    fprintf(stderr, "CCPU_BIOS_ACCESS cases=%u failures=%u\n", cases, failures);
    return failures ? 1 : 0;
}
