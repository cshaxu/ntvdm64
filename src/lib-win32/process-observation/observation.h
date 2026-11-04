#ifndef LIB_WIN32_PROCESS_OBSERVATION_H
#define LIB_WIN32_PROCESS_OBSERVATION_H
#include <windows.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Source handle and callback context are borrowed. A retained process is owned
 * by the caller. The caller keeps process/context alive until wait is drained. */
BOOL win32_process_observation_retain(HANDLE source,HANDLE *retained);
void win32_process_observation_identity(HANDLE process,DWORD *pid,FILETIME *created);
BOOL win32_process_observation_image(HANDLE process,WCHAR *image,DWORD capacity);
/* Queries inherited parent identity only; the caller decides its trust and
 * lifetime. Native failures retain the existing NOT_SUPPORTED mapping. */
DWORD win32_process_observation_parent(HANDLE process,ULONG_PTR *parent);
/* Wait registration is one-shot. Drain may block; caller must not hold a lock
 * needed by its callback, and must not release process/context on failure. */
BOOL win32_process_observation_watch(HANDLE *wait,HANDLE process,WAITORTIMERCALLBACK callback,void *context);
BOOL win32_process_observation_drain(HANDLE wait);
#ifdef __cplusplus
}
#endif
#endif
