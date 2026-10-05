#include <windows.h>
#include <lm.h>
/* Selected public NetAPI inputs only; real original provider logic remains in
 * vrnetapi.c. Test compilation replaces no register/status/copy algorithms. */
NET_API_STATUS WINAPI fixture_user_info(LPCWSTR, DWORD, LPBYTE *);
NET_API_STATUS WINAPI fixture_wksta_info(LPCWSTR, DWORD, LPBYTE *);
NET_API_STATUS WINAPI fixture_net_free(LPVOID);
#define NetWkstaUserGetInfo fixture_user_info
#define NetWkstaGetInfo fixture_wksta_info
#define NetApiBufferFree fixture_net_free
