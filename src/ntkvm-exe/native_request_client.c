#include "native_request_client.h"
#include "native_request_protocol.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"
#include <stdio.h>
DWORD run16_native_request_submit_receipt(HANDLE root,HANDLE root_capability,const run16_native_start *start,HANDLE *target,HANDLE *receipt)
{
    static LONG serial;
    WCHAR name[96];HANDLE server=INVALID_HANDLE_VALUE,client=INVALID_HANDLE_VALUE,event=NULL;
    native_request_header header={NATIVE_REQUEST_VERSION,0};
    native_request_reply reply={0};
    run16_native_start local=*start;
    BYTE *payload=NULL;DWORD error,bytes;
    *target=NULL;*receipt=NULL;local.capabilities[0]=local.capabilities[1]=NULL;
    error=run16_native_launch_pack(&local,&payload,&bytes);if(error)return error;
    header.bytes=bytes;
    event=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!event) { error=GetLastError();goto done; }
    swprintf_s(name,96,L"\\\\.\\pipe\\run16-request-%lu-%lu",GetCurrentProcessId(),(DWORD)InterlockedIncrement(&serial));
    server=CreateNamedPipeW(name,PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED|FILE_FLAG_FIRST_PIPE_INSTANCE,
        PIPE_TYPE_BYTE|PIPE_WAIT|PIPE_REJECT_REMOTE_CLIENTS,1,65536,65536,0,NULL);
    if(server==INVALID_HANDLE_VALUE) { error=GetLastError();goto done; }
    client=CreateFileW(name,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED|SECURITY_SQOS_PRESENT|SECURITY_IDENTIFICATION,NULL);
    if(client==INVALID_HANDLE_VALUE) { error=GetLastError();goto done; }
    {
        OVERLAPPED io={0};DWORD ignored;io.hEvent=event;
        if(!ConnectNamedPipe(server,&io) && GetLastError()!=ERROR_PIPE_CONNECTED) {
            error=GetLastError();CancelIoEx(server,&io);GetOverlappedResult(server,&io,&ignored,TRUE);goto done;
        }
    }
    error=OpenNtBaseClientSubmitFrontendChannel(root_capability,server);
    CloseHandle(server);server=INVALID_HANDLE_VALUE;
    if(!error)error=frontend_request_transfer(client,root,NULL,event,TRUE,&header,sizeof(header));
    if(!error)error=frontend_request_transfer(client,root,NULL,event,TRUE,payload,header.bytes);
    if(!error)error=frontend_request_transfer(client,root,NULL,event,FALSE,&reply,sizeof(reply));
    if(!error) {
        if(reply.version!=NATIVE_REQUEST_VERSION || reply.target>(uint64_t)(ULONG_PTR)-1 ||
            reply.receipt>(uint64_t)(ULONG_PTR)-1 ||
            (!reply.error && (!reply.target || !reply.receipt)) ||
            (reply.error && (reply.target || reply.receipt)))error=ERROR_INVALID_DATA;
        else if(reply.error)error=reply.error;
        else { *target=(HANDLE)(ULONG_PTR)reply.target;*receipt=(HANDLE)(ULONG_PTR)reply.receipt; }
    }
done:
    if(server!=INVALID_HANDLE_VALUE)CloseHandle(server);
    if(client!=INVALID_HANDLE_VALUE)CloseHandle(client);
    if(event)CloseHandle(event);
    HeapFree(GetProcessHeap(),0,payload);
    return error;
}
DWORD run16_native_request_submit(HANDLE root,HANDLE capability,const run16_native_start *start,HANDLE *target)
{
    HANDLE receipt=NULL;
    DWORD error=run16_native_request_submit_receipt(root,capability,start,target,&receipt);
    if(receipt)CloseHandle(receipt);
    return error;
}
