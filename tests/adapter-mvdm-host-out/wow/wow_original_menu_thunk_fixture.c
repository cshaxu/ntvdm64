#include "precomp.h"
#include "wow_private_user_compat.h"
#include "wow_menu_parser_bindings.h"

/* Actual original thunks and conversion bodies; only guest address lookup,
 * allocation failure and final handle observation are controlled here. */
static BYTE guest[] = {0,0,0,0,0x80,1,23,0,'O','w','n','e','r',0};
static unsigned errors, allocations, reject_alloc, mappings, output_calls;
static HMENU last_menu;
static DWORD guest_base = 0x12340020;
static char guest_name[] = "MENU";
static BOOL named;
PFNWOWHANDLERSOUT pfnOut;
#define CHECK(x) do { if (!(x)) { ++errors; printf("FAIL line=%d\n", __LINE__); } } while (0)

PVOID FASTCALL GetPModeVDMPointerAssert(DWORD address)
{
    if (address == 0x23450020) return guest_name;
    CHECK(address == guest_base);
    ++mappings;
    return guest;
}
WORD GetExePtr16(WORD module)
{
    CHECK(module == 9);
    return 9;
}
PVOID FASTCALL malloc_w(ULONG size)
{
    PVOID p;
    if (reject_alloc) return NULL;
    p = HeapAlloc(GetProcessHeap(), 0, size);
    if (p) ++allocations;
    return p;
}
VOID FASTCALL free_w(PVOID p)
{
    CHECK(allocations != 0);
    if (p) { --allocations; CHECK(HeapFree(GetProcessHeap(), 0, p)); }
}
static HMENU WINAPI observe_indirect(const MENUTEMPLATEW *data)
{
    ++output_calls;
    return last_menu = wow_user_load_menu_indirect(data);
}
static HMENU WINAPI observe_resource(HANDLE module, LPTSTR name,
    CONST LPMENUTEMPLATE data, DWORD size, BOOL client)
{
    CHECK((DWORD)module == 0x00090009 && !client);
    if (named) CHECK(!lstrcmpW((LPCWSTR)name,L"MENU"));
    else CHECK((DWORD)name == 7);
    ++output_calls;
    return last_menu = wow_private_user_load_create_menu(module,name,data,size,client);
}
#undef LoadMenuIndirect
#define LoadMenuIndirect observe_indirect
#include "original-menu-convert.inc"
#include "original-menu-thunks.inc"

static void run(BOOL indirect, BOOL fail)
{
    BYTE storage[sizeof(VDMFRAME) + sizeof(LOADMENU16)] = {0};
    PVDMFRAME frame = (PVDMFRAME)storage;
    ULONG result;
    MENUITEMINFOW info = {0};
    reject_alloc = fail; mappings = output_calls = 0; last_menu = NULL;
    if (indirect) {
        ((PLOADMENUINDIRECT16)&frame->bArgs)->f1 = guest_base;
        result = WU32LoadMenuIndirect(frame);
    } else {
        PLOADMENU16 args = (PLOADMENU16)&frame->bArgs;
        args->f1=9; args->f2=named ? 0x23450020 : 7; args->f3=guest_base;
        args->f4=sizeof(guest); args->f5=0x300;
        result = WU32LoadMenu(frame);
    }
    CHECK(allocations == 0);
    CHECK(mappings == (indirect ? (fail ? 1u : 2u) : (fail ? 0u : 1u)));
    CHECK(output_calls == (fail ? 0u : 1u));
    if (fail) CHECK(result == 0 && last_menu == NULL);
    else {
        CHECK(last_menu != NULL && result == (WORD)(ULONG_PTR)last_menu);
        info.cbSize=sizeof(info); info.fMask=MIIM_DATA|MIIM_FTYPE|MIIM_ID;
        CHECK(GetMenuItemInfoW(last_menu,0,TRUE,&info));
        CHECK(info.wID == 23 && (info.fType & MFT_OWNERDRAW));
        CHECK(info.dwItemData == guest_base + 8);
        CHECK(DestroyMenu(last_menu));
    }
}
int __cdecl main(void)
{
    pfnOut.pfnServerLoadCreateMenu = observe_resource;
    run(FALSE,FALSE); run(TRUE,FALSE); run(FALSE,TRUE); run(TRUE,TRUE);
    named=TRUE; run(FALSE,FALSE); run(FALSE,TRUE);
    printf("WOW_ORIGINAL_MENU_THUNKS errors=%u cases=6 allocations=%u\n",errors,allocations);
    return errors != 0;
}
