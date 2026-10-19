/* Native USER capture observation. Never linked into the runtime. The
 * receiver observes capture identity after the sender protects its own page.
 * Output writes are attempted only on a distinct, currently writable capture;
 * faults remain a failed diagnostic, never a product exception workaround. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>

static HWND window;
static HANDLE acknowledgement;
static volatile UINT current_message;
static BYTE *page;
static void *received, *received_data;
static DWORD protection, data_protection, receive_flags;
static volatile LONG errors, continued;
static LONG capture_write;

static void check(BOOL condition){if(!condition)InterlockedIncrement(&errors);}

static DWORD query_protection(const void *pointer)
{
    MEMORY_BASIC_INFORMATION info={0};
    return VirtualQuery(pointer,&info,sizeof(info))==sizeof(info) &&
        info.State==MEM_COMMIT ? info.Protect : 0;
}

static LRESULT CALLBACK receive(HWND target,UINT message,WPARAM wp,LPARAM lp)
{
    if(message==WM_APP+1){check(continued==1);return 0x447a0098;}
    if(message==WM_CLOSE){check(DestroyWindow(target));return 0;}
    if(message==WM_DESTROY){PostQuitMessage(0);return 0;}
    if(current_message && message==current_message){
        received=(void *)lp;received_data=NULL;
        receive_flags=InSendMessageEx(NULL);
        check((receive_flags & ISMEX_SEND)!=0);
        if(message==WM_GETTEXT){
            check(wp==64);memcpy((void *)lp,"before",7);
        }else if(message==WM_GETMINMAXINFO){
            ((MINMAXINFO *)lp)->ptReserved.x=0x1234;
        }else if(message==WM_WINDOWPOSCHANGING){
            ((WINDOWPOS *)lp)->x=23;
        }else{
            const COPYDATASTRUCT *copy=(const COPYDATASTRUCT *)lp;
            check(copy->dwData==0x456789ab && copy->cbData==8 &&
                !memcmp(copy->lpData,"M4copy!!",8));
            received_data=copy->lpData;
        }
        check(ReplyMessage(6));
        check(WaitForSingleObject(acknowledgement,5000)==WAIT_OBJECT_0);
        /* Observe without dereferencing first: an alias may be NOACCESS. */
        protection=query_protection(received);
        if(received_data)data_protection=query_protection(received_data);
        capture_write=0;
        if(message!=WM_COPYDATA && received!=page &&
            !(protection & PAGE_GUARD) &&
            ((protection & 0xff)==PAGE_READWRITE ||
             (protection & 0xff)==PAGE_WRITECOPY ||
             (protection & 0xff)==PAGE_EXECUTE_READWRITE ||
             (protection & 0xff)==PAGE_EXECUTE_WRITECOPY)){
            __try {
                if(message==WM_GETTEXT)memcpy(received,"after!",7);
                else if(message==WM_GETMINMAXINFO)((MINMAXINFO *)received)->ptReserved.x=0x4321;
                else ((WINDOWPOS *)received)->x=42;
                capture_write=1;
            } __except(EXCEPTION_EXECUTE_HANDLER){check(FALSE);}
        }
        InterlockedExchange(&continued,1);
        return 0x447a0099;
    }
    return DefWindowProcA(target,message,wp,lp);
}

static DWORD WINAPI send_cases(PVOID ignored)
{
    const UINT messages[]={WM_GETTEXT,WM_GETMINMAXINFO,WM_WINDOWPOSCHANGING,WM_COPYDATA};
    unsigned i;DWORD old,unused;DWORD_PTR result=0;SIZE_T read;
    BYTE byte;
    (void)ignored;
    for(i=0;i<sizeof(messages)/sizeof(messages[0]);++i){
        WPARAM wp=messages[i]==WM_GETTEXT ? 64 : 0;
        LRESULT sent;DWORD send_error;DWORD_PTR reply_result;int output_at_reply=-1;
        check(ResetEvent(acknowledgement));
        page=VirtualAlloc(NULL,4096,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
        if(!page){check(FALSE);break;}
        if(messages[i]==WM_COPYDATA){
            COPYDATASTRUCT *copy=(COPYDATASTRUCT *)page;
            copy->dwData=0x456789ab;copy->cbData=8;copy->lpData=page+64;
            memcpy(copy->lpData,"M4copy!!",8);
        }else if(messages[i]==WM_WINDOWPOSCHANGING){
            WINDOWPOS *position=(WINDOWPOS *)page;
            position->hwnd=window;position->hwndInsertAfter=HWND_TOP;
            position->cx=100;position->cy=60;
        }
        received=NULL;received_data=NULL;protection=data_protection=0;
        InterlockedExchange(&continued,0);current_message=messages[i];
        SetLastError(ERROR_SUCCESS);
        sent=SendMessageTimeoutA(window,current_message,wp,(LPARAM)page,
            SMTO_ABORTIFHUNG|SMTO_ERRORONEXIT,5000,&result);
        send_error=GetLastError();reply_result=result;
        check(sent!=0 && result==6);
        if(sent){
            if(current_message==WM_GETTEXT)output_at_reply=!memcmp(page,"before",7);
            else if(current_message==WM_GETMINMAXINFO)output_at_reply=((MINMAXINFO *)page)->ptReserved.x==0x1234;
            else if(current_message==WM_WINDOWPOSCHANGING)output_at_reply=((WINDOWPOS *)page)->x==23;
        }
        check(VirtualProtect(page,4096,PAGE_NOACCESS,&old));
        read=0;check(!ReadProcessMemory(GetCurrentProcess(),page,&byte,1,&read) && !read);
        check(SetEvent(acknowledgement));
        /* The receiver performs no USER operation between acknowledgement
         * and return. This second cross-thread send brackets full return. */
        if(!SendMessageTimeoutA(window,WM_APP+1,0,0,SMTO_ERRORONEXIT,5000,&result) ||
            result!=0x447a0098 || continued!=1){check(FALSE);break;}
        printf("message=%04X bits=%u alias=%u data_alias=%u protection=%08lX data_protection=%08lX sender=%llX receiver=%llX data=%llX status=%lld error=%lu reply=%llX output_at_reply=%d capture_write=%ld full_return=1\n",
            current_message,(unsigned)(sizeof(void *)*8),received==page,
            received_data==page+64,protection,data_protection,
            (unsigned long long)(ULONG_PTR)page,(unsigned long long)(ULONG_PTR)received,
            (unsigned long long)(ULONG_PTR)received_data,(long long)sent,send_error,
            (unsigned long long)reply_result,output_at_reply,capture_write);
        check(VirtualProtect(page,4096,PAGE_READWRITE,&unused));
        check(VirtualFree(page,0,MEM_RELEASE));page=NULL;
        if(errors)break;
    }
    check(PostMessageA(window,WM_CLOSE,0,0));
    return errors ? 1 : 0;
}

int main(void)
{
    WNDCLASSA cls={0};MSG msg;HANDLE sender;BOOL got;DWORD code=1;
    cls.hInstance=GetModuleHandleA(NULL);cls.lpfnWndProc=receive;
    cls.lpszClassName="T447_CAPTURE_NATIVE_CONTROL";
    acknowledgement=CreateEventA(NULL,TRUE,FALSE,NULL);
    if(!acknowledgement || !RegisterClassA(&cls))return 2;
    window=CreateWindowA(cls.lpszClassName,"Capture control",WS_POPUP,
        0,0,100,60,NULL,NULL,cls.hInstance,NULL);
    if(!window)return 3;
    sender=CreateThread(NULL,0,send_cases,NULL,0,NULL);
    if(!sender)return 4;
    while((got=GetMessageA(&msg,NULL,0,0))>0)DispatchMessageA(&msg);
    check(got==0 && !IsWindow(window));
    check(WaitForSingleObject(sender,10000)==WAIT_OBJECT_0 &&
        GetExitCodeThread(sender,&code) && code==0);
    check(CloseHandle(sender));check(CloseHandle(acknowledgement));
    check(UnregisterClassA(cls.lpszClassName,cls.hInstance));
    printf("errors=%ld\n",errors);
    return errors ? 1 : 0;
}
