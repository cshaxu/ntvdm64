#include <windows.h>
#include <stdio.h>

/* Compile the extracted current WU32LoadCursor body, not a copied algorithm.
 * Its resource access and 16-bit alias manager are observed at their actual
 * call boundary; the native output callback is deliberately a finite stub. */
typedef WORD HAND16;
typedef ULONG HAND32;
typedef char *PSZ;
typedef struct {
    WORD f1;
    DWORD f2;
    DWORD f3;
    DWORD f4;
    WORD f5;
    DWORD f6;
    LPTSTR f7;
} LOADCURSOR16, *PLOADCURSOR16;
typedef LOADCURSOR16 *PVDMFRAME;

static unsigned errors, output_errors, alias_errors, return_errors;
static unsigned step, calls, aliases, has_data;
static BYTE resource[8];
static PSZ resource_name = (PSZ)MAKEINTRESOURCEA(42);
static LPTSTR expected_type;
static WORD expected_kind;

#define FASTCALL
#define GETARGPTR(frame,size,arg) do { (void)(size); ++step; (arg)=(frame); } while (0)
#define GETPSZIDPTR(vp,p) do { (void)(vp); ++step; (p)=resource_name; } while (0)
#define GETMISCPTR(vp,p) do { (void)(vp); ++step; (p)=has_data ? resource : NULL; } while (0)
#define FETCHWORD(value) ((WORD)(value))
#define HINSTRES32(value) ((HANDLE)(ULONG_PTR)(value))
#define FREEMISCPTR(p) do { (void)(p); ++step; } while (0)
#define FREEPSZIDPTR(p) do { (void)(p); ++step; } while (0)
#define FREEARGPTR(p) do { (void)(p); ++step; } while (0)
#define RETURN(value) return (value)
#define HANDLE_TYPE_CURSOR 0x01
#define HANDLE_TYPE_ICON 0x02

static BOOL MBToWCS(PSZ source, int source_count, LPWSTR *destination,
    int destination_count, BOOL allocate)
{
    (void)source; (void)source_count; (void)destination;
    (void)destination_count; (void)allocate;
    return FALSE;
}

static HANDLE WINAPI output(HANDLE module, LPTSTR module_name,
    DWORD version, LPCTSTR name, DWORD size, PVOID bytes, LPTSTR type,
    BOOL client_side)
{
    ++calls;
    if (module != (HANDLE)(ULONG_PTR)7 || !module_name ||
        version != 0x30a || name != (LPCTSTR)resource_name ||
        size != 8 || bytes != (has_data ? resource : NULL) ||
        type != expected_type || client_side) { ++errors; ++output_errors; }
    return (HANDLE)(ULONG_PTR)0x4567;
}

static ULONG SetupResCursorIconAlias(HAND16 instance, HAND32 handle,
    PSZ name, HAND16 resource_handle, WORD kind)
{
    ++aliases;
    if (instance != 7 || handle != 0x4567 || name != resource_name ||
        resource_handle != 9 || kind != expected_kind) { ++errors; ++alias_errors; }
    return handle;
}
static struct {
    HANDLE (WINAPI *pfnServerLoadCreateCursorIcon)(HANDLE, LPTSTR, DWORD,
        LPCTSTR, DWORD, PVOID, LPTSTR, BOOL);
} pfnOut = { output };

#pragma warning(disable: 4305 4306)
#include "cursor-call.inc"

static void run(ULONG_PTR type, BOOL bytes)
{
    LOADCURSOR16 frame = {7, 0, 0, 8, 9, 0x30a, (LPTSTR)type};
    ULONG result;
    has_data = bytes;
    expected_type = (LPTSTR)type;
    expected_kind = (WORD)type == (WORD)(ULONG_PTR)RT_CURSOR ?
        HANDLE_TYPE_CURSOR : HANDLE_TYPE_ICON;
    step = 0;
    result = WU32LoadCursor(&frame);
    if (result != 0x4567 || step != 6) { ++errors; ++return_errors; }
}

int __cdecl main(void)
{
    run((ULONG_PTR)RT_CURSOR, TRUE);
    run((ULONG_PTR)RT_ICON, TRUE);
    run((ULONG_PTR)RT_CURSOR, FALSE);
    if (calls != 3 || aliases != 3) ++errors;
    printf("WOW_CURSOR_CALL_ORDER errors=%u cases=3 output=%u aliases=%u output_errors=%u alias_errors=%u return_errors=%u\n",
        errors, calls, aliases, output_errors, alias_errors, return_errors);
    return errors ? 3 : 0;
}
