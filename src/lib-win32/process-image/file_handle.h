#ifndef WIN32_PROCESS_IMAGE_FILE_HANDLE_H
#define WIN32_PROCESS_IMAGE_FILE_HANDLE_H
#include <windows.h>
/* Caller supplies a synchronous file positioned at offset zero. Validates a
 * non-reparse DLL for the caller architecture; advances the borrowed file pointer. */
DWORD win32_process_image_validate_dll_handle(HANDLE file);
#endif
