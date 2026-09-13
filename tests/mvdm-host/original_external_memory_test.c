#include <stdint.h>
#include <stdlib.h>
#include <windows.h>
#include "session/session.h"
#include "adapter-mvdm-host-out/softpc/include/mvdm_softpc_guest_memory.h"
#include "adapter-mvdm-host-out/softpc/include/mvdm_softpc_physical_mapping.h"

/* Link the formal CCPU40/nt_mem libraries, never replacement memory bodies. */
extern void sas_init(uint32_t);
extern void sas_term(void);
extern void *setup_global_data_ptr(void);
extern void setup_vga_globals(void);
extern uint8_t *c_GetPhyAdd(uint32_t);
extern void c_sas_loads(uint32_t, uint8_t *, uint32_t);
extern void c_sas_stores(uint32_t, uint8_t *, uint32_t);
extern void c_sas_move_bytes_forward(uint32_t, uint32_t, uint32_t);
extern LONG VdmAddVirtualMemory(ULONG, ULONG, PULONG);
extern LONG VdmRemoveVirtualMemory(ULONG);
extern LONG VdmAllocateVirtualMemory(PULONG, ULONG, BOOL);
extern LONG VdmFreeVirtualMemory(ULONG);
extern LONG VdmReallocateVirtualMemory(ULONG, PULONG, ULONG);
extern int host_copy_con_to_con(int, unsigned short, unsigned short,
    unsigned short, unsigned short);
extern void fwd_word_fill(unsigned short, unsigned char *, int);
extern PBYTE get_aligned_disk_buffer(void);
extern DWORD disk_buffer_pool, max_align_factor, cur_align_factor;

static void report(const char *message)
{
    DWORD written;
    WriteFile(GetStdHandle(STD_ERROR_HANDLE), message,
        (DWORD)lstrlenA(message), &written, NULL);
}
#define CHECK(c) do { if (!(c)) { \
    report("failed: " #c "\n"); return 1; \
} } while (0)

static LONG WINAPI report_exception(EXCEPTION_POINTERS *e)
{
    char message[160];
    wsprintfA(message, "exception=%08lx image=%p address=%p\n",
        e->ExceptionRecord->ExceptionCode, GetModuleHandle(NULL),
        e->ExceptionRecord->ExceptionAddress);
    report(message);
    return EXCEPTION_EXECUTE_HANDLER;
}

int main(void)
{
    session owner;
    uint8_t *external = VirtualAlloc(NULL, 8192,
        MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    uint8_t *resolved;
    ULONG guest = 0, normal;
    void *gdp;
    SetUnhandledExceptionFilter(report_exception);
    {
        static const DWORD masks[] = {0, 1, 3, 15, 511, 4095};
        unsigned i;
        CHECK(disk_buffer_pool == 0);
        max_align_factor = 4095;
        for (i = 0; i < sizeof(masks) / sizeof(masks[0]); ++i) {
            PBYTE aligned;
            cur_align_factor = masks[i];
            aligned = get_aligned_disk_buffer();
            CHECK(aligned != NULL);
            CHECK(((uintptr_t)aligned & masks[i]) == 0);
            CHECK((uintptr_t)aligned >= disk_buffer_pool &&
                (uintptr_t)aligned - disk_buffer_pool <= masks[i]);
            aligned[0] = 0x12;
            aligned[511] = 0x34;
            CHECK(aligned[0] == 0x12 && aligned[511] == 0x34);
        }
        free((void *)(uintptr_t)disk_buffer_pool);
        disk_buffer_pool = max_align_factor = cur_align_factor = 0;
        report("original disk-buffer alignment PASS (no disk I/O)\n");
    }
    {
        uint8_t buffer[96];
        uint32_t digest = 2166136261u;
        unsigned offset, words, i;
        char message[80];
        for (offset = 4; offset < 8; ++offset) {
            for (words = 0; words <= 32; ++words) {
                for (i = 0; i < sizeof(buffer); ++i) buffer[i] = 0xcc;
                fwd_word_fill(0x1234, buffer + offset, words);
                for (i = 0; i < sizeof(buffer); ++i) {
                    if (i < offset || i >= offset + words * 2)
                        CHECK(buffer[i] == 0xcc);
                    digest = (digest ^ buffer[i]) * 16777619u;
                }
            }
        }
        wsprintfA(message, "word-fill 132 alignment/length cases digest=%08lx\n",
            (ULONG)digest);
        report(message);
        CHECK(digest == 0x91153575u);
    }
    session_initialize(&owner, 406);
    CHECK(external && session_activate(&owner) && session_thread_bind(&owner));
    gdp = setup_global_data_ptr();
    CHECK(gdp != NULL);
    report("before sas_init\n");
    sas_init(0x200000);
    report("after sas_init\n");
    CHECK(mvdm_softpc_guest_memory_begin(&owner));
    setup_vga_globals();
    {
        ULONG unavailable = 0;
        CHECK(VdmAddVirtualMemory((ULONG)(uintptr_t)(external + 1),
            0x40000000, &unavailable) == (LONG)0xc0000017);
        CHECK(external[1] == 0);
    }
    /* No publication/prepare call: this is the unchanged WOW caller shape. */
    CHECK(VdmAddVirtualMemory((ULONG)(uintptr_t)(external + 1), 4096, &guest) == 0);
    report("after original add\n");
    CHECK((guest & 4095) == 1);
    CHECK(c_GetPhyAdd(guest) == external + 1);
    *c_GetPhyAdd(guest) = 0x5a;
    CHECK(external[1] == 0x5a);
    external[4096] = 0xa5;
    CHECK(*c_GetPhyAdd(guest + 4095) == 0xa5);
    CHECK(c_GetPhyAdd((guest & ~4095u) + 8191) == external + 8191);
    CHECK(VdmMapDosMemory(0x100, guest >> 12, 2) == 0);
    CHECK(c_GetPhyAdd(0x100001) == external + 1);
    *c_GetPhyAdd(0x101000) = 0x7e;
    CHECK(external[4096] == 0x7e);
    CHECK(VdmUnmapDosMemory(0x100, 2) == 0);
    {
        uint8_t bytes[4] = {0};
        uint8_t replacement[4] = {0x31, 0x32, 0x33, 0x34};
        /* Adjacent guest pages deliberately point to reversed host pages. */
        CHECK(VdmMapDosMemory(0x80, (guest >> 12) + 1, 1) == 0);
        CHECK(VdmMapDosMemory(0x81, guest >> 12, 1) == 0);
        external[8190] = 0x11;
        external[8191] = 0x12;
        external[0] = 0x13;
        external[1] = 0x14;
        c_sas_loads(0x80ffe, bytes, 4);
        CHECK(bytes[0] == 0x11 && bytes[1] == 0x12 &&
            bytes[2] == 0x13 && bytes[3] == 0x14);
        c_sas_stores(0x80ffe, replacement, 4);
        CHECK(external[8190] == 0x31 && external[8191] == 0x32 &&
            external[0] == 0x33 && external[1] == 0x34);
        c_sas_move_bytes_forward(0x80ffe, 0x70000, 4);
        c_sas_loads(0x70000, bytes, 4);
        CHECK(bytes[0] == 0x31 && bytes[1] == 0x32 &&
            bytes[2] == 0x33 && bytes[3] == 0x34);
        /* Original EMS dispatch reaches nt_emm's lease binding only because
         * this conventional span crosses independently mapped alias pages. */
        external[8190] = 0x41;
        external[8191] = 0x42;
        external[0] = 0x43;
        external[1] = 0x44;
        CHECK(host_copy_con_to_con(4, 0x8000, 0x0ffe, 0x7000, 0) == 0);
        c_sas_loads(0x70000, bytes, 4);
        CHECK(bytes[0] == 0x41 && bytes[1] == 0x42 &&
            bytes[2] == 0x43 && bytes[3] == 0x44);
        bytes[0] = 0x51; bytes[1] = 0x52; bytes[2] = 0x53; bytes[3] = 0x54;
        c_sas_stores(0x70000, bytes, 4);
        CHECK(host_copy_con_to_con(4, 0x7000, 0, 0x8000, 0x0ffe) == 0);
        CHECK(external[8190] == 0x51 && external[8191] == 0x52 &&
            external[0] == 0x53 && external[1] == 0x54);
        report("EMS cross-window original nt_emm lease copy PASS\n");
        CHECK(VdmUnmapDosMemory(0x80, 1) == 0);
        CHECK(VdmUnmapDosMemory(0x81, 1) == 0);
        external[1] = 0x5a;
        report("original SAS reversed-page loads/stores/move PASS\n");
    }
    normal = guest & ~4095u;
    CHECK(VdmRemoveVirtualMemory(guest) == 0);
    CHECK(!mvdm_softpc_physical_mapping_resolve(guest, &resolved));
    CHECK(VdmAllocateVirtualMemory(&normal, 8192, TRUE) == 0);
    CHECK(normal == (guest & ~4095u));
    CHECK(c_GetPhyAdd(normal + 1) != external + 1);
    *c_GetPhyAdd(normal + 1) = 0x19;
    CHECK(external[1] == 0x5a);
    {
        ULONG resized = 0;
        CHECK(VdmReallocateVirtualMemory(normal, &resized, 16384) == 0);
        CHECK(*c_GetPhyAdd(resized + 1) == 0x19);
        normal = resized;
        CHECK(VdmReallocateVirtualMemory(normal, &resized, 4096) == 0);
        CHECK(*c_GetPhyAdd(resized + 1) == 0x19);
        normal = resized;
        report("original allocation grow/shrink content PASS\n");
    }
    CHECK(VdmFreeVirtualMemory(normal) == 0);
    {
        BITMAPINFO info = {0};
        void *bits = NULL;
        HDC dc = CreateCompatibleDC(NULL);
        HBITMAP bitmap;
        HGDIOBJ prior;
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = 4;
        info.bmiHeader.biHeight = -4;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        bitmap = CreateDIBSection(dc, &info, DIB_RGB_COLORS, &bits, NULL, 0);
        CHECK(dc && bitmap && bits);
        prior = SelectObject(dc, bitmap);
        guest = 0;
        CHECK(VdmAddVirtualMemory((ULONG)(uintptr_t)bits, 64, &guest) == 0);
        CHECK(SetPixelV(dc, 0, 0, RGB(0x12, 0x34, 0x56)));
        CHECK(GdiFlush());
        CHECK(*c_GetPhyAdd(guest) == 0x56);
        CHECK(*c_GetPhyAdd(guest + 2) == 0x12);
        *c_GetPhyAdd(guest + 2) = 0xab;
        CHECK(GetPixel(dc, 0, 0) == RGB(0xab, 0x34, 0x56));
        CHECK(VdmRemoveVirtualMemory(guest) == 0);
        CHECK(!mvdm_softpc_physical_mapping_resolve(guest, &resolved));
        SelectObject(dc, prior);
        CHECK(DeleteObject(bitmap) && DeleteDC(dc));
        report("real DIB GDI/guest bidirectional sharing PASS\n");
    }
    mvdm_softpc_guest_memory_end(&owner);
    sas_term();
    free(gdp);
    CHECK(session_thread_unbind(&owner) && session_dispose(&owner));
    CHECK(VirtualFree(external, 0, MEM_RELEASE));
    report("original add/remove unaligned sharing and normal-backing PASS\n");
    return 0;
}
