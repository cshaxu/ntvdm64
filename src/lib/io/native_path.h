#ifndef LIB_IO_NATIVE_PATH_H
#define LIB_IO_NATIVE_PATH_H
#include <windows.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Resolve a caller-selected file to a final DOS/UNC creation path. */
DWORD common_native_application_path(PCWSTR application,PWSTR path,DWORD capacity);
#ifdef __cplusplus
}
#endif
#endif
