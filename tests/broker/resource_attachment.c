/* Test-only modern transport. No BaseSrv/VDM policy is implemented here. */
#include <windows.h>
#include <rpc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "resource_attachment.h"
#include "rpc_security.h"

void *__RPC_USER midl_user_allocate(size_t size) { return malloc(size); }
void __RPC_USER midl_user_free(void *value) { free(value); }

#ifdef RESOURCE_SERVER
static RPC_BINDING_HANDLE downstream;
static broker_rpc_scope serverScope;
static RPC_STATUS RPC_ENTRY authorize(RPC_IF_HANDLE interface_id, void *context)
{
    ULONG level=0, service=0;
    (void)interface_id;
    (void)RpcBindingInqAuthClientW(context,NULL,NULL,&level,&service,NULL);
    printf("AUTH level=%lu service=%lu\n",level,service); fflush(stdout);
    return broker_rpc_authorize(&serverScope,context);
}

error_status_t Server_Transfer(handle_t binding, HANDLE input, HANDLE event, HANDLE *output)
{
    DWORD written;
    LARGE_INTEGER zero = {0}, position;
    error_status_t result = ERROR_INVALID_DATA;
    (void)binding;
    *output = NULL;
    if (downstream) {
        /* Synchronous third-process delivery; no file reopen or byte pump. */
        RpcTryExcept { result = Client_Transfer(downstream, input, event, output); }
        RpcExcept(1) { result = RpcExceptionCode(); }
        RpcEndExcept
        (void)RpcMgmtStopServerListening(NULL);
        return result;
    }
    if (SetFilePointerEx(input, zero, &position, FILE_CURRENT) && position.QuadPart == 1) {
        if (!WriteFile(input, "B", 1, &written, NULL)) result = GetLastError();
        else if (written == 1 &&
            DuplicateHandle(GetCurrentProcess(), input, GetCurrentProcess(), output,
                0, FALSE, DUPLICATE_SAME_ACCESS) && SetEvent(event)) result = ERROR_SUCCESS;
    }
    if (result != ERROR_SUCCESS && *output) { CloseHandle(*output); *output = NULL; }
    (void)RpcMgmtStopServerListening(NULL);
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
    puts("READY"); fflush(stdout);
    status = RpcServerListen(1, 2, FALSE);
    (void)RpcServerUnregisterIf(NULL, NULL, TRUE);
    if (downstream) RpcBindingFree(&downstream);
    return status == RPC_S_OK ? 0 : 3;
}
#else
int main(int argc, char **argv)
{
    RPC_CSTR text = NULL;
    RPC_BINDING_HANDLE binding = NULL;
    HANDLE input, event, output = NULL;
    LARGE_INTEGER zero = {0}, position;
    DWORD count;
    char data[2];
    error_status_t status = RPC_S_CALL_FAILED;
    int readonly, unavailable, denied;
    if (argc != 3 && argc != 4) return 1;
    readonly = argc == 4 && strcmp(argv[3], "readonly") == 0;
    unavailable = argc == 4 && strcmp(argv[3], "unavailable") == 0;
    denied = argc == 4 && strcmp(argv[3], "denied") == 0;
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
    RpcTryExcept { status = Client_Transfer(binding, input, event, &output); }
    RpcExcept(1) { status = RpcExceptionCode(); }
    RpcEndExcept
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
