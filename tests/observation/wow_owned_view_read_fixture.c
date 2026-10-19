/* Exact production leaf bodies, with explicit RAM/context stand-ins.
 * Operation counts are a local mechanism proof, not actual input latency. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MVDM_SOFTPC_PAGE_BYTES 4096u
#define MVDM_SOFTPC_WND_HEAD_HANDLE 0u
#define MVDM_SOFTPC_HANDLE_ENTRY_BYTES 16u
typedef struct mvdm_softpc_wow_window_borrow {
    ULONG backing;
    int retired;
} mvdm_softpc_wow_window_borrow;
static struct {
    ULONG desktop_handle, guest_desktop_window, guest_handle_table;
    ULONG guest_windows[0x10000];
} domain;
static BYTE ram[2 * 1024 * 1024];
static ULONG scalar_reads, mapped_reads, fallback_reads;
static int errors, context_active = 1;
static void check(int value, const char *name)
{
    if (!value) { ++errors; printf("FAIL %s\n", name); }
}
static void *checked_address(ULONG address, ULONG bytes)
{
    if (address > sizeof(ram) || bytes > sizeof(ram) - address) {
        fprintf(stderr, "Invalid fixture RAM span %lu/%lu\n", address, bytes);
        exit(93);
    }
    return ram + address;
}
static BYTE *c_GetLinAdd(ULONG address)
{
    ++mapped_reads;
    return checked_address(address, 1);
}
ULONG c_sas_dw_at(ULONG address)
{
    ULONG value;
    ++scalar_reads;
    CopyMemory(&value, checked_address(address, sizeof(value)), sizeof(value));
    return value;
}
static void c_sas_loads(ULONG address, void *output, ULONG bytes)
{
    ++fallback_reads;
    CopyMemory(output, checked_address(address, bytes), bytes);
}
static int page_domain_menu_context(void) { return context_active; }
static void store(ULONG address, ULONG value)
{
    CopyMemory(checked_address(address, sizeof(value)), &value, sizeof(value));
}
#include "owned_view_read_body.h"

int main(int argc, char **argv)
{
    const ULONG first = 0x00010042u, reused = 0x00020042u;
    ULONG value = 0, before, iteration, expected_scalar;
    BYTE cross[8] = {0};
    mvdm_softpc_wow_window_borrow borrow = {0x2000, 0};
    if (argc != 2 || (strcmp(argv[1], "--fast") && strcmp(argv[1], "--scalar"))) return 64;
    expected_scalar = !strcmp(argv[1], "--scalar");
    domain.desktop_handle = 0x00010001;
    domain.guest_desktop_window = 0x1000;
    domain.guest_handle_table = 0x10000;
    domain.guest_windows[LOWORD(first)] = borrow.backing;
    store(borrow.backing, first);
    store(domain.guest_handle_table + LOWORD(first) * 16u + 8u, 0x00010001);
    SetLastError(0x12345678);
    before = scalar_reads + mapped_reads;
    check(!page_domain_window_lookup(0) && !page_domain_window_lookup(0xffff) &&
        !page_domain_window_lookup(0x00020001), "invalid/window-desktop-generation");
    check(page_domain_window_lookup(domain.desktop_handle) == 0x1000,
        "exact-desktop-special-identity");
    check(scalar_reads + mapped_reads == before, "invalid-identity-does-not-map");
    for (iteration = 0; iteration < 128; ++iteration) {
        check(page_domain_window_lookup(first) == borrow.backing, "published-exact-generation");
        check(!page_domain_window_lookup(reused), "stale-generation-rejected");
        check(window_borrow_is_current(&borrow), "current-retained-record");
        check(mvdm_softpc_wow_page_domain_window_handle(LOWORD(first)) == first,
            "public-full-handle");
        check(mvdm_softpc_wow_page_domain_handle_metadata(LOWORD(first), &value) &&
            value == 0x00010001, "public-handle-metadata");
    }
    check(GetLastError() == 0x12345678, "successful-read-preserves-error");
    check(expected_scalar ? scalar_reads == 640 && !mapped_reads :
        !scalar_reads && mapped_reads == 640, "exact-selected-read-operation-counts");
    before = scalar_reads + mapped_reads;
    borrow.retired = 1;
    check(!window_borrow_is_current(&borrow), "retired-record-not-current");
    check(scalar_reads + mapped_reads == before, "retired-record-does-not-read");
    borrow.retired = 0;
    domain.guest_windows[LOWORD(first)] = 0x3000;
    store(0x3000, reused);
    check(!window_borrow_is_current(&borrow), "old-backing-replaced-by-new-generation");
    check(page_domain_window_lookup(reused) == 0x3000 && !page_domain_window_lookup(first),
        "replacement-never-aliases-old-full-handle");
    domain.guest_windows[LOWORD(first)] = 0;
    check(!page_domain_window_lookup(reused) &&
        !mvdm_softpc_wow_page_domain_window_handle(LOWORD(first)), "withdrawn-slot");
    check(!mvdm_softpc_wow_page_domain_window_handle(0) &&
        !mvdm_softpc_wow_page_domain_window_handle(0xffff) &&
        mvdm_softpc_wow_page_domain_window_handle(1) == domain.desktop_handle,
        "invalid-and-desktop-public-slots");
    context_active = 0;
    before = scalar_reads + mapped_reads;
    check(!mvdm_softpc_wow_page_domain_handle_metadata(1, &value) &&
        !mvdm_softpc_wow_page_domain_window_handle(1), "inactive-context");
    context_active = 1;
    check(!mvdm_softpc_wow_page_domain_handle_metadata(1, NULL), "invalid-output");
    check(scalar_reads + mapped_reads == before, "refused-public-read-does-not-map");
    store(0x4ffcu, 0x11223344);
    store(0x5000u, 0x55667788);
    value = page_domain_read_owned_word(0x4ffcu);
    check(value == 0x11223344, "last-page-fitting-scalar");
    page_domain_copy_owned_ram(0x4ffcu, cross, sizeof(cross));
    CopyMemory(&value, cross + 4, sizeof(value));
    check(fallback_reads == 1 && value == 0x55667788, "cross-page-keeps-original-load-service");
    printf("OWNED_VIEW_READ errors=%d selected=%s generation=1 retirement=1 "
        "metadata=1 fallback=1 LastError=1 scalar=%lu mapped=%lu\n",
        errors, expected_scalar ? "scalar" : "fast", scalar_reads, mapped_reads);
    return errors ? 1 : 0;
}
