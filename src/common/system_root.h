/* Product directory mechanics only. Not a task, loader or environment owner. */
#ifndef COMMON_SYSTEM_ROOT_H
#define COMMON_SYSTEM_ROOT_H
#include <windows.h>

/* Status-returning APIs clear output on failure. Capacity includes NUL.
 * The actual loaded EXE is the only authority; no environment/CWD fallback. */
DWORD common_system_root_w(PWSTR output,DWORD capacity);
DWORD common_system_root_a(PSTR output,DWORD capacity);
DWORD common_product_path_w(PCWSTR relative,PWSTR output,DWORD capacity);
DWORD common_product_path_a(PCWSTR relative,PSTR output,DWORD capacity);
#endif
