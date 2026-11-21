/* Hook RPC uses the process heap; preserve allocation during DLL lifetime. */
#include <windows.h>
#include <rpc.h>

void *__RPC_USER MIDL_user_allocate(size_t bytes)
{ return HeapAlloc(GetProcessHeap(), 0, bytes); }
void __RPC_USER MIDL_user_free(void *value)
{ if (value) HeapFree(GetProcessHeap(), 0, value); }
