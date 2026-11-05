#ifndef WIN32_PROCESS_IMAGE_H
#define WIN32_PROCESS_IMAGE_H
#include <windows.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Outputs are written on success only; the process handle is borrowed. */
/* Preserves the launcher MZ/non-PE classification, not a full NE parser. */
BOOL win32_process_image_is_legacy(PCWSTR path);
DWORD win32_process_image_machine(HANDLE process,WORD *machine);
DWORD win32_process_image_validate(PCWSTR path,WORD machine,BOOL dll);
DWORD win32_process_image_validate_ex(PCWSTR path,WORD machine,BOOL dll,WORD *subsystem);
#ifdef __cplusplus
}
#endif
#endif
