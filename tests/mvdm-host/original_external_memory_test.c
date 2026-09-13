#include <stdint.h>
#include <stdlib.h>
#include <windows.h>
#include "session/session.h"
#include "adapter-mvdm-host-out/softpc/include/mvdm_softpc_physical_mapping.h"

/* Link the formal CCPU40/nt_mem libraries, never replacement memory bodies. */
extern void sas_init(uint32_t);
extern void sas_term(void);
extern void *setup_global_data_ptr(void);
extern void setup_vga_globals(void);
extern uint8_t *c_GetPhyAdd(uint32_t);
extern LONG VdmAddVirtualMemory(ULONG, ULONG, PULONG);
extern LONG VdmRemoveVirtualMemory(ULONG);
extern LONG VdmAllocateVirtualMemory(PULONG, ULONG, BOOL);
extern LONG VdmFreeVirtualMemory(ULONG);

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
    session_initialize(&owner, 406);
    CHECK(external && session_activate(&owner) && session_thread_bind(&owner));
    gdp = setup_global_data_ptr();
    CHECK(gdp != NULL);
    report("before sas_init\n");
    sas_init(0x200000);
    report("after sas_init\n");
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
    normal = guest & ~4095u;
    CHECK(VdmRemoveVirtualMemory(guest) == 0);
    CHECK(!mvdm_softpc_physical_mapping_resolve(guest, &resolved));
    CHECK(VdmAllocateVirtualMemory(&normal, 8192, TRUE) == 0);
    CHECK(normal == (guest & ~4095u));
    CHECK(c_GetPhyAdd(normal + 1) != external + 1);
    *c_GetPhyAdd(normal + 1) = 0x19;
    CHECK(external[1] == 0x5a);
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
    sas_term();
    free(gdp);
    CHECK(session_thread_unbind(&owner) && session_dispose(&owner));
    CHECK(VirtualFree(external, 0, MEM_RELEASE));
    report("original add/remove unaligned sharing and normal-backing PASS\n");
    return 0;
}
