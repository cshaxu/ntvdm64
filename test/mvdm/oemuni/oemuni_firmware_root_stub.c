/* Test-only boundary for OEM process wrappers.  The production owner obtains
 * these roots through the session-aware firmware provider; this fixture needs
 * only real Unicode roots to exercise the original OEM narrowing behavior. */
#include <windows.h>
#include <stdint.h>

uint32_t NtvdmGetWindowsDirectoryW(wchar_t *path_out, uint32_t capacity)
{
    return GetWindowsDirectoryW(path_out, capacity);
}

uint32_t NtvdmGetSystemDirectoryW(wchar_t *path_out, uint32_t capacity)
{
    return GetSystemDirectoryW(path_out, capacity);
}
