#ifndef WIN32_PROCESS_IMAGE_REMOTE_H
#define WIN32_PROCESS_IMAGE_REMOTE_H
#include <windows.h>
/* These helpers inspect an already-open process of the caller's bitness. */
DWORD win32_process_image_remote_header(HANDLE process,const void *base,IMAGE_NT_HEADERS *header);
BOOL win32_process_image_remote_executable(HANDLE process,const void *address,const void *base);
BOOL win32_process_image_remote_range(DWORD rva,SIZE_T bytes,DWORD image_size);
DWORD win32_process_image_remote_read(HANDLE process,const BYTE *base,DWORD rva,void *value,SIZE_T bytes,DWORD image_size);
#endif
