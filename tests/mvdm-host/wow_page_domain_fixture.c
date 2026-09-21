#include <stdint.h>
#include <stdio.h>
#include <windows.h>

#include "ntvdm-exe/session/session.h"
#include "ntvdm-exe/softpc/include/mvdm_cvidc_vector_binding.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_guest_memory.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_physical_mapping.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"

extern void sas_init(uint32_t size);
extern void sas_term(void);
extern void c_sas_enable_20_bit_wrapping(void);
extern void c_sas_disable_20_bit_wrapping(void);
extern uint32_t c_sas_memory_size(void);
extern uint8_t *c_GetPhyAdd(uint32_t address);
extern void c_cpu_init(void);
extern void load_sw_cpu_access_functions(void);
extern uint32_t c_getCR0(void);
extern uint32_t c_getCR3(void);
extern void c_setCR3(uint32_t value);
extern void c_setCR0(uint32_t value);
extern uint16_t c_getMSW(void);
extern void c_setMSW(uint16_t value);
extern void c_setCPL(uint32_t value);
extern int32_t c_getCPL(void);
extern int xtrn2phy(uint32_t linear, uint32_t access, uint32_t *physical);
extern uint32_t c_sas_dw_at(uint32_t address);
extern void c_sas_fills(uint32_t address, uint8_t value, uint32_t byte_count);
extern void phy_w32(uint32_t address, uint32_t value);
extern uint32_t c_getGDT_BASE(void);
extern uint16_t c_getGDT_LIMIT(void);
extern void c_setGDT_BASE_LIMIT(uint32_t base, uint16_t limit);
extern LONG VdmAllocateVirtualMemory(ULONG *address, ULONG size, BOOL commit);
extern LONG VdmFreeVirtualMemory(ULONG address);

/* CCPU's access request bits are not x86 page-table bits. */
#define PG_R 0x00u
#define PG_W 0x01u
#define PG_U 0x02u
#define CR0_PE 0x00000001u
#define CR0_PG 0x80000000u
#define SHARED_CLOCK_LINEAR 0x7ffe0000u

int main(void)
{
    session owner;
    uint32_t first_physical = 0u;
    uint32_t second_physical = 0u;
    uint32_t first_tick;
    uint32_t original_gdt = 0u;
    uint16_t original_gdt_limit = 0u;
    ULONG dosx_gdt = 0u;
    uint32_t shared = 0u;
    uint32_t wow_gdt;
    uint32_t low, high, teb;
    int sas_started = 0;
    int guest_memory_started = 0;
    int physical_mapping_started = 0;
    int passed = 0;
    char const *stage = "initial";

    /* A worker domain is never a process-global fallback: without a bound
     * session or protected mode, admission must leave no published view. */
    stage = "unbound-rejection";
    if (mvdm_softpc_wow_page_domain_enter() ||
        mvdm_softpc_wow_page_domain_active() ||
        mvdm_softpc_wow_page_domain_guest_shared_info() != 0u) goto done;
    stage = "session";
    session_initialize(&owner, 41u);
    if (!session_activate(&owner) || !session_thread_bind(&owner)) goto done;
    sas_init(0x00200000u);
    sas_started = 1;
    c_cpu_init();
    load_sw_cpu_access_functions();
    stage = "guest-memory";
    if (!mvdm_softpc_guest_memory_begin(&owner)) goto done;
    guest_memory_started = 1;
    mvdm_cvidc_bind_vectors();
    /* This narrow fixture does not run the original nt_mem bootstrap. Reset
     * any inherited fixture binding, then establish the same source-shaped
     * physical record that nt_mem owns in the product before VdmAdd is used. */
    mvdm_softpc_physical_mapping_release();
    stage = "physical-mapping";
    if (!mvdm_softpc_physical_mapping_initialize(c_GetPhyAdd(0u),
            c_sas_memory_size())) goto done;
    physical_mapping_started = 1;
    stage = "real-mode-rejection";
    if (mvdm_softpc_wow_page_domain_enter() ||
        mvdm_softpc_wow_page_domain_active()) goto done;
    original_gdt = c_getGDT_BASE();
    original_gdt_limit = c_getGDT_LIMIT();
    stage = "dosx-gdt";
    if (VdmAllocateVirtualMemory(&dosx_gdt, 4096u, TRUE) < 0) goto done;
    c_sas_fills(dosx_gdt, 0u, 4096u);
    /* Deliberately retain DOSX's conflicting selectors. The product must
     * copy, not overwrite them, before it activates the WOW descriptor view. */
    phy_w32(dosx_gdt + 0x20u, 0x11112222u);
    phy_w32(dosx_gdt + 0x24u, 0x33334444u);
    phy_w32(dosx_gdt + 0x38u, 0x55556666u);
    phy_w32(dosx_gdt + 0x3cu, 0x77778888u);
    phy_w32(dosx_gdt + 0x18u, 0x0000ffffu);
    phy_w32(dosx_gdt + 0x1cu, 0x0000fa00u);
    if (c_sas_dw_at(dosx_gdt + 0x20u) != 0x11112222u ||
        c_sas_dw_at(dosx_gdt + 0x38u) != 0x55556666u) {
        fprintf(stderr, "WOW_PAGE_DOMAIN_DOSX_SETUP gdt=%08lx first=%08lx second=%08lx\n",
            (unsigned long)dosx_gdt,
            (unsigned long)c_sas_dw_at(dosx_gdt + 0x20u),
            (unsigned long)c_sas_dw_at(dosx_gdt + 0x38u));
        goto done;
    }
    c_setGDT_BASE_LIMIT(dosx_gdt, 0xfffu);
    c_setMSW((uint16_t)(c_getMSW() | CR0_PE));
    c_setCR0(CR0_PE);
    c_setCPL(3u);
    /* The page domain must reject the real-mode A20 view rather than silently
     * wrap its high clock page. DOSX owns the subsequent A20 transition. */
    c_sas_enable_20_bit_wrapping();
    stage = "a20-rejection";
    if (mvdm_softpc_wow_page_domain_enter() ||
        mvdm_softpc_wow_page_domain_active()) goto done;
    c_sas_disable_20_bit_wrapping();
    stage = "domain-admission";
    {
        int entered = mvdm_softpc_wow_page_domain_enter();
        int readable = xtrn2phy(SHARED_CLOCK_LINEAR, PG_U | PG_R,
            &first_physical);
        int writable = xtrn2phy(SHARED_CLOCK_LINEAR, PG_U | PG_W,
            &second_physical);

        if (!entered ||
            (c_getCR0() & (CR0_PE | CR0_PG)) != (CR0_PE | CR0_PG) ||
            c_getCR3() == 0u || !readable || writable) {
            fprintf(stderr, "WOW_PAGE_DOMAIN_ADMISSION entered=%d cr0=%08lx cr3=%08lx read=%d write=%d read_phy=%08lx write_phy=%08lx\n",
                entered, (unsigned long)c_getCR0(),
                (unsigned long)c_getCR3(), readable, writable,
                (unsigned long)first_physical, (unsigned long)second_physical);
            goto done;
        }
    }
    stage = "client-view";
    shared = (uint32_t)mvdm_softpc_wow_page_domain_guest_shared_info();
    if (shared == 0u || shared >= c_sas_memory_size() ||
        c_sas_dw_at(shared) != shared + 4096u ||
        c_sas_dw_at(shared + 4u) != shared + 8192u ||
        c_sas_dw_at(shared + 4096u + 4u) != 0u ||
        c_getGDT_BASE() != dosx_gdt) {
        fprintf(stderr, "WOW_PAGE_DOMAIN_CLIENT_VIEW shared=%08lx sas=%08lx dosx=%08lx psi=%08lx ahe=%08lx first=%08lx second=%08lx\n",
            (unsigned long)shared, (unsigned long)c_sas_memory_size(),
            (unsigned long)dosx_gdt,
            (unsigned long)c_sas_dw_at(shared),
            (unsigned long)c_sas_dw_at(shared + 4u),
            0ul, 0ul);
        goto done;
    }
    stage = "wow-gdt";
    if (!mvdm_softpc_wow_page_domain_activate_wow_context() ||
        (wow_gdt = c_getGDT_BASE()) == dosx_gdt) goto done;
    low = c_sas_dw_at(wow_gdt + 0x20u);
    high = c_sas_dw_at(wow_gdt + 0x24u);
    if (low != 0x0000ffffu || high != 0x00cff200u) goto done;
    low = c_sas_dw_at(wow_gdt + 0x38u);
    high = c_sas_dw_at(wow_gdt + 0x3cu);
    teb = (low >> 16) | ((high & 0xffu) << 16) | (high & 0xff000000u);
    if (teb != shared + 3u * 4096u || (low & 0xffffu) != 0x0fffu ||
        (high & 0x00ffff00u) != 0x0000f200u ||
        c_sas_dw_at(teb + 0x18u) != teb ||
        c_sas_dw_at(teb + 0x44u) != 0u || c_sas_dw_at(teb + 0x50u) != 0u ||
        c_sas_dw_at(teb + 0x5cu) != 0u || c_sas_dw_at(teb + 0x60u) != 0u ||
        c_sas_dw_at(teb + 0x6cu) != 0u || c_sas_dw_at(teb + 0x70u) != 0u) goto done;
    stage = "clock-instruction";
    first_tick = c_sas_dw_at(SHARED_CLOCK_LINEAR);
    Sleep(100u);
    stage = "clock-update";
    if (!xtrn2phy(SHARED_CLOCK_LINEAR, PG_U | PG_R, &second_physical) ||
        second_physical != first_physical ||
        c_sas_dw_at(SHARED_CLOCK_LINEAR) == first_tick) {
        goto done;
    }
    stage = "withdrawal";
    mvdm_softpc_wow_page_domain_leave_protected();
    if ((c_getCR0() & CR0_PG) != 0u || c_getGDT_BASE() != dosx_gdt) goto done;
    stage = "reentry";
    if (!mvdm_softpc_wow_page_domain_reenter_protected() ||
        (c_getCR0() & (CR0_PE | CR0_PG)) != (CR0_PE | CR0_PG) ||
        !xtrn2phy(SHARED_CLOCK_LINEAR, PG_U | PG_R, &second_physical) ||
        !mvdm_softpc_wow_page_domain_activate_wow_context() ||
        c_getGDT_BASE() == dosx_gdt) goto done;
    passed = 1;
done:
    mvdm_softpc_wow_page_domain_leave_protected();
    if (original_gdt != 0u) c_setGDT_BASE_LIMIT(original_gdt, original_gdt_limit);
    if (guest_memory_started) mvdm_softpc_guest_memory_end(&owner);
    if (session_thread_current() == &owner) (void)session_thread_unbind(&owner);
    if (session_valid(&owner) && !session_dispose(&owner)) passed = 0;
    if (mvdm_softpc_wow_page_domain_active() ||
        mvdm_softpc_wow_page_domain_guest_shared_info() != 0u) passed = 0;
    if (dosx_gdt != 0u && VdmFreeVirtualMemory(dosx_gdt) < 0) passed = 0;
    if (physical_mapping_started) mvdm_softpc_physical_mapping_release();
    if (sas_started) sas_term();
    if (!passed) {
        fprintf(stderr, "WOW_PAGE_DOMAIN_LIFECYCLE_FAILED stage=%s\n", stage);
        return 1;
    }
    puts("WOW_PAGE_DOMAIN_LIFECYCLE_OK");
    return 0;
}
