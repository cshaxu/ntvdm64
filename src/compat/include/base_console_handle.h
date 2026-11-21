#ifndef BASE_CONSOLE_HANDLE_H
#define BASE_CONSOLE_HANDLE_H
#include <windows.h>
BOOL OpenNtBaseIsConsoleHandle(HANDLE);
/* Replace the three BASE presence markers with inheritable local handles.
 * On failure the caller retains every attachment for cleanup. */
DWORD OpenNtBaseAdoptStandardHandles(HANDLE streams[3],
    ULONG pipe_mask, ULONG pipe_count, HANDLE *pipes,
    ULONG file_mask, ULONG file_count, HANDLE *files);
#endif
