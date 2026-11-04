#include <windows.h>
#include <rpc.h>
#include <rpcasync.h>
#include <stdio.h>
static RPC_STATUS rpc_status;
static ULONG_PTR rpc_pid;

static int locality;
static DWORD process_pid, wait_result;
static RPC_STATUS mock_inquiry(RPC_BINDING_HANDLE binding, void *attributes)
{
    RPC_CALL_ATTRIBUTES_V2_W *call=attributes;
    (void)binding;
    if (call->Version!=2 || call->Flags!=(RPC_QUERY_CLIENT_PID | RPC_QUERY_IS_CLIENT_LOCAL)) return RPC_S_INVALID_ARG;
    call->ClientPID=(HANDLE)rpc_pid; call->IsClientLocal=locality; return rpc_status;
}
static DWORD mock_process_id(HANDLE process) { (void)process; return process_pid; }
static DWORD mock_wait(HANDLE process,DWORD timeout) { (void)process; (void)timeout; return wait_result; }
#define RpcServerInqCallAttributesW mock_inquiry
#define GetProcessId mock_process_id
#define WaitForSingleObject mock_wait
#include "lib-win32/rpc/peer.c"
#define CHECK(x) do { if(!(x)){fprintf(stderr,"peer check failed line %d\n",__LINE__); return 1;} ++checks; } while(0)
int main(void)
{
    DWORD pid=99; int checks=0;
    rpc_status=RPC_S_OK;rpc_pid=42;process_pid=42;locality=rcclLocal;wait_result=WAIT_TIMEOUT;
    CHECK(win32_rpc_local_peer_process(NULL,(HANDLE)1,NULL)==RPC_S_INVALID_ARG);
    CHECK(win32_rpc_local_peer_process(NULL,(HANDLE)1,&pid)==RPC_S_OK && pid==42);
    rpc_status=RPC_S_SERVER_UNAVAILABLE;pid=99;
    CHECK(win32_rpc_local_peer_process(NULL,(HANDLE)1,&pid)==RPC_S_SERVER_UNAVAILABLE && pid==0);
    rpc_status=RPC_S_OK;locality=rcclRemote;pid=99;
    CHECK(win32_rpc_local_peer_process(NULL,(HANDLE)1,&pid)==RPC_S_ACCESS_DENIED && pid==0);
    locality=rcclLocal;rpc_pid=43;pid=99;
    CHECK(win32_rpc_local_peer_process(NULL,(HANDLE)1,&pid)==RPC_S_ACCESS_DENIED && pid==0);
    rpc_pid=42;process_pid=0;pid=99;
    CHECK(win32_rpc_local_peer_process(NULL,(HANDLE)1,&pid)==RPC_S_ACCESS_DENIED && pid==0);
    process_pid=42;wait_result=WAIT_OBJECT_0;pid=99;
    CHECK(win32_rpc_local_peer_process(NULL,(HANDLE)1,&pid)==RPC_S_ACCESS_DENIED && pid==0);
    wait_result=WAIT_FAILED;pid=99;
    CHECK(win32_rpc_local_peer_process(NULL,(HANDLE)1,&pid)==RPC_S_ACCESS_DENIED && pid==0);
    printf("PASS Win32 RPC peer matching checks=%d\n",checks);return 0;
}
