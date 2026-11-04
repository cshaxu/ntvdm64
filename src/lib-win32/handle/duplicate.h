#ifndef LIB_WIN32_HANDLE_DUPLICATE_H
#define LIB_WIN32_HANDLE_DUPLICATE_H
#include <windows.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef BOOL (WINAPI *win32_handle_compare_fn)(HANDLE,HANDLE);
/* Returns NULL if the operating system does not provide object comparison.
 * The caller retains its own unavailable-capability and mismatch policy. */
win32_handle_compare_fn win32_handle_compare_provider(void);
/* Duplicate into target_process. Both process handles and source are borrowed;
 * a successful output belongs to target_process, not necessarily the caller. */
DWORD win32_handle_duplicate(HANDLE source_process,HANDLE source,HANDLE target_process,HANDLE *output);
/* Best-effort release of a handle owned by another process. The caller must
 * supply a process handle with PROCESS_DUP_HANDLE and serialize ownership. */
void win32_handle_close_remote(HANDLE owner_process,HANDLE remote_handle);
#ifdef __cplusplus
}
#endif
#endif
