/* Native half of the independent Win16 Clipboard witness. It has no product
 * role: it only proves the public Clipboard view after selected original
 * WU32* calls, and is loaded by the disposable CLIPPROBE.EXE. */
#include <windows.h>
#include <string.h>

#define CLIP_BRIDGE_OK 0x447A0001u

typedef BOOL (WINAPI *clipboard_open_fn)(HWND);
typedef BOOL (WINAPI *clipboard_close_fn)(void);
typedef BOOL (WINAPI *clipboard_empty_fn)(void);
typedef HANDLE (WINAPI *clipboard_get_fn)(UINT);
typedef HANDLE (WINAPI *clipboard_set_fn)(UINT,HANDLE);

typedef struct clipboard_api {
    clipboard_open_fn open;
    clipboard_close_fn close;
    clipboard_empty_fn empty;
    clipboard_get_fn get;
    clipboard_set_fn set;
} clipboard_api;

/* The witness is loaded through original LoadLibraryEx32W inside NTVDM.
 * Resolve USER at operation time so DLL load itself has the same minimal
 * kernel-only dependency boundary as the established GENHOST witness. */
static BOOL clipboard_resolve(clipboard_api *api)
{
    HMODULE user=GetModuleHandleA("user32.dll");
    if (!user) user=LoadLibraryA("user32.dll");
    if (!user) return FALSE;
    api->open=(clipboard_open_fn)GetProcAddress(user,"OpenClipboard");
    api->close=(clipboard_close_fn)GetProcAddress(user,"CloseClipboard");
    api->empty=(clipboard_empty_fn)GetProcAddress(user,"EmptyClipboard");
    api->get=(clipboard_get_fn)GetProcAddress(user,"GetClipboardData");
    api->set=(clipboard_set_fn)GetProcAddress(user,"SetClipboardData");
    return api->open && api->close && api->empty && api->get && api->set;
}

static BOOL clipboard_text_is(const char *expected)
{
    clipboard_api api;
    HANDLE data;
    const char *text;
    BOOL match=FALSE;
    if (!clipboard_resolve(&api) || !api.open(NULL)) return FALSE;
    data=api.get(CF_TEXT);
    if (data && (text=(const char *)GlobalLock(data))!=NULL) {
        match=strcmp(text,expected)==0;
        GlobalUnlock(data);
    }
    api.close();
    return match;
}

static BOOL clipboard_set_text(const char *value)
{
    clipboard_api api;
    SIZE_T length=strlen(value)+1;
    HGLOBAL data;
    char *text;
    BOOL result=FALSE;
    if (!clipboard_resolve(&api) || !api.open(NULL)) return FALSE;
    if (!api.empty()) goto done;
    data=GlobalAlloc(GMEM_MOVEABLE,length);
    if (!data) goto done;
    text=(char *)GlobalLock(data);
    if (!text) { GlobalFree(data); goto done; }
    memcpy(text,value,length);
    GlobalUnlock(data);
    if (!api.set(CF_TEXT,data)) { GlobalFree(data); goto done; }
    result=TRUE;
done:
    api.close();
    return result;
}

__declspec(dllexport) DWORD WINAPI ClipboardBridge(DWORD operation,
    DWORD unused_one, DWORD unused_two)
{
    (void)unused_one;
    (void)unused_two;
    if (operation==1) {
        return clipboard_text_is("guest clipboard") ? CLIP_BRIDGE_OK : 0;
    }
    if (operation==2) return clipboard_set_text("native clipboard") ? CLIP_BRIDGE_OK : 0;
    if (operation==3) {
        return clipboard_text_is("native clipboard") ? CLIP_BRIDGE_OK : 0;
    }
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0;
}
