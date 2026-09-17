/* Product-level management RPC fixture.  It starts the sibling BaseSrv,
 * exercises the same authenticated, connection-less wire calls used by
 * DTASKMGR, then terminates only that fixture-owned server. */
#include <windows.h>
#include <rpc.h>
#include <stdio.h>
#include <stdlib.h>
#include "service.h"
#include "basesrv-exe/transport/rpc_security.h"
#include "product-abi/version.h"

#define CHECK(value) do { if (!(value)) { fprintf(stderr,"FAIL %d: %lu\n",__LINE__,(unsigned long)GetLastError()); return 1; } } while (0)

void *__RPC_USER MIDL_user_allocate(size_t bytes) { return malloc(bytes); }
void __RPC_USER MIDL_user_free(void *value) { free(value); }

static const unsigned char version[APP_VERSION_BYTES]=APP_VERSION;

static RPC_BINDING_HANDLE bind_server(const broker_rpc_scope *scope)
{
    WCHAR endpoint[128];
    RPC_WSTR text=NULL;
    RPC_BINDING_HANDLE binding=NULL;
    RPC_STATUS status;
    wsprintfW(endpoint,L"ntvdm-basesrv-%lu-%08lx-%08lx",scope->session,
        (ULONG)scope->logon.HighPart,(ULONG)scope->logon.LowPart);
    status=RpcStringBindingComposeW(NULL,(RPC_WSTR)L"ncalrpc",NULL,(RPC_WSTR)endpoint,NULL,&text);
    if (status==RPC_S_OK) status=RpcBindingFromStringBindingW(text,&binding);
    if (text) RpcStringFreeW(&text);
    if (status==RPC_S_OK) status=RpcBindingSetAuthInfoW(binding,NULL,
        RPC_C_AUTHN_LEVEL_PKT_PRIVACY,RPC_C_AUTHN_WINNT,NULL,RPC_C_AUTHZ_NONE);
    if (status!=RPC_S_OK) { if (binding) RpcBindingFree(&binding); SetLastError(status); return NULL; }
    return binding;
}

int main(int argc,char **argv)
{
    broker_rpc_scope scope={0};
    RPC_BINDING_HANDLE binding;
    PROCESS_INFORMATION broker={0};
    STARTUPINFOW startup={sizeof(startup)};
    WCHAR path[MAX_PATH],*slash;
    HANDLE self;
    DWORD error=RPC_S_SERVER_UNAVAILABLE,attempt;
    hyper epoch=0,valid_epoch=0;
    ULONG count=0;
    DTASKMGR_WORKER *entries=NULL;
    BOOL existing=argc>=2 && (!_stricmp(argv[1],"--existing") || !_stricmp(argv[1],"--terminate"));
    BOOL terminate=argc==2 && !_stricmp(argv[1],"--terminate");
    ULONG index;
    if (argc!=1 && !existing) { fputs("usage: dtaskmgr-rpc-test [--existing]\n",stderr); return 2; }
    CHECK(broker_rpc_capture_scope(&scope));
    if (!existing) {
        CHECK(GetModuleFileNameW(NULL,path,MAX_PATH));
        slash=wcsrchr(path,L'\\'); CHECK(slash!=NULL); lstrcpyW(slash+1,L"basesrv.exe");
        CHECK(CreateProcessW(path,NULL,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&startup,&broker));
    }
    self=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,GetCurrentProcessId());
    CHECK(self!=NULL);
    binding=bind_server(&scope); CHECK(binding!=NULL);
    for (attempt=0;attempt<100;++attempt) {
        RpcTryExcept {
            error=Client_TaskSnapshot(binding,self,APP_PROTOCOL_VERSION,(unsigned char *)version,
                &epoch,&count,&entries);
        }
        RpcExcept(1) { error=RpcExceptionCode(); }
        RpcEndExcept
        if (error==ERROR_SUCCESS) break;
        Sleep(50);
    }
    CHECK(error==ERROR_SUCCESS && epoch!=0 && (!existing || count));
    if (existing) {
        for (index=0;index<count;++index)
            wprintf(L"WORKER sequence=%lu task=%lu kind=%lu state=%lu image=%ls\n",
                (unsigned long)entries[index].sequence,(unsigned long)entries[index].task,
                (unsigned long)entries[index].kind,(unsigned long)entries[index].state,
                entries[index].image);
        if (terminate) {
            RpcTryExcept {
                error=Client_TerminateWorker(binding,self,APP_PROTOCOL_VERSION,(unsigned char *)version,
                    epoch,entries[0].sequence);
            }
            RpcExcept(1) { error=RpcExceptionCode(); }
            RpcEndExcept
            CHECK(error==ERROR_SUCCESS);
            puts("PASS: authenticated DTASKMGR RPC accepted selected live worker termination");
        }
        MIDL_user_free(entries); RpcBindingFree(&binding); CloseHandle(self);
        puts("PASS: authenticated DTASKMGR RPC sees at least one live worker");
        return 0;
    }
    CHECK(count==0 && entries==NULL);
    valid_epoch=epoch;
    RpcTryExcept {
        error=Client_TaskSnapshot(binding,self,APP_PROTOCOL_VERSION+1,(unsigned char *)version,
            &epoch,&count,&entries);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    CHECK(error==ERROR_REVISION_MISMATCH);
    RpcTryExcept {
        error=Client_TerminateWorker(binding,self,APP_PROTOCOL_VERSION,(unsigned char *)version,valid_epoch,1);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    CHECK(error==ERROR_NOT_FOUND);
    RpcBindingFree(&binding); CloseHandle(self);
    CHECK(TerminateProcess(broker.hProcess,0));
    CHECK(WaitForSingleObject(broker.hProcess,5000)==WAIT_OBJECT_0);
    CloseHandle(broker.hThread);CloseHandle(broker.hProcess);
    puts("PASS: authenticated DTASKMGR RPC sees empty broker, rejects version and cannot terminate absent worker");
    return 0;
}
