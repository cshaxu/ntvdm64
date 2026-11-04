/* Process-local endpoint identity, never a wire record. */
#ifndef BASE_SRV_SCOPE_H
#define BASE_SRV_SCOPE_H
#include <windows.h>
#include <rpc.h>
#define BROKER_RPC_ENDPOINT_FORMAT L"ntvdm-basesrv-%lu-%08lx-%08lx-%ls"
#define BROKER_PACKAGE_ID_HEX_CHARS 32u
#define BROKER_PACKAGE_RESOURCE L"NTVDM_PACKAGE_ID"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct broker_rpc_scope {
    LUID logon;
    DWORD session;
    BOOL valid;
    WCHAR package_id[BROKER_PACKAGE_ID_HEX_CHARS+1u];
} broker_rpc_scope;
BOOL broker_rpc_capture_scope(broker_rpc_scope *scope);
#ifdef __cplusplus
}
#endif
#endif
