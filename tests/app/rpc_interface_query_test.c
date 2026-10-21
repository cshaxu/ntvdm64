/* Actual host RPC management provider, isolated process-qualified endpoint.
 * Links the production query and generated client, not a mock completion. */
#include <windows.h>
#include <rpc.h>
#include <stdio.h>
#include <stdlib.h>
#include "common/rpc/interface_query.h"

static volatile LONG mode,calls,allocations;
static unsigned checks,failures;
#define CHECK(x) do{++checks;if(!(x)){++failures;fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);}}while(0)
void *__RPC_USER MIDL_user_allocate(size_t bytes)
{void *ptr=malloc(bytes);if(ptr)InterlockedIncrement(&allocations);return ptr;}
void __RPC_USER MIDL_user_free(void *ptr)
{if(ptr){InterlockedDecrement(&allocations);free(ptr);}}
static int RPC_ENTRY authorize(RPC_BINDING_HANDLE binding,unsigned long op,RPC_STATUS *status)
{
    LONG action=InterlockedCompareExchange(&mode,0,0);
    (void)binding;(void)op;InterlockedIncrement(&calls);*status=RPC_S_OK;
    if(action==1){*status=RPC_S_ACCESS_DENIED;return FALSE;}
    if(action==2)Sleep(12000);
    if(action==3)Sleep(4000);
    return TRUE;
}
int main(void)
{
    RPC_SERVER_INTERFACE iface={0};
    RPC_IF_ID expected;
    RPC_BINDING_HANDLE binding=NULL;
    RPC_WSTR text=NULL;
    WCHAR endpoint[80];
    DWORD error,handles_before=0,handles_after=0,index;
    ULONGLONG start,elapsed;
    iface.Length=sizeof(iface);
    UuidFromStringA((RPC_CSTR)"77a49066-7aa9-42cd-96cb-ea379da350c4",&iface.InterfaceId.SyntaxGUID);
    iface.InterfaceId.SyntaxVersion.MajorVersion=1;
    UuidFromStringA((RPC_CSTR)"8a885d04-1ceb-11c9-9fe8-08002b104860",&iface.TransferSyntax.SyntaxGUID);
    iface.TransferSyntax.SyntaxVersion.MajorVersion=2;
    expected.Uuid=iface.InterfaceId.SyntaxGUID;expected.VersMajor=1;expected.VersMinor=0;
    swprintf_s(endpoint,80,L"ntvdm-interface-query-%lu",GetCurrentProcessId());
    error=RpcServerUseProtseqEpW((RPC_WSTR)L"ncalrpc",10,(RPC_WSTR)endpoint,NULL);
    if(!error)error=RpcServerRegisterIf(&iface,NULL,NULL);
    if(!error)error=RpcMgmtSetAuthorizationFn(authorize);
    if(!error)error=RpcServerListen(1,10,TRUE);
    if(!error)error=RpcStringBindingComposeW(NULL,(RPC_WSTR)L"ncalrpc",NULL,(RPC_WSTR)endpoint,NULL,&text);
    if(!error)error=RpcBindingFromStringBindingW(text,&binding);
    if(text)RpcStringFreeW(&text);
    CHECK(!error);if(error)goto done;
    CHECK(common_rpc_classify_interface(binding,&expected,GetTickCount64()+10000)==RPC_S_SERVER_UNAVAILABLE);
    expected.VersMajor=2;
    CHECK(common_rpc_classify_interface(binding,&expected,GetTickCount64()+10000)==ERROR_REVISION_MISMATCH);
    expected.VersMajor=1;expected.VersMinor=1;
    CHECK(common_rpc_classify_interface(binding,&expected,GetTickCount64()+10000)==ERROR_REVISION_MISMATCH);
    ++expected.Uuid.Data1;
    CHECK(common_rpc_classify_interface(binding,&expected,GetTickCount64()+10000)==RPC_S_SERVER_UNAVAILABLE);
    --expected.Uuid.Data1;
    InterlockedExchange(&mode,1);
    CHECK(common_rpc_classify_interface(binding,&expected,GetTickCount64()+10000)==RPC_S_ACCESS_DENIED);
    index=(DWORD)InterlockedCompareExchange(&calls,0,0);
    CHECK(common_rpc_classify_interface(binding,&expected,GetTickCount64()-1)==ERROR_TIMEOUT);
    CHECK((DWORD)InterlockedCompareExchange(&calls,0,0)==index);
    InterlockedExchange(&mode,0);
    GetProcessHandleCount(GetCurrentProcess(),&handles_before);
    for(index=0;index<16;++index)
        CHECK(common_rpc_classify_interface(binding,&expected,GetTickCount64()+10000)==ERROR_REVISION_MISMATCH);
    GetProcessHandleCount(GetCurrentProcess(),&handles_after);
    CHECK(handles_before==handles_after);
    InterlockedExchange(&mode,2);start=GetTickCount64();
    CHECK(common_rpc_classify_interface(binding,&expected,start+10000)==ERROR_TIMEOUT);
    elapsed=GetTickCount64()-start;CHECK(elapsed>=9000 && elapsed<11000);
    printf("deadline elapsed=%llu handles-before=%lu after=%lu\n",elapsed,handles_before,handles_after);
    InterlockedExchange(&mode,0);
    CHECK(RpcServerUnregisterIf(&iface,NULL,TRUE)==RPC_S_OK);
    CHECK(common_rpc_classify_interface(binding,&expected,GetTickCount64()+10000)==RPC_S_NO_INTERFACES);
    CHECK(RpcServerRegisterIf(&iface,NULL,NULL)==RPC_S_OK);
    InterlockedExchange(&mode,3);start=GetTickCount64();
    /* Model a 10s connection budget with only 1.5s left for diagnosis. */
    CHECK(common_rpc_classify_interface(binding,&expected,start+1500)==ERROR_TIMEOUT);
    elapsed=GetTickCount64()-start;CHECK(elapsed>=1400 && elapsed<2500);
    printf("remaining-budget elapsed=%llu\n",elapsed);
done:
    if(binding)RpcBindingFree(&binding);
    RpcMgmtStopServerListening(NULL);RpcMgmtWaitServerListen();
    RpcServerUnregisterIf(NULL,NULL,TRUE);
    CHECK(!InterlockedCompareExchange(&allocations,0,0));
    printf("RPC-INTERFACE-QUERY checks=%u failures=%u allocations=%ld\n",checks,failures,allocations);
    return failures?1:0;
}
