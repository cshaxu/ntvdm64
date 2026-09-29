#ifndef NTCON_IO_H
#define NTCON_IO_H
#include <windows.h>
/* Local byte transfer only; peer/stop/event remain caller-owned capabilities. */
DWORD ntcon_channel_transfer(HANDLE,HANDLE,HANDLE,HANDLE,BOOL,void *,DWORD);
#endif
