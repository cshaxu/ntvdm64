#include "xactsrv_wksta_minimal.h"
NET_API_STATUS WINAPI fixture_server_enum(LPCWSTR,DWORD,LPBYTE *,DWORD,LPDWORD,LPDWORD,DWORD,LPCWSTR,LPDWORD);
NET_API_STATUS WINAPI fixture_net_free(LPVOID);
#define NetServerEnum fixture_server_enum
#define NetApiBufferFree fixture_net_free
