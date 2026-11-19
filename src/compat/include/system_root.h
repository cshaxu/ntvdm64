/* Product directory mechanics only. Not a task, loader or environment owner. */
#ifndef COMPAT_SYSTEM_ROOT_H
#define COMPAT_SYSTEM_ROOT_H
#include <windows.h>
#include <stdint.h>

/* Status-returning APIs clear output on failure. Capacity includes NUL.
 * Images live in <root>\system32; root is that directory's parent.
 * The actual loaded EXE is the only authority; no environment/CWD fallback. */
#ifdef __cplusplus
extern "C" {
#endif
DWORD compat_system_root_w(PWSTR output,DWORD capacity);
DWORD compat_system_root_a(PSTR output,DWORD capacity);
DWORD compat_product_path_w(PCWSTR relative,PWSTR output,DWORD capacity);
DWORD compat_product_path_a(PCWSTR relative,PSTR output,DWORD capacity);
/* Directory-shaped standalone equivalents of the original host-directory
 * queries.  They expose the selected worker package, not the host Windows
 * installation.  Return values follow GetWindowsDirectoryA/W and
 * GetSystemDirectoryA/W: zero is failure, a too-small caller buffer receives
 * the required character count including NUL, otherwise the copied length
 * excludes NUL. */
uint32_t __cdecl NtvdmGetWindowsDirectoryA(char *path_out,
    uint32_t path_out_chars);
uint32_t __cdecl NtvdmGetSystemDirectoryA(char *path_out,
    uint32_t path_out_chars);
uint32_t __cdecl NtvdmGetWindowsDirectoryW(wchar_t *path_out,
    uint32_t path_out_chars);
uint32_t __cdecl NtvdmGetSystemDirectoryW(wchar_t *path_out,
    uint32_t path_out_chars);

#ifdef __cplusplus
}
#endif
#endif
