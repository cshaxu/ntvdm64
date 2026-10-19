/* Unpublished diagnostic copies only; never included by production source.
 * Record the completed native input/result without querying USER before the
 * guest's local IsWindow decision. Observe only its existing failure report. */
#include "wow_user_object_bindings.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"
typedef struct { HWND raw, full; BOOL result; } wow_diag_destroy_record;
#ifdef WOW_DIAG_DESTROY_STORAGE
__declspec(thread) wow_diag_destroy_record wow_diag_destroy;
#else
extern __declspec(thread) wow_diag_destroy_record wow_diag_destroy;
#endif
#ifdef WOW_DIAG_FAILURE_REPORT
static void wow_diag_failure_report(LPFNW32 thunk, PVDMFRAME frame)
{
    DWORD saved = GetLastError(), metadata = 0, owner = 0, written;
    FILEIOWRITE16 args;
    BYTE *data;
    WORD error, dialog;
    ULONG *pair;
    HWND current;
    HANDLE file;
    char path[MAX_PATH], line[512], kind[96] = {0};
    int bytes;
    if (thunk != WK32FileWrite) return;
    CopyMemory(&args, &frame->bArgs, sizeof(args));
    if (args.bufsize != 24 || !HIWORD(args.lpBuf)) return;
    data = (BYTE *)GetPModeVDMPointer(args.lpBuf, 24);
    if (!data || memcmp(data, "CDL2", 4)) { SetLastError(saved); return; }
    CopyMemory(&error, data + 6, 2);
    if (!error) { SetLastError(saved); return; }
    CopyMemory(&dialog, data + 18, 2);
    current = wow_user_window_native_handle((HWND)(ULONG_PTR)dialog);
    (void)mvdm_softpc_wow_page_domain_handle_metadata(dialog, &metadata);
    pair = (ULONG *)GetPModeVDMPointer(0x003b006cu, 8);
    if (current) {
        (void)GetWindowThreadProcessId(current, &owner);
        (void)GetClassNameA(current, kind, sizeof(kind));
    }
    bytes = sprintf_s(line, sizeof(line),
        "CDL_FAILURE code=%04X dialog=%04X raw=%08lX full=%08lX result=%d old_live=%d current=%08lX current_live=%d metadata=%08lX cached_hwnd=%08lX cached_wnd=%08lX current_pid=%lu class=%s\r\n",
        error, dialog, (ULONG)wow_diag_destroy.raw, (ULONG)wow_diag_destroy.full,
        wow_diag_destroy.result, IsWindow(wow_diag_destroy.full),
        (ULONG)current, IsWindow(current), metadata,
        pair ? pair[0] : 0, pair ? pair[1] : 0, owner, kind);
    if (GetEnvironmentVariableA("MVDM_WOW_DISPATCH_TRACE_PATH", path, sizeof(path))) {
        file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (file != INVALID_HANDLE_VALUE) {
            if (bytes > 0) (void)WriteFile(file, line, bytes, &written, NULL);
            CloseHandle(file);
        }
    }
    SetLastError(saved);
}
#endif
