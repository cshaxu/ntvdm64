#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <oemuni.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

static int scenario, creates, removes, posts, errors, closed, bad;
static USHORT ax, bp, bx, cx, carry;
static CHAR guest_name[64];
static WCHAR removed[2][64];
static HANDLE WINAPI create_provider(LPCWSTR name, DWORD access, DWORD share,
    LPSECURITY_ATTRIBUTES sa, DWORD disposition, DWORD attributes, HANDLE template_file)
{
    (void)attributes; (void)template_file;
    ++creates;
    if (name[3] != 0xe9 || access != (GENERIC_READ|GENERIC_WRITE) ||
        share != (FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE) ||
        !sa->bInheritHandle || disposition != CREATE_ALWAYS) bad = 1;
    if (creates == 1 || scenario == 3 || scenario == 4 || scenario == 5) {
        SetLastError(ERROR_SHARING_VIOLATION); return INVALID_HANDLE_VALUE;
    }
    return (HANDLE)0x1234;
}
static BOOL WINAPI remove_provider(LPCWSTR name)
{
    if (removes >= 2) { bad = 1; return FALSE; }
    wcscpy(removed[removes++], name);
    return scenario == 1 || scenario == 3 || (scenario == 2 && removes == 2);
}
static BOOL WINAPI post_provider(HWND window, UINT message, WPARAM w, LPARAM l)
{
    if (window != HWND_BROADCAST || message != WM_FONTCHANGE || w || l) bad = 1;
    ++posts; return TRUE;
}
static DWORD WINAPI size_provider(HANDLE handle, LPDWORD high)
{ if (handle != (HANDLE)0x1234) bad = 1; *high = 0; return 0; }
static BOOL WINAPI close_provider(HANDLE handle)
{ (void)handle; ++closed; return TRUE; }
#define CreateFileW create_provider
#define RemoveFontResourceW remove_provider
#include "../../../src/mvdm/oemuni/file.c"

/* Test-only CPU/guest-memory inputs, not a replacement DEM implementation. */
typedef struct { int unused; } mvdm_guest_location;
static int mvdm_guest_location_set_real_mode(mvdm_guest_location *p, USHORT s, USHORT o)
{ (void)p; (void)s; (void)o; return 1; }
static int mvdm_guest_location_copy_c_string(mvdm_guest_location *p, uint8_t *out,
    uint32_t capacity, ULONG *bytes)
{ (void)p; *bytes = (ULONG)strlen(guest_name)+1; if (*bytes > capacity) return 0; memcpy(out,guest_name,*bytes); return 1; }
#define getDS() 0
#define getSI() 0
#define getCX() 0
#define setAX(v) (ax=(v))
#define setBP(v) (bp=(v))
#define setBX(v) (bx=(v))
#define setCX(v) (cx=(v))
#define setCF(v) (carry=(v))
#define DEM_CREATE 0
#define ATTR_VOLUME_ID 8
#define DRIVEBYTE 0
#define LABELOFF 3
static USHORT demCreateLabel(CHAR drive, PCHAR label)
{ (void)drive; (void)label; bad=1; return 1; }
static BOOL IsCdRomFile(PSTR path) { (void)path; return FALSE; }
static VOID demClientError(HANDLE h, CHAR drive)
{ (void)h; (void)drive; ++errors; carry=1; ax=(USHORT)GetLastError(); }
#define PostMessage post_provider
#define GetFileSize size_provider
#define CloseHandle close_provider
#include "dem-create-body.inc"

int main(void)
{
    if (GetOEMCP()!=437) return 1;
    for (scenario=1; scenario<=5; ++scenario) {
        creates=removes=posts=errors=closed=bad=0; carry=1;
        strcpy(guest_name, scenario==5 ? "C:\\\x82.DAT" : "C:\\\x82.TTF");
        demCreateCommon(DEM_CREATE);
        if (bad || closed) return 2;
        if (scenario<=2) {
            if (creates!=2 || posts!=1 || errors || carry || bp!=0x1234 || ax || bx || cx) return 3;
        } else if (!carry || errors!=1 || ax!=ERROR_SHARING_VIOLATION) return 4;
        if (scenario==1 && removes!=1) return 5;
        if (scenario==2 && (removes!=2 || wcscmp(removed[1],L"C:\\\x00e9.FOT"))) return 6;
        if (scenario==3 && (creates!=2 || removes!=1 || posts!=1)) return 7;
        if (scenario==4 && (creates!=1 || removes!=2 || posts)) return 8;
        if (scenario==5 && (creates!=1 || removes || posts)) return 9;
        printf("S37_DEM_FONT_RETRY scenario=%d creates=%d removes=%d posts=%d errors=%d PASS\n",
            scenario,creates,removes,posts,errors);
    }
    return 0;
}
