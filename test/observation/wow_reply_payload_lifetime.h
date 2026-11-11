/* Independently authored test protocol. Never linked into a product image.
 * The sender protects only its own VirtualAlloc input after an early reply.
 * Receivers consume input before replying, then wait without touching it. */
#ifndef T447_REPLY_PAYLOAD_LIFETIME_H
#define T447_REPLY_PAYLOAD_LIFETIME_H
#include <windows.h>
#include <string.h>

#define REPLY_PAYLOAD_DATA 0x456789acUL
#define REPLY_PAYLOAD_VALUE 0x447a0051UL
#define TIMEOUT_PAYLOAD_DATA 0x456789adUL
C_ASSERT(sizeof(void *) == 4);
C_ASSERT(sizeof(COPYDATASTRUCT) <= 64);

typedef struct reply_payload_lifetime {
    BYTE *page;
    SIZE_T bytes;
    HANDLE acknowledgement;
    volatile LONG errors, accepted, sender_returned, protected_input;
    volatile LONG unreadable, continued;
    DWORD reply_flags, sender_thread, receiver_thread, result, old_protection;
    DWORD receiver_alias, sender_window, receiver_window;
    DWORD receiver_header, receiver_payload;
    BOOL timeout;
    DWORD send_status, send_error, after_flags, receiver_alive;
} reply_payload_lifetime;

static BOOL reply_payload_create(reply_payload_lifetime *p)
{
    SYSTEM_INFO info;
    COPYDATASTRUCT *copy;
    ZeroMemory(p,sizeof(*p));
    p->receiver_alias=MAXDWORD; /* Unobserved for the actual WOW converter. */
    GetSystemInfo(&info); p->bytes=info.dwPageSize;
    p->acknowledgement=CreateEventA(NULL,TRUE,FALSE,NULL);
    if (!p->acknowledgement) return FALSE;
    p->page=VirtualAlloc(NULL,p->bytes,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
    if (!p->page) { CloseHandle(p->acknowledgement); p->acknowledgement=NULL; return FALSE; }
    copy=(COPYDATASTRUCT *)p->page;
    copy->dwData=REPLY_PAYLOAD_DATA; copy->cbData=8; copy->lpData=p->page+64;
    memcpy(copy->lpData,"M4copy!!",8);
    return TRUE;
}

static BOOL reply_payload_send(reply_payload_lifetime *p, HWND receiver, HWND sender)
{
    DWORD_PTR result=0;
    SIZE_T read=0;
    BYTE byte=0;
    LRESULT sent;
    p->sender_thread=GetCurrentThreadId();
    p->receiver_thread=GetWindowThreadProcessId(receiver,NULL);
    p->sender_window=(DWORD)(ULONG_PTR)sender;
    p->receiver_window=(DWORD)(ULONG_PTR)receiver;
    SetLastError(ERROR_SUCCESS);
    sent=SendMessageTimeoutA(receiver,WM_COPYDATA,(WPARAM)sender,(LPARAM)p->page,
        p->timeout ? SMTO_BLOCK|SMTO_ERRORONEXIT : SMTO_ABORTIFHUNG|SMTO_ERRORONEXIT,
        p->timeout ? 1000 : 5000,&result);
    p->send_error=GetLastError(); p->send_status=(DWORD)sent;
    p->result=(DWORD)result;
    InterlockedExchange(&p->sender_returned,1); /* API returned, not API success. */
    p->receiver_alive=IsWindow(receiver);
    /* A generic error, unentered receiver or dead window is not our expected
     * in-flight timeout. Preserve that failure rather than claiming success. */
    if (p->timeout ? (sent || p->send_error != ERROR_TIMEOUT || !p->accepted || !p->receiver_alive) :
        (!sent || result != REPLY_PAYLOAD_VALUE)) InterlockedIncrement(&p->errors);
    if (p->errors == 0) {
        if (!VirtualProtect(p->page,p->bytes,PAGE_NOACCESS,&p->old_protection))
            InterlockedIncrement(&p->errors);
        else {
            InterlockedExchange(&p->protected_input,1);
            if (!ReadProcessMemory(GetCurrentProcess(),p->page,&byte,1,&read) && !read)
                InterlockedExchange(&p->unreadable,1);
            else InterlockedIncrement(&p->errors);
        }
    }
    if (!SetEvent(p->acknowledgement)) InterlockedIncrement(&p->errors);
    return p->errors == 0;
}

static BOOL reply_payload_wait(reply_payload_lifetime *p)
{
    p->reply_flags=InSendMessageEx(NULL);
    if ((!p->timeout && !(p->reply_flags & ISMEX_REPLIED)) ||
        WaitForSingleObject(p->acknowledgement,5000) != WAIT_OBJECT_0 ||
        !p->sender_returned || !p->protected_input || !p->unreadable)
        InterlockedIncrement(&p->errors);
    p->after_flags=InSendMessageEx(NULL);
    InterlockedExchange(&p->continued,1);
    return p->errors == 0;
}

/* Caller has observed the receiver's full window-procedure return, not just
 * the early SendMessage result. Only now restore/free the owned input page. */
static BOOL reply_payload_finish(reply_payload_lifetime *p, const char *path)
{
    DWORD report[26], written=0, ignored;
    DWORD report_bytes=p->timeout ? sizeof(report) : 21u*sizeof(DWORD);
    HANDLE output;
    if (!p->accepted || !p->continued || !p->sender_returned ||
        !p->protected_input || !p->unreadable ||
        (p->timeout ? (p->send_status || p->send_error != ERROR_TIMEOUT || !p->receiver_alive) :
            p->result != REPLY_PAYLOAD_VALUE) ||
        !p->sender_thread || !p->receiver_thread || p->sender_thread == p->receiver_thread)
        InterlockedIncrement(&p->errors);
    report[0]=p->timeout ? 0x504f4d54 : 0x594c5052; /* TMOP / RPLY */
    report[2]=p->sender_thread; report[3]=p->receiver_thread;
    report[4]=(DWORD)p->accepted; report[5]=p->reply_flags;
    report[6]=(DWORD)p->sender_returned; report[7]=p->result;
    report[8]=(DWORD)p->protected_input; report[9]=(DWORD)p->unreadable;
    report[10]=(DWORD)p->continued; report[11]=1; /* Full-return caller boundary. */
    report[12]=(DWORD)(ULONG_PTR)p->page; report[13]=report[12]+64;
    report[14]=p->receiver_alias; report[15]=p->old_protection;
    report[16]=(DWORD)p->bytes;
    report[17]=p->sender_window; report[18]=p->receiver_window;
    report[19]=p->receiver_header; report[20]=p->receiver_payload;
    if (p->timeout) {
        report[21]=1; report[22]=p->send_status; report[23]=p->send_error;
        report[24]=p->after_flags; report[25]=p->receiver_alive;
    }
    if (p->protected_input && !VirtualProtect(p->page,p->bytes,PAGE_READWRITE,&ignored))
        InterlockedIncrement(&p->errors);
    if (p->page && !VirtualFree(p->page,0,MEM_RELEASE)) InterlockedIncrement(&p->errors);
    p->page=NULL;
    if (p->acknowledgement && !CloseHandle(p->acknowledgement)) InterlockedIncrement(&p->errors);
    p->acknowledgement=NULL;
    report[1]=(DWORD)p->errors;
    output=CreateFileA(path,GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
    if (output == INVALID_HANDLE_VALUE) return FALSE;
    if (!WriteFile(output,report,report_bytes,&written,NULL) || written != report_bytes)
        InterlockedIncrement(&p->errors);
    if (!CloseHandle(output)) InterlockedIncrement(&p->errors);
    return p->errors == 0;
}
#endif
