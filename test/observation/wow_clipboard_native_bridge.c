/* Native half of the independent Win16 Clipboard witness. It has no product
 * role: it only proves the public Clipboard view after selected original
 * WU32* calls, and is loaded by the disposable CLIPPROBE.EXE. */
#include <windows.h>
#include <string.h>
#ifdef CLIPBOARD_RETIREMENT_WITNESS
#include <stdlib.h>
#include <stdio.h>
#endif

#define CLIP_BRIDGE_OK 0x447A0001u

#ifdef CLIPBOARD_DIB
#define CLIPBOARD_FORMAT CF_DIB
/* Exact peers of the independently authored 1x1 24-bit guest DIBs. */
static const BYTE dib_guest[]    = {40,0,0,0,1,0,0,0,1,0,0,0,1,0,24,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x31,0x22,0x13,0};
static const BYTE dib_native[]   = {40,0,0,0,1,0,0,0,1,0,0,0,1,0,24,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x32,0x23,0x14,0};
static const BYTE dib_deferred[] = {40,0,0,0,1,0,0,0,1,0,0,0,1,0,24,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x34,0x25,0x16,0};
static const BYTE dib_replace[]  = {40,0,0,0,1,0,0,0,1,0,0,0,1,0,24,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x35,0x26,0x17,0};
#else
#define CLIPBOARD_FORMAT CF_TEXT
#endif

typedef BOOL (WINAPI *clipboard_open_fn)(HWND);
typedef BOOL (WINAPI *clipboard_close_fn)(void);
typedef BOOL (WINAPI *clipboard_empty_fn)(void);
typedef HANDLE (WINAPI *clipboard_get_fn)(UINT);
typedef HANDLE (WINAPI *clipboard_set_fn)(UINT,HANDLE);
typedef HWND (WINAPI *clipboard_open_window_fn)(void);
typedef DWORD (WINAPI *clipboard_window_process_fn)(HWND,DWORD *);

typedef struct clipboard_api {
    clipboard_open_fn open;
    clipboard_close_fn close;
    clipboard_empty_fn empty;
    clipboard_get_fn get;
    clipboard_set_fn set;
    clipboard_open_window_fn open_window;
    clipboard_window_process_fn window_process;
} clipboard_api;

static void clipboard_trace(const char *stage, DWORD value, DWORD error)
{
    char path[MAX_PATH], line[160];
    HANDLE file;
    DWORD written;
    if (!GetEnvironmentVariableA("MVDM_WOW_CLIPBOARD_BRIDGE_TRACE_PATH", path,
            sizeof(path))) return;
    file=CreateFileA(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,
        NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    if (file==INVALID_HANDLE_VALUE) return;
    wsprintfA(line,"%s value=%08lx error=%lu\r\n",stage,value,error);
    WriteFile(file,line,(DWORD)strlen(line),&written,NULL);
    CloseHandle(file);
}

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
    api->open_window=(clipboard_open_window_fn)GetProcAddress(user,"GetOpenClipboardWindow");
    api->window_process=(clipboard_window_process_fn)GetProcAddress(user,"GetWindowThreadProcessId");
    return api->open && api->close && api->empty && api->get && api->set &&
        api->open_window && api->window_process;
}

static BOOL clipboard_text_is(const char *expected)
{
    clipboard_api api;
    HANDLE data;
    const char *text;
    BOOL match=FALSE;
    if (!clipboard_resolve(&api)) {
        clipboard_trace("resolve",0,GetLastError()); return FALSE;
    }
    if (!api.open(NULL)) {
        DWORD error=GetLastError();
        HWND owner=api.open_window();
        DWORD pid=0;
        DWORD thread=api.window_process(owner,&pid);
        clipboard_trace("open",(DWORD)(ULONG_PTR)owner,error);
        clipboard_trace("open-owner-pid",pid,ERROR_SUCCESS);
        clipboard_trace("open-owner-thread",thread,ERROR_SUCCESS);
        clipboard_trace("open-current-thread",GetCurrentThreadId(),ERROR_SUCCESS);
        return FALSE;
    }
    data=api.get(CF_TEXT);
    if (data && (text=(const char *)GlobalLock(data))!=NULL) {
        match=strcmp(text,expected)==0;
        GlobalUnlock(data);
        clipboard_trace(match ? "match" : "mismatch",(DWORD)(ULONG_PTR)data,
            match ? ERROR_SUCCESS : ERROR_INVALID_DATA);
    } else {
        clipboard_trace(data ? "lock" : "get",(DWORD)(ULONG_PTR)data,
            GetLastError());
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

#ifdef CLIPBOARD_DIB
static BOOL clipboard_dib_is(const BYTE *expected)
{
    clipboard_api api; HANDLE data; const BYTE *bytes; BOOL match=FALSE;
    if (!clipboard_resolve(&api) || !api.open(NULL)) return FALSE;
    data=api.get(CLIPBOARD_FORMAT);
    if (data && (bytes=(const BYTE *)GlobalLock(data))!=NULL) {
        SIZE_T size=GlobalSize(data);
        /* Win16 GlobalAlloc rounds this 44-byte DIB block to its original
         * movable allocation quantum. Copyh16Toh32 correctly preserves that
         * full allocation; prove the declared DIB prefix, not an invented
         * exact GlobalSize contract. */
        match=size>=sizeof(dib_guest) && memcmp(bytes,expected,sizeof(dib_guest))==0;
        clipboard_trace(match ? "dib-match" : "dib-mismatch",(DWORD)size,
            match ? ERROR_SUCCESS : ERROR_INVALID_DATA);
        if (!match && size >= 4) clipboard_trace("dib-first-dword",
            *(const DWORD *)bytes,ERROR_INVALID_DATA);
        GlobalUnlock(data);
    }
    api.close(); return match;
}
static BOOL clipboard_set_dib(const BYTE *value)
{
    clipboard_api api; HGLOBAL data; BYTE *bytes; BOOL result=FALSE;
    if (!clipboard_resolve(&api) || !api.open(NULL)) return FALSE;
    if (!api.empty()) goto done;
    data=GlobalAlloc(GMEM_MOVEABLE,sizeof(dib_guest)); if (!data) goto done;
    bytes=(BYTE *)GlobalLock(data); if (!bytes) {GlobalFree(data);goto done;}
    memcpy(bytes,value,sizeof(dib_guest)); GlobalUnlock(data);
    if (!api.set(CLIPBOARD_FORMAT,data)) {GlobalFree(data);goto done;}
    result=TRUE;
done: api.close(); return result;
}

#endif

__declspec(dllexport) DWORD WINAPI ClipboardBridge(DWORD operation,
    DWORD unused_one, DWORD unused_two)
{
    (void)unused_one;
    (void)unused_two;
    if (operation==1) {
#ifdef CLIPBOARD_DIB
        return clipboard_dib_is(dib_guest) ? CLIP_BRIDGE_OK : 0;
#else
        return clipboard_text_is("guest clipboard") ? CLIP_BRIDGE_OK : 0;
#endif
    }
    if (operation==2) {
#ifdef CLIPBOARD_DIB
        return clipboard_set_dib(dib_native) ? CLIP_BRIDGE_OK : 0;
#else
        return clipboard_set_text("native clipboard") ? CLIP_BRIDGE_OK : 0;
#endif
    }
    if (operation==3) {
#ifdef CLIPBOARD_DIB
        return clipboard_dib_is(dib_native) ? CLIP_BRIDGE_OK : 0;
#else
        return clipboard_text_is("native clipboard") ? CLIP_BRIDGE_OK : 0;
#endif
    }
    if (operation==4) {
#ifdef CLIPBOARD_DIB
        return clipboard_dib_is(dib_deferred) ? CLIP_BRIDGE_OK : 0;
#else
        return clipboard_text_is("deferred clipboard") ? CLIP_BRIDGE_OK : 0;
#endif
    }
    if (operation==5) {
#ifdef CLIPBOARD_DIB
        return clipboard_set_dib(dib_replace) ? CLIP_BRIDGE_OK : 0;
#else
        return clipboard_set_text("replacement clipboard") ? CLIP_BRIDGE_OK : 0;
#endif
    }
    if (operation==6) {
        clipboard_trace("renderall-entry",0,ERROR_SUCCESS);
        return CLIP_BRIDGE_OK;
    }
    if (operation==7) {
        BOOL rendered=
#ifdef CLIPBOARD_DIB
            clipboard_dib_is(dib_deferred);
#else
            clipboard_text_is("deferred clipboard");
#endif
        clipboard_trace("renderall-verify",rendered,rendered ? ERROR_SUCCESS : GetLastError());
        return rendered ? CLIP_BRIDGE_OK : 0;
    }
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0;
}

#ifdef CLIPBOARD_RETIREMENT_WITNESS
/* Headless post-exit observation, not a Clipboard manager or input driver. */
int main(int argc, char **argv)
{
    char *end;
    unsigned long value;
    HWND departed;
    if (argc != 2) return 64;
    value = strtoul(argv[1], &end, 16);
    if (!*argv[1] || *end || !value) return 64;
    departed = (HWND)(ULONG_PTR)value;
    if (IsWindow(departed) || GetClipboardOwner() == departed) {
        fprintf(stderr, "Clipboard owner survives task exit: %08lx\n", value);
        return 1;
    }
    printf("CLIPBOARD_TASK_EXIT_OK native_window_retired=1 owner_retired=1\n");
    return 0;
}
#endif
