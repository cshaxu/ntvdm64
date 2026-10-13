#ifndef OPENNT_BASE_RPC_CLIENT_LAUNCH_STYLE_H
#define OPENNT_BASE_RPC_CLIENT_LAUNCH_STYLE_H
#include <windows.h>
DWORD OpenNtBaseClientSetWindowStyle(DWORD style);
DWORD WINAPI OpenNtBaseClientWowStyle(ULONG task,DWORD *style);
DWORD OpenNtBaseClientDosWindowStyle(void);
DWORD OpenNtBaseClientSeparateWowStyle(DWORD *style);
#endif
