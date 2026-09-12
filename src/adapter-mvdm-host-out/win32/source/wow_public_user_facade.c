/*
 * Narrow public USER mappings reached by the selected WOW32 provider.
 *
 * These preserve two original client-side call shapes only.  They neither
 * register WOW callbacks nor recreate a USER/CSRSS/Win32k service.
 */

#include <windows.h>
#include <string.h>

#include "adapter-mvdm-host-out/win32/include/wow32_provider_private.h"
#include "opennt-abi/source/public/internal/windows/inc/wingdip.h"

/* DIVERGENCE(ADAPTER-WIN32-050): OpenNT ntuser's MBToWCSEx allocates with
 * UserRtlAllocMem and WOW32 releases the reached allocations with LocalFree.
 * The selected calls use the same caller-owned pointer/result contract, so
 * use LocalAlloc rather than inventing a private heap or conversion cache. */
int MBToWCSEx(WORD code_page, LPCSTR ansi, int ansi_count, LPWSTR *wide_out,
    int wide_count, BOOL allocate)
{
    UINT effective_code_page;
    LPWSTR destination;
    int converted;

    if (ansi == NULL || wide_out == NULL || ansi_count == 0 || wide_count == 0)
        return 0;
    if (ansi_count == -1) {
        ansi_count = (int)strlen(ansi) + 1;
    } else if (ansi_count < -1) {
        return 0;
    }
    if (wide_count == -1) {
        if (!allocate) return 0;
        wide_count = ansi_count;
    } else if (wide_count < -1) {
        return 0;
    }

    destination = *wide_out;
    if (allocate) {
        destination = (LPWSTR)LocalAlloc(LMEM_FIXED,
            (SIZE_T)wide_count * sizeof(WCHAR));
        if (destination == NULL) return 0;
        *wide_out = destination;
    } else if (destination == NULL) {
        return 0;
    }

    effective_code_page = code_page == 0 ? CP_ACP : (UINT)code_page;
    converted = MultiByteToWideChar(effective_code_page, 0, ansi, ansi_count,
        destination, wide_count);
    if (converted == 0 && allocate) (void)LocalFree(destination);
    return converted;
}

/* DIVERGENCE(ADAPTER-WIN32-051): the selected WOW32 call supplies a converted
 * 16-bit dialog template and `SCDLG_ANSI`; the original ntuser helper then
 * chooses its ANSI client-dialog path.  Public USER32 supplies the matching
 * A/W modal APIs and their -1 failure direction.  No private template parser,
 * server registration, or callback policy is introduced. */
int WINAPI DialogBoxIndirectParamAorW(HINSTANCE module,
    LPCDLGTEMPLATEW dialog_template, HWND owner, DLGPROC dialog_proc,
    LPARAM init_param, UINT flags)
{
    if (dialog_template == NULL) return -1;
    if ((flags & SCDLG_ANSI) != 0) {
        return DialogBoxIndirectParamA(module,
            (LPCDLGTEMPLATEA)dialog_template, owner, dialog_proc, init_param);
    }
    return DialogBoxIndirectParamW(module, dialog_template, owner, dialog_proc,
        init_param);
}

/* DIVERGENCE(ADAPTER-WIN32-052): OpenNT's GetHFONT retrieves the font
 * currently selected by the DC.  The selected WOW32 caller only returns that
 * handle through its existing 16/32 handle conversion, which is exactly the
 * public GetCurrentObject(OBJ_FONT) contract. */
HFONT WINAPI GetHFONT(HDC device_context)
{
    return (HFONT)GetCurrentObject(device_context, OBJ_FONT);
}

/* DIVERGENCE(ADAPTER-WIN32-053): WOW32 supplies the ANSI 16-bit glyph code,
 * metrics, output buffer and MAT2 directly.  The public ANSI entry point has
 * the same caller-owned buffer, result-count and GDI error contract; no GDI
 * handle table or private glyph engine is reproduced here. */
DWORD WINAPI GetGlyphOutlineWow(HDC device_context, UINT character,
    UINT format, LPGLYPHMETRICS metrics, DWORD buffer_size, LPVOID buffer,
    const MAT2 *matrix)
{
    return GetGlyphOutlineA(device_context, character, format, metrics,
        buffer_size, buffer, matrix);
}

/* DIVERGENCE(ADAPTER-WIN32-054): OpenNT's public client ExtEscape handling
 * routes GETEXTENDEDTEXTMETRICS through GetETM, then updates exactly the
 * kerning-pair count from GetKerningPairsA.  Retain that same public escape,
 * structure size and BOOL result direction; no driver-font or private GDI
 * table is modeled. */
BOOL GetETM(HDC device_context, EXTTEXTMETRIC *metrics)
{
    int result;

    if (metrics == NULL) return FALSE;
    result = ExtEscape(device_context, GETEXTENDEDTEXTMETRICS, 0, NULL,
        sizeof(*metrics), (LPSTR)metrics);
    if (result == 0) return FALSE;
    metrics->etmNKernPairs = (WORD)GetKerningPairsA(device_context, 0, NULL);
    return TRUE;
}

typedef struct _WOW_NETWORK_FONT {
    struct _WOW_NETWORK_FONT *next;
    UINT owner_id;
    UINT load_count;
    CHAR path[MAX_PATH];
} WOW_NETWORK_FONT;

static INIT_ONCE wow_network_font_once = INIT_ONCE_STATIC_INIT;
static CRITICAL_SECTION wow_network_font_lock;
static WOW_NETWORK_FONT *wow_network_fonts;

static BOOL CALLBACK wow_network_font_initialize(PINIT_ONCE once,
    PVOID parameter, PVOID *context)
{
    UNREFERENCED_PARAMETER(once);
    UNREFERENCED_PARAMETER(parameter);
    UNREFERENCED_PARAMETER(context);
    InitializeCriticalSection(&wow_network_font_lock);
    return TRUE;
}

static BOOL wow_network_font_is_remote(LPCSTR path, LPSTR full_path)
{
    CHAR root[MAX_PATH];
    DWORD length;
    CHAR *share_end;

    length = GetFullPathNameA(path, MAX_PATH, full_path, NULL);
    if (length == 0 || length >= MAX_PATH) return FALSE;
    if (full_path[0] != '\\' || full_path[1] != '\\') {
        if (full_path[1] != ':' || full_path[2] != '\\') return FALSE;
        root[0] = full_path[0];
        root[1] = ':';
        root[2] = '\\';
        root[3] = '\0';
        return GetDriveTypeA(root) == DRIVE_REMOTE;
    }
    share_end = strchr(full_path + 2, '\\');
    if (share_end == NULL || share_end[1] == '\0') return FALSE;
    share_end = strchr(share_end + 1, '\\');
    if (share_end == NULL) {
        lstrcpynA(root, full_path, MAX_PATH);
        lstrcatA(root, "\\");
    } else {
        lstrcpynA(root, full_path, (int)(share_end - full_path) + 2);
    }
    return GetDriveTypeA(root) == DRIVE_REMOTE;
}

/* DIVERGENCE(ADAPTER-WIN32-055): preserve the OpenNT private font tracking
 * contract with public font APIs and a narrow task-id/reference-count list.
 * Only remote-volume fonts are tracked, and task teardown repeats the public
 * remove call once for every recorded load, matching the original ownership
 * rule without modeling GDI's font internals. */
int AddFontResourceTracking(LPCSTR path, UINT owner_id)
{
    WOW_NETWORK_FONT *entry;
    CHAR full_path[MAX_PATH];
    int result = AddFontResourceA(path);

    if (result == 0 || !wow_network_font_is_remote(path, full_path)) return result;
    (void)InitOnceExecuteOnce(&wow_network_font_once,
        wow_network_font_initialize, NULL, NULL);
    EnterCriticalSection(&wow_network_font_lock);
    for (entry = wow_network_fonts; entry != NULL; entry = entry->next) {
        if (entry->owner_id == owner_id && _stricmp(entry->path, full_path) == 0) {
            ++entry->load_count;
            LeaveCriticalSection(&wow_network_font_lock);
            return result;
        }
    }
    entry = (WOW_NETWORK_FONT *)LocalAlloc(LMEM_FIXED, sizeof(*entry));
    if (entry != NULL) {
        entry->owner_id = owner_id;
        entry->load_count = 1;
        lstrcpynA(entry->path, full_path, MAX_PATH);
        entry->next = wow_network_fonts;
        wow_network_fonts = entry;
    }
    LeaveCriticalSection(&wow_network_font_lock);
    return result;
}

int RemoveFontResourceTracking(LPCSTR path, UINT owner_id)
{
    WOW_NETWORK_FONT **link;
    WOW_NETWORK_FONT *entry;
    CHAR full_path[MAX_PATH];
    int result = RemoveFontResourceA(path);

    if (result == 0 || !wow_network_font_is_remote(path, full_path)) return result;
    (void)InitOnceExecuteOnce(&wow_network_font_once,
        wow_network_font_initialize, NULL, NULL);
    EnterCriticalSection(&wow_network_font_lock);
    for (link = &wow_network_fonts; (entry = *link) != NULL; link = &entry->next) {
        if (entry->owner_id == owner_id && _stricmp(entry->path, full_path) == 0) {
            if (--entry->load_count == 0) {
                *link = entry->next;
                (void)LocalFree(entry);
            }
            break;
        }
    }
    LeaveCriticalSection(&wow_network_font_lock);
    return result;
}

void UnloadNetworkFonts(UINT owner_id)
{
    WOW_NETWORK_FONT **link;
    WOW_NETWORK_FONT *entry;
    UINT count;

    (void)InitOnceExecuteOnce(&wow_network_font_once,
        wow_network_font_initialize, NULL, NULL);
    EnterCriticalSection(&wow_network_font_lock);
    for (link = &wow_network_fonts; (entry = *link) != NULL;) {
        if (entry->owner_id != owner_id) {
            link = &entry->next;
            continue;
        }
        *link = entry->next;
        count = entry->load_count;
        while (count-- != 0) (void)RemoveFontResourceA(entry->path);
        (void)LocalFree(entry);
    }
    LeaveCriticalSection(&wow_network_font_lock);
}

/* DIVERGENCE(ADAPTER-WIN32-039): the original private USER request changes
 * only NT4's server-managed start-glass presentation.  This selected product
 * has no CSRSS GUI presentation owner; retain the void call/order without
 * synthesizing a cursor, dialog or task result. */
void ShowStartGlass(DWORD timeout)
{
    UNREFERENCED_PARAMETER(timeout);
}

/* DIVERGENCE(ADAPTER-WIN32-056): OpenNT's GdiCleanCacheDC validates a private
 * local-DC tag, then returns TRUE without mutating the DC; its own source
 * calls out that a server-side cleanup would be preferable.  WOW calls it
 * only before the existing public GdiFlush.  Modern public HDCs do not expose
 * that private tag, so preserve the reached successful no-state direction. */
BOOL GdiCleanCacheDC(HDC device_context)
{
    UNREFERENCED_PARAMETER(device_context);
    return TRUE;
}
