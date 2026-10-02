#ifndef NTW32_IO_H
#define NTW32_IO_H
#include <windows.h>
/* Local byte transfer only; peer/stop/event remain caller-owned capabilities. */
DWORD ntw32_channel_transfer(HANDLE,HANDLE,HANDLE,HANDLE,BOOL,void *,DWORD);
#endif
