/* OS image metadata shared by launcher, Hook and worker admission.
 * No filename search, worker policy or PE parser. */
#ifndef COMMON_NATIVE_IMAGE_H
#define COMMON_NATIVE_IMAGE_H
#include <windows.h>
#ifdef __cplusplus
extern "C" {
#endif
DWORD common_native_image(PCWSTR application,DWORD *machine,DWORD *subsystem);
/* Borrow process; return a Win32-openable native path, avoiding the caller's
 * WOW64 redirection. All outputs are cleared on failure. */
DWORD common_native_process_path(HANDLE process,PWSTR path,DWORD capacity);
DWORD common_native_process_image(HANDLE process,DWORD *machine,DWORD *subsystem);
#ifdef __cplusplus
}
#endif
#endif
