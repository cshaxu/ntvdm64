/* Process-local client binding for the admitted BaseSrv VDM RPC facade.
 * The original BaseClient callers retain CsrClientCallServer's signature;
 * this header is app composition only and is never a command wire ABI. */
#ifndef OPENNT_BASE_RPC_CLIENT_H
#define OPENNT_BASE_RPC_CLIENT_H
#include <windows.h>
#include <stdint.h>
DWORD OpenNtBaseClientConnectCurrent(void);
void OpenNtBaseClientDisconnectCurrent(void);
/* App-only launch composition over the authenticated connection.  These do
 * not carry Console or command data: original CheckVDM owns both locally. */
DWORD OpenNtBaseClientReserveWorker(ULONG task,uint64_t *reservation);
DWORD OpenNtBaseClientPrepareWorker(uint64_t reservation,HANDLE worker);
DWORD OpenNtBaseClientReleaseWorker(uint64_t reservation);
#endif
