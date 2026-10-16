/* Selected native provider boundary only, not Win16 networking acceptance.
 * Socket traffic is confined to this process's IPv4 loopback pair. */
#define WIN32_LEAN_AND_MEAN
/* WOW deliberately selects the 1.1 blocking-hook contract being measured. */
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

static DWORD owner_thread, slot=TLS_OUT_OF_INDEXES;
static unsigned calls, wrong_owner, not_blocking;
static BOOL cancel_request;
static int cancel_result;
static INT_PTR WINAPI blocking_hook(void)
{
    ++calls;
    if(GetCurrentThreadId()!=owner_thread || TlsGetValue(slot)!=(PVOID)1)
        ++wrong_owner;
    if(!WSAIsBlocking())++not_blocking;
    if(cancel_request)cancel_result=WSACancelBlockingCall();
    return FALSE;
}

static DWORD WINAPI delayed_send(LPVOID context)
{
    SOCKET peer=*(SOCKET *)context;
    Sleep(200);
    return send(peer,"K",1,0)==1 ? 0 : 1;
}

int main(void)
{
    typedef FARPROC (WSAAPI *set_hook_fn)(FARPROC);
    WSADATA data;
    struct sockaddr_in address={0};
    SOCKET listener=INVALID_SOCKET,client=INVALID_SOCKET,peer=INVALID_SOCKET;
    HANDLE sender=NULL;
    HMODULE provider=NULL;
    set_hook_fn set_hook=NULL;
    FARPROC previous=NULL;
    BOOL started=FALSE;
    int size=sizeof(address),result=1,received,error;
    DWORD timeout=2000,thread_result;
    unsigned pass;
    char byte;
    owner_thread=GetCurrentThreadId();
    if(WSAStartup(MAKEWORD(1,1),&data))goto done;
    started=TRUE;
    provider=LoadLibraryW(L"wsock32.dll");
    if(!provider)goto done;
    set_hook=(set_hook_fn)(void *)GetProcAddress(provider,(LPCSTR)109);
    if(!set_hook || !GetProcAddress(provider,(LPCSTR)1000))goto done;
    slot=TlsAlloc();
    if(slot==TLS_OUT_OF_INDEXES || !TlsSetValue(slot,(PVOID)1))goto done;
    previous=set_hook(blocking_hook);
    if(!previous)goto done;
    if(set_hook(NULL)!=NULL || WSAGetLastError()!=WSAEFAULT)goto done;
    listener=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(listener==INVALID_SOCKET)goto done;
    address.sin_family=AF_INET;address.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
    if(bind(listener,(SOCKADDR *)&address,sizeof(address)) || listen(listener,1) ||
        getsockname(listener,(SOCKADDR *)&address,&size))goto done;
    client=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(client==INVALID_SOCKET || connect(client,(SOCKADDR *)&address,sizeof(address)))goto done;
    peer=accept(listener,NULL,NULL);
    if(peer==INVALID_SOCKET || setsockopt(client,SOL_SOCKET,SO_RCVTIMEO,
            (const char *)&timeout,sizeof(timeout)))goto done;
    for(pass=0;pass<2;++pass){
        unsigned before=calls;
        sender=CreateThread(NULL,0,delayed_send,&peer,0,NULL);
        if(!sender)goto done;
        received=recv(client,&byte,1,0);
        if(WaitForSingleObject(sender,4000)!=WAIT_OBJECT_0 ||
            !GetExitCodeThread(sender,&thread_result) || thread_result)goto done;
        CloseHandle(sender);sender=NULL;
        if(received!=1 || byte!='K' || calls==before || WSAIsBlocking() ||
            TlsGetValue(slot)!=(PVOID)1)goto done;
    }
    cancel_request=TRUE;cancel_result=SOCKET_ERROR;
    received=recv(client,&byte,1,0);error=WSAGetLastError();
    if(received!=SOCKET_ERROR || error!=WSAEINTR || cancel_result ||
        WSAIsBlocking() || wrong_owner || not_blocking)goto done;
    if(set_hook(previous)!=blocking_hook)goto done;
    previous=NULL;
    result=0;
done:
    fprintf(stderr,"WINSOCK_PROVIDER result=%d calls=%u foreign=%u outside_blocking=%u cancel=%d error=%d\n",
        result,calls,wrong_owner,not_blocking,cancel_result,WSAGetLastError());
    if(sender){WaitForSingleObject(sender,4000);CloseHandle(sender);}
    if(previous && set_hook)set_hook(previous);
    if(peer!=INVALID_SOCKET)closesocket(peer);
    if(client!=INVALID_SOCKET)closesocket(client);
    if(listener!=INVALID_SOCKET)closesocket(listener);
    if(slot!=TLS_OUT_OF_INDEXES)TlsFree(slot);
    if(provider)FreeLibrary(provider);
    if(started)WSACleanup();
    return result;
}
