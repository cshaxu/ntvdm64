#include "mvdm_softpc_wow_page_domain.h"

#include <windows.h>

#include "ntvdm-exe/session/session.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_termination.h"

/* Keep this non-mirror binding on the narrow, fixed-width CCPU/nt_mem ABI it
 * consumes.  Pulling the historical macro headers into a new adapter would
 * make its build depend on source-era include ordering. */
extern uint32_t c_sas_memory_size(void);
extern int c_sas_twenty_bit_wrapping_enabled(void);
extern void c_sas_fills(uint32_t address, uint8_t value, uint32_t byte_count);
extern void c_sas_storedw(uint32_t address, uint32_t value);
extern void c_sas_loads(uint32_t source, uint8_t *destination,
    uint32_t byte_count);
extern void c_sas_stores(uint32_t destination, uint8_t *source,
    uint32_t byte_count);
extern uint32_t c_getCR0(void);
extern uint32_t c_getCR3(void);
extern void c_setCR0(uint32_t value);
extern void c_setCR3(uint32_t value);
extern uint32_t c_getGDT_BASE(void);
extern uint16_t c_getGDT_LIMIT(void);
extern void c_setGDT_BASE_LIMIT(uint32_t base, uint16_t limit);
extern LONG VdmAllocateVirtualMemory(ULONG *address, ULONG size, BOOL commit);
extern LONG VdmFreeVirtualMemory(ULONG address);
extern LONG VdmAddVirtualMemory(ULONG host_address, ULONG size,
    ULONG *intel_address);
extern LONG VdmRemoveVirtualMemory(ULONG intel_address);

#define MVDM_SOFTPC_PAGE_BYTES 4096u
#define MVDM_SOFTPC_PAGE_PRESENT_USER 7u
#define MVDM_SOFTPC_PAGE_PRESENT_USER_READ 5u
#define MVDM_SOFTPC_CR0_PG 0x80000000u
#define MVDM_SOFTPC_SHARED_CLOCK_LINEAR 0x7ffe0000u
#define MVDM_SOFTPC_GDT_ENTRY_BYTES 8u
#define MVDM_SOFTPC_KGDT_R3_DATA 0x20u
#define MVDM_SOFTPC_KGDT_R3_TEB 0x38u
#define MVDM_SOFTPC_TEB_BYTES 4096u
/* These are offsets read from the pinned immutable USER.EXE profile, not
 * inferred from a modern host TEB. */
#define MVDM_SOFTPC_TEB_SELF 0x18u
#define MVDM_SOFTPC_CLIENT_VIEW_BYTES (5u * MVDM_SOFTPC_PAGE_BYTES)

typedef struct mvdm_softpc_wow_page_domain {
    session *owner;
    ULONG table_allocation;
    ULONG table_bytes;
    ULONG clock_intel_address;
    volatile ULONG *clock_page;
    HANDLE clock_stop;
    HANDLE clock_thread;
    ULONG directory;
    ULONG client_allocation;
    ULONG guest_shared_info;
    ULONG guest_server_info;
    ULONG guest_handle_table;
    ULONG guest_teb;
    ULONG wow_gdt;
    ULONG dosx_gdt;
    USHORT dosx_gdt_limit;
    int wow_context_active;
    int active;
} mvdm_softpc_wow_page_domain;

static mvdm_softpc_wow_page_domain domain;

/* Default-off lifecycle evidence for the finite S41 binding.  The existing
 * scalar observer performs no I/O unless its report-path environment variable
 * is explicitly set; no guest address, descriptor, page or session state is
 * read or changed by this call. */
static void page_domain_report(char const *stage)
{
    mvdm_softpc_report_dpmi_mode_state(stage, (unsigned long)c_getCR0(),
        (unsigned long)c_getCR3(), 0u, 0u, 0u, 0u);
}

static DWORD WINAPI clock_publisher(void *context)
{
    mvdm_softpc_wow_page_domain *value = context;

    for (;;) {
        if (WaitForSingleObject(value->clock_stop, 10u) != WAIT_TIMEOUT)
            return 0;
        value->clock_page[0] = GetTickCount();
        value->clock_page[1] = 0x01000000u;
    }
}

static void page_domain_dispose(void *context)
{
    mvdm_softpc_wow_page_domain *value = context;

    mvdm_softpc_wow_page_domain_leave_protected();
    if (value->clock_stop != NULL) SetEvent(value->clock_stop);
    if (value->clock_thread != NULL) {
        WaitForSingleObject(value->clock_thread, INFINITE);
        CloseHandle(value->clock_thread);
    }
    if (value->clock_stop != NULL) CloseHandle(value->clock_stop);
    if (value->clock_intel_address != 0u)
        (void)VdmRemoveVirtualMemory(value->clock_intel_address);
    if (value->clock_page != NULL)
        (void)VirtualFree((void *)value->clock_page, 0u, MEM_RELEASE);
    if (value->table_allocation != 0u)
        (void)VdmFreeVirtualMemory(value->table_allocation);
    if (value->client_allocation != 0u)
        (void)VdmFreeVirtualMemory(value->client_allocation);
    if (value->wow_gdt != 0u)
        (void)VdmFreeVirtualMemory(value->wow_gdt);
    ZeroMemory(value, sizeof(*value));
}

static void page_domain_store_descriptor(ULONG gdt, ULONG selector,
    ULONG base, ULONG limit)
{
    ULONG offset = selector & ~7u;
    ULONG low = (limit & 0xffffu) | ((base & 0xffffu) << 16);
    ULONG high = ((base >> 16) & 0xffu) | 0x0000f200u |
        (((limit >> 16) & 0xfu) << 16) | (base & 0xff000000u);

    if (limit > 0xffffu) high |= 0x00c00000u; /* G=1, default 32-bit */

    c_sas_storedw(gdt + offset, low);
    c_sas_storedw(gdt + offset + 4u, high);
}

static int page_domain_create_client_view(void)
{
    ULONG address = 0u;
    NTSTATUS status;

    status = VdmAllocateVirtualMemory(&address, MVDM_SOFTPC_CLIENT_VIEW_BYTES,
        TRUE);
    if (status < 0) return 0;
    domain.client_allocation = address;
    domain.guest_shared_info = address;
    domain.guest_server_info = address + MVDM_SOFTPC_PAGE_BYTES;
    domain.guest_handle_table = address + 2u * MVDM_SOFTPC_PAGE_BYTES;
    domain.guest_teb = address + 3u * MVDM_SOFTPC_PAGE_BYTES;
    c_sas_fills(address, 0u, MVDM_SOFTPC_CLIENT_VIEW_BYTES);

    /* SHAREDINFO: gpsi->psi and gpsi->aheList. The table's contents and
     * SERVERINFO count are owned by the later original object producers;
     * this carrier must not manufacture an initial window/handle graph. */
    c_sas_storedw(domain.guest_shared_info, domain.guest_server_info);
    c_sas_storedw(domain.guest_shared_info + 4u, domain.guest_handle_table);

    /* Native NT's context switch makes FS:18h a self pointer. This projection
     * only publishes that source-defined guest-linear value; it never aliases
     * host TLS/TEB. CLIENTINFO begins at TEB+44h and is deliberately all zero
     * until its original task/desktop owners publish it. */
    c_sas_storedw(domain.guest_teb + MVDM_SOFTPC_TEB_SELF, domain.guest_teb);
    return 1;
}

static int page_domain_create_wow_gdt(void)
{
    ULONG address = 0u;
    ULONG source = c_getGDT_BASE();
    USHORT limit = c_getGDT_LIMIT();
    ULONG bytes;
    ULONG allocation_bytes;
    uint8_t *image;
    NTSTATUS status;

    if (source == 0u || limit < MVDM_SOFTPC_KGDT_R3_TEB + 7u) return 0;
    bytes = (ULONG)limit + 1u;
    allocation_bytes = (bytes + MVDM_SOFTPC_PAGE_BYTES - 1u) &
        ~(MVDM_SOFTPC_PAGE_BYTES - 1u);
    if (allocation_bytes < bytes) return 0;
    image = HeapAlloc(GetProcessHeap(), 0u, bytes);
    if (image == NULL) return 0;
    status = VdmAllocateVirtualMemory(&address, allocation_bytes, TRUE);
    if (status < 0) {
        HeapFree(GetProcessHeap(), 0u, image);
        return 0;
    }
    domain.wow_gdt = address;
    domain.dosx_gdt = source;
    domain.dosx_gdt_limit = limit;
    c_sas_loads(source, image, bytes);
    c_sas_stores(address, image, bytes);
    HeapFree(GetProcessHeap(), 0u, image);
    page_domain_store_descriptor(address, MVDM_SOFTPC_KGDT_R3_DATA, 0u,
        0xffffffffu);
    page_domain_store_descriptor(address, MVDM_SOFTPC_KGDT_R3_TEB,
        domain.guest_teb, MVDM_SOFTPC_TEB_BYTES - 1u);
    return 1;
}

static int page_domain_create(session *owner)
{
    ULONG sas_bytes = c_sas_memory_size();
    ULONG identity_tables;
    ULONG table_pages;
    ULONG index;
    ULONG allocation;
    ULONG clock_table;
    NTSTATUS status;

    page_domain_report("wow-domain-create-begin");
    /* The fixed USER clock mapping is above the one-megabyte A20 boundary.
     * DOSX owns the real-mode-to-protected-mode A20 transition; accepting a
     * wrapped SAS view here would silently bind 7FFE:0000 to the wrong page. */
    if (c_sas_twenty_bit_wrapping_enabled() || sas_bytes == 0u ||
        (sas_bytes & (MVDM_SOFTPC_PAGE_BYTES - 1u)) != 0u)
        return 0;
    identity_tables = (sas_bytes + 0x3fffffu) >> 22;
    table_pages = 2u + identity_tables;
    if (table_pages > 0xffffffffu / MVDM_SOFTPC_PAGE_BYTES)
        return 0;
    domain.table_bytes = table_pages * MVDM_SOFTPC_PAGE_BYTES;
    allocation = 0u;
    status = VdmAllocateVirtualMemory(&allocation, domain.table_bytes, TRUE);
    if (status < 0) goto fail;
    domain.table_allocation = allocation;
    domain.directory = allocation;
    c_sas_fills(allocation, 0u, domain.table_bytes);
    for (index = 0u; index < identity_tables; ++index) {
        ULONG table = allocation + MVDM_SOFTPC_PAGE_BYTES * (1u + index);
        ULONG first = index << 22;
        ULONG count = sas_bytes - first;
        ULONG page;

        if (count > 0x400000u) count = 0x400000u;
        c_sas_storedw(allocation + index * 4u,
            table | MVDM_SOFTPC_PAGE_PRESENT_USER);
        for (page = 0u; page < (count >> 12); ++page)
            c_sas_storedw(table + page * 4u,
                (first + page * MVDM_SOFTPC_PAGE_BYTES) |
                MVDM_SOFTPC_PAGE_PRESENT_USER);
    }
    page_domain_report("wow-domain-identity-ready");
    page_domain_report("wow-domain-clock-allocate");
    domain.clock_page = VirtualAlloc(NULL, MVDM_SOFTPC_PAGE_BYTES,
        MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (domain.clock_page == NULL) goto fail;
    page_domain_report("wow-domain-clock-allocated");
    domain.clock_page[0] = GetTickCount();
    domain.clock_page[1] = 0x01000000u;
    page_domain_report("wow-domain-clock-register");
    status = VdmAddVirtualMemory((ULONG)(ULONG_PTR)domain.clock_page,
        MVDM_SOFTPC_PAGE_BYTES, &domain.clock_intel_address);
    if (status < 0) goto fail;
    page_domain_report("wow-domain-clock-registered");
    clock_table = allocation + MVDM_SOFTPC_PAGE_BYTES * (1u + identity_tables);
    c_sas_storedw(allocation + ((MVDM_SOFTPC_SHARED_CLOCK_LINEAR >> 22) * 4u),
        clock_table | MVDM_SOFTPC_PAGE_PRESENT_USER);
    c_sas_storedw(clock_table +
        (((MVDM_SOFTPC_SHARED_CLOCK_LINEAR >> 12) & 1023u) * 4u),
        domain.clock_intel_address | MVDM_SOFTPC_PAGE_PRESENT_USER_READ);
    page_domain_report("wow-domain-clock-ready");
    domain.clock_stop = CreateEventA(NULL, TRUE, FALSE, NULL);
    if (domain.clock_stop == NULL) goto fail;
    domain.clock_thread = CreateThread(NULL, 0u, clock_publisher, &domain, 0u,
        NULL);
    if (domain.clock_thread == NULL) goto fail;
    page_domain_report("wow-domain-thread-ready");
    if (!page_domain_create_client_view() || !page_domain_create_wow_gdt())
        goto fail;
    page_domain_report("wow-domain-view-ready");
    domain.owner = owner;
    if (!session_register_teardown(owner, page_domain_dispose, &domain)) goto fail;
    domain.active = 1;
    return 1;
fail:
    page_domain_report("wow-domain-create-failed");
    page_domain_dispose(&domain);
    return 0;
}

int mvdm_softpc_wow_page_domain_enter(void)
{
    session *owner = session_thread_current();

    if (owner == NULL || !session_valid(owner) || (c_getCR0() & 1u) == 0u)
        return 0;
    if (!domain.active && !page_domain_create(owner)) return 0;
    if (domain.owner != owner) return 0;
    c_setCR3(domain.directory);
    c_setCR0(c_getCR0() | MVDM_SOFTPC_CR0_PG);
    page_domain_report("wow-domain-entered");
    return 1;
}

void mvdm_softpc_wow_page_domain_leave_protected(void)
{
    if (domain.wow_context_active) {
        c_setGDT_BASE_LIMIT(domain.dosx_gdt, domain.dosx_gdt_limit);
        domain.wow_context_active = 0;
    }
    if (domain.active && (c_getCR0() & MVDM_SOFTPC_CR0_PG) != 0u)
        c_setCR0(c_getCR0() & ~MVDM_SOFTPC_CR0_PG);
    page_domain_report("wow-domain-left");
}

int mvdm_softpc_wow_page_domain_reenter_protected(void)
{
    if (!domain.active) return 1;
    if ((c_getCR0() & 1u) == 0u) return 0;
    c_setCR3(domain.directory);
    c_setCR0(c_getCR0() | MVDM_SOFTPC_CR0_PG);
    page_domain_report("wow-domain-reentered");
    return 1;
}

int mvdm_softpc_wow_page_domain_active(void)
{
    return domain.active;
}

int mvdm_softpc_wow_page_domain_activate_wow_context(void)
{
    if (!domain.active || domain.wow_gdt == 0u ||
        (c_getCR0() & MVDM_SOFTPC_CR0_PG) == 0u) return 0;
    c_setGDT_BASE_LIMIT(domain.wow_gdt, domain.dosx_gdt_limit);
    domain.wow_context_active = 1;
    page_domain_report("wow-domain-wow-gdt-active");
    return 1;
}

unsigned long mvdm_softpc_wow_page_domain_guest_shared_info(void)
{
    return domain.active ? domain.guest_shared_info : 0u;
}
