/* Test-only modern transport. No BaseSrv/VDM policy is implemented here. */
#include <windows.h>
#include <rpc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "resource_attachment.h"
#include "rpc_security.h"
#include "vdm_receipt.h"

void *__RPC_USER midl_user_allocate(size_t size) { return malloc(size); }
void __RPC_USER midl_user_free(void *value) { free(value); }
static DWORD revoke_remote(handle_t binding,HANDLE process,ULONG fileId,ULONG eventId)
{
    DWORD result=ERROR_SUCCESS;
    if (!fileId && !eventId) return result;
    if (!fileId || !eventId) return ERROR_INVALID_DATA;
    RpcTryExcept {
        if (Client_Revoke(binding,process,2,fileId)!=ERROR_ACCESS_DENIED ||
            Client_Revoke(binding,process,1,fileId)!=ERROR_SUCCESS ||
            Client_Revoke(binding,process,1,fileId)!=ERROR_SUCCESS ||
            Client_Revoke(binding,process,1,eventId)!=ERROR_SUCCESS) result=ERROR_INVALID_DATA;
    }
    RpcExcept(1) { result=RpcExceptionCode(); }
    RpcEndExcept
    return result;
}

#ifdef RESOURCE_SERVER
DWORD test_registration_start(void);
DWORD test_registration_retain(HANDLE,DWORD);
DWORD test_registration_finish(void);
static RPC_BINDING_HANDLE downstream;
static broker_rpc_scope serverScope;
static broker_vdm_receipts receipts;
static DWORD registeredPeer;
static RPC_STATUS RPC_ENTRY authorize(RPC_IF_HANDLE interface_id, void *context)
{
    ULONG level=0, service=0;
    (void)interface_id;
    (void)RpcBindingInqAuthClientW(context,NULL,NULL,&level,&service,NULL);
    printf("AUTH level=%lu service=%lu\n",level,service); fflush(stdout);
    return broker_rpc_authorize(&serverScope,context);
}

error_status_t Server_Transfer(handle_t binding, HANDLE process, HANDLE input, HANDLE event, HANDLE *output,
    ULONG *fileReceipt, ULONG *eventReceipt)
{
    DWORD written, peerPid;
    LARGE_INTEGER zero = {0}, position;
    error_status_t result = ERROR_INVALID_DATA;
    *output = NULL;
    *fileReceipt=*eventReceipt=0;
    result=broker_rpc_peer_process(&serverScope,binding,process,&peerPid);
    printf("PEER status=%lu matched=%d\n",result,peerPid!=0); fflush(stdout);
    if (result!=RPC_S_OK) return result;
    result=test_registration_retain(process,peerPid);
    if (result!=ERROR_SUCCESS) return result;
    registeredPeer=peerPid;
    {
        uint32_t fileId=0,eventId=0;
        HANDLE rejected=(HANDLE)1;
        result=broker_vdm_receipt_accept(&receipts,BROKER_VDM_STDOUT,input,&fileId);
        if (!result) result=broker_vdm_receipt_accept(&receipts,BROKER_VDM_PARENT_WAIT,event,&eventId);
        if (result) { if (fileId) broker_vdm_receipt_revoke(&receipts,1,fileId); return result; }
        if (broker_vdm_receipt_resolve(&receipts,2,fileId,BROKER_VDM_STDOUT,&rejected)!=ERROR_ACCESS_DENIED || rejected)
            return ERROR_INVALID_DATA;
        if (broker_vdm_receipt_resolve(&receipts,1,eventId,BROKER_VDM_STDOUT,&rejected)!=ERROR_ACCESS_DENIED || rejected ||
            broker_vdm_receipt_resolve(&receipts,1,fileId,BROKER_VDM_PARENT_WAIT,&rejected)!=ERROR_ACCESS_DENIED || rejected)
            return ERROR_INVALID_DATA;
        if (broker_vdm_receipt_resolve(&receipts,1,fileId,BROKER_VDM_STDERR,&input) ||
            broker_vdm_receipt_resolve(&receipts,1,eventId,BROKER_VDM_PARENT_WAIT,&event)) return ERROR_INVALID_DATA;
        puts("RECEIPT wrong-role=denied stream-alias=accepted");
        *fileReceipt=fileId; *eventReceipt=eventId;
        puts("RECEIPT retained=1 wrong-generation=denied"); fflush(stdout);
    }
    if (downstream) {
        ULONG remoteFile=0,remoteEvent=0;
        DWORD revoked;
        HANDLE self=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE,FALSE,GetCurrentProcessId());
        if (!self) return GetLastError();
        /* Synchronous third-process delivery; no file reopen or byte pump. */
        RpcTryExcept { result = Client_Transfer(downstream, self, input, event, output,&remoteFile,&remoteEvent); }
        RpcExcept(1) { result = RpcExceptionCode(); }
        RpcEndExcept
        revoked=revoke_remote(downstream,self,remoteFile,remoteEvent);
        CloseHandle(self);
        if (revoked) return revoked;
        return result;
    }
    if (SetFilePointerEx(input, zero, &position, FILE_CURRENT) && position.QuadPart == 1) {
        if (!WriteFile(input, "B", 1, &written, NULL)) result = GetLastError();
        else if (written == 1 &&
            DuplicateHandle(GetCurrentProcess(), input, GetCurrentProcess(), output,
                0, FALSE, DUPLICATE_SAME_ACCESS) && SetEvent(event)) result = ERROR_SUCCESS;
    }
    if (result != ERROR_SUCCESS && *output) { CloseHandle(*output); *output = NULL; }
    return result;
}

error_status_t Server_Revoke(handle_t binding,HANDLE process,ULONG generation,ULONG receipt)
{
    DWORD pid=0;
    DWORD result=broker_rpc_peer_process(&serverScope,binding,process,&pid);
    if (result || pid!=registeredPeer) return ERROR_ACCESS_DENIED;
    result=broker_vdm_receipt_revoke(&receipts,generation,receipt);
    printf("REVOKE generation=%lu id=%lu status=%lu\n",generation,receipt,result); fflush(stdout);
    if (!result && !receipts.entries) (void)RpcMgmtStopServerListening(NULL);
    return result;
}

int main(int argc, char **argv)
{
    RPC_STATUS status;
    RPC_CSTR text = NULL;
    if (argc != 2 && argc != 3) return 1;
    if (!broker_rpc_capture_scope(&serverScope)) return 5;
    if (argc==3 && !strcmp(argv[2],"deny-scope")) serverScope.logon.LowPart^=1;
    else if (argc==3 && !strcmp(argv[2],"deny-session")) serverScope.session^=1;
    else if (argc == 3) {
        if (RpcStringBindingComposeA(NULL, (RPC_CSTR)"ncalrpc", NULL,
                (RPC_CSTR)argv[2], NULL, &text) ||
            RpcBindingFromStringBindingA(text, &downstream) ||
            RpcBindingSetAuthInfoA(downstream, NULL, RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
                RPC_C_AUTHN_WINNT, NULL, RPC_C_AUTHZ_NONE)) return 4;
        RpcStringFreeA(&text);
    }
    status = RpcServerUseProtseqEpA((RPC_CSTR)"ncalrpc", 2, (RPC_CSTR)argv[1], NULL);
    if (status == RPC_S_OK) status = RpcServerRegisterAuthInfoA(NULL, RPC_C_AUTHN_WINNT, NULL, NULL);
    if (status == RPC_S_OK) status = RpcServerRegisterIfEx(Server_resource_attachment_v1_0_s_ifspec,
        NULL, NULL, RPC_IF_ALLOW_LOCAL_ONLY | RPC_IF_ALLOW_SECURE_ONLY, 2, authorize);
    if (status != RPC_S_OK) { fprintf(stderr, "server setup=%lu\n", status); return 2; }
    if (test_registration_start()!=0) return 6;
    if (broker_vdm_receipts_initialize(&receipts,1)!=ERROR_SUCCESS) return 8;
    {
        uint32_t invalid=123;
        if (broker_vdm_receipt_accept(&receipts,BROKER_VDM_STDOUT,NULL,&invalid)!=ERROR_INVALID_HANDLE || invalid)
            return 10;
        if (broker_vdm_receipt_revoke(&receipts,1,1)!=ERROR_NOT_FOUND) return 11;
        receipts.issued=UINT32_MAX;
        if (broker_vdm_receipt_accept(&receipts,BROKER_VDM_PARENT_WAIT,NULL,&invalid)!=ERROR_ARITHMETIC_OVERFLOW || invalid)
            return 12;
        receipts.issued=0;
    }
    {
        uint32_t invalid=123;
        if (broker_vdm_receipt_accept(&receipts,BROKER_VDM_STDOUT,NULL,&invalid)!=ERROR_INVALID_HANDLE || invalid)
            return 10;
        if (broker_vdm_receipt_revoke(&receipts,1,1)!=ERROR_NOT_FOUND) return 11;
        receipts.issued=UINT32_MAX;
        if (broker_vdm_receipt_accept(&receipts,BROKER_VDM_PARENT_WAIT,NULL,&invalid)!=ERROR_ARITHMETIC_OVERFLOW || invalid)
            return 12;
        receipts.issued=0;
    }
    puts("READY"); fflush(stdout);
    status = RpcServerListen(1, 2, FALSE);
    (void)RpcServerUnregisterIf(NULL, NULL, TRUE);
    if (test_registration_finish()!=0) return 7;
    if (receipts.issued) {
        HANDLE released=(HANDLE)1;
        if (broker_vdm_receipt_revoke(&receipts,1,1) || broker_vdm_receipt_revoke(&receipts,1,1) ||
            broker_vdm_receipt_resolve(&receipts,1,1,BROKER_VDM_STDOUT,&released)!=ERROR_NOT_FOUND || released) return 9;
        broker_vdm_receipts_drain(&receipts);
        puts("RECEIPT revoked=1 repeat-safe=1 drained=1"); fflush(stdout);
    }
    if (downstream) RpcBindingFree(&downstream);
    return status == RPC_S_OK ? 0 : 3;
}
#else
int main(int argc, char **argv)
{
    RPC_CSTR text = NULL;
    RPC_BINDING_HANDLE binding = NULL;
    HANDLE input, event, process, output = NULL;
    LARGE_INTEGER zero = {0}, position;
    DWORD count;
    char data[2];
    error_status_t status = RPC_S_CALL_FAILED;
    ULONG fileReceipt=0,eventReceipt=0;
    DWORD revoked;
    int readonly, unavailable, denied;
    if (argc != 3 && argc != 4 && argc != 5) return 1;
    readonly = argc == 4 && strcmp(argv[3], "readonly") == 0;
    unavailable = argc == 4 && strcmp(argv[3], "unavailable") == 0;
    denied = (argc == 4 && strcmp(argv[3], "denied") == 0) || argc==5;
    process=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE,FALSE,
        argc==5?(DWORD)strtoul(argv[4],NULL,10):GetCurrentProcessId());
    if (!process) return 10;
    input = CreateFileA(argv[2], GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    event = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (input == INVALID_HANDLE_VALUE || !event ||
        !WriteFile(input, "A", 1, &count, NULL) || count != 1) return 2;
    if (readonly) {
        LARGE_INTEGER one = {1};
        CloseHandle(input);
        input = CreateFileA(argv[2], GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (input == INVALID_HANDLE_VALUE || !SetFilePointerEx(input, one, NULL, FILE_BEGIN)) return 2;
    }
    if (RpcStringBindingComposeA(NULL, (RPC_CSTR)"ncalrpc", NULL, (RPC_CSTR)argv[1], NULL, &text) ||
        RpcBindingFromStringBindingA(text, &binding) ||
        RpcBindingSetAuthInfoA(binding, NULL, argc==4 && !strcmp(argv[3],"low-auth") ? RPC_C_AUTHN_LEVEL_PKT_INTEGRITY : RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
            RPC_C_AUTHN_WINNT, NULL, RPC_C_AUTHZ_NONE)) return 3;
    RpcTryExcept { status = Client_Transfer(binding, process, input, event, &output,&fileReceipt,&eventReceipt); }
    RpcExcept(1) { status = RpcExceptionCode(); }
    RpcEndExcept
    revoked=revoke_remote(binding,process,fileReceipt,eventReceipt);
    CloseHandle(process);
    if (revoked) { fprintf(stderr,"remote revoke=%lu\n",revoked); return 13; }
    if (denied) {
        if (status!=RPC_S_ACCESS_DENIED || output!=NULL ||
            WaitForSingleObject(event,0)!=WAIT_TIMEOUT || GetFileSize(input,NULL)!=1) {
            fprintf(stderr,"denial result=%lu output=%d\n",status,output!=NULL); return 9;
        }
        CloseHandle(input); CloseHandle(event);
        RpcBindingFree(&binding); RpcStringFreeA(&text);
        puts("PASS: rejected scope or authentication level before resource mutation");
        return 0;
    }
    if (unavailable) {
        if (status != RPC_S_SERVER_UNAVAILABLE || output != NULL ||
            WaitForSingleObject(event, 0) != WAIT_TIMEOUT ||
            !SetFilePointerEx(input, zero, &position, FILE_CURRENT) ||
            position.QuadPart != 1 || GetFileSize(input, NULL) != 1) {
            fprintf(stderr, "unavailable=%lu\n", status); return 8;
        }
        CloseHandle(input); CloseHandle(event);
        RpcBindingFree(&binding); RpcStringFreeA(&text);
        puts("PASS: unavailable target fails synchronously without mutation or signaling");
        return 0;
    }
    if (readonly) {
        if (status != ERROR_ACCESS_DENIED || output != NULL ||
            WaitForSingleObject(event, 0) != WAIT_TIMEOUT ||
            GetFileSize(input, NULL) != 1) { fprintf(stderr, "readonly=%lu\n", status); return 7; }
        CloseHandle(input); CloseHandle(event);
        RpcBindingFree(&binding); RpcStringFreeA(&text);
        puts("PASS: read-only resource attachment denies write without signaling or mutation");
        return 0;
    }
    if (status || !output) { fprintf(stderr, "transfer=%lu\n", status); return 4; }
    if (WaitForSingleObject(event, 0) != WAIT_OBJECT_0 ||
        !SetFilePointerEx(input, zero, &position, FILE_CURRENT) || position.QuadPart != 2)
        return 5;
    CloseHandle(input);
    if (!SetFilePointerEx(output, zero, NULL, FILE_BEGIN) ||
        !ReadFile(output, data, sizeof(data), &count, NULL) || count != 2 ||
        memcmp(data, "AB", 2) != 0) return 6;
    CloseHandle(output); CloseHandle(event);
    RpcBindingFree(&binding); RpcStringFreeA(&text);
    puts("PASS: authenticated local RPC file/event attachments, shared position, returned ownership");
    return 0;
}
#endif
