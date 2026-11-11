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
static BOOL extended;
static UINT current_variant;
static LONG data_capture_write;
static void *source_data;
static HMENU menu;

static BOOL writable(DWORD value)
{
    DWORD access=value & 0xff;
    return !(value & PAGE_GUARD) && (access==PAGE_READWRITE ||
        access==PAGE_WRITECOPY || access==PAGE_EXECUTE_READWRITE ||
        access==PAGE_EXECUTE_WRITECOPY);
}

/* Fixed original output families, not an automatic payload-size inference. */
static void write_output(UINT message,void *payload,BOOL after)
{
    int value=after ? 42 : 23;
    if(message==WM_GETTEXT || message==WM_ASKCBFORMATNAME)
        memcpy(payload,after ? "after!" : "before",7);
    else if(message==WM_GETMINMAXINFO)
        ((MINMAXINFO *)payload)->ptReserved.x=after ? 0x4321 : 0x1234;
    else if(message==WM_WINDOWPOSCHANGING)((WINDOWPOS *)payload)->x=value;
    else if(message==WM_MEASUREITEM)((MEASUREITEMSTRUCT *)payload)->itemWidth=(UINT)value;
    else if(message==WM_NCCALCSIZE)((RECT *)payload)->left=value;
    else if(message==WM_MDIGETACTIVE)*(BOOL *)payload=after ? FALSE : TRUE;
    else if(message==WM_NEXTMENU){
        ((MDINEXTMENU *)payload)->hmenuNext=after ? NULL : menu;
        ((MDINEXTMENU *)payload)->hwndNext=after ? NULL : window;
    }
}

static int output_before_reply(UINT message)
{
    if(message==WM_GETTEXT || message==WM_ASKCBFORMATNAME)return !memcmp(page,"before",7);
    if(message==WM_GETMINMAXINFO)return ((MINMAXINFO *)page)->ptReserved.x==0x1234;
    if(message==WM_WINDOWPOSCHANGING)return ((WINDOWPOS *)page)->x==23;
    if(message==WM_MEASUREITEM)return ((MEASUREITEMSTRUCT *)page)->itemWidth==23;
    if(message==WM_NCCALCSIZE)return ((RECT *)page)->left==23;
    if(message==WM_MDIGETACTIVE)return *(BOOL *)page==TRUE;
    if(message==WM_NEXTMENU)return ((MDINEXTMENU *)page)->hmenuNext==menu;
    return -1;
}

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
        if(message==WM_COPYDATA){
            const COPYDATASTRUCT *copy=(const COPYDATASTRUCT *)lp;
            check(copy->dwData==0x456789ab && copy->cbData==8 &&
                !memcmp(copy->lpData,"M4copy!!",8));
            received_data=copy->lpData;
        }else{
            if(message==WM_GETTEXT || message==WM_ASKCBFORMATNAME)check(wp==64);
            write_output(message,(void *)lp,FALSE);
            if(message==WM_NCCALCSIZE && current_variant){
                received_data=((NCCALCSIZE_PARAMS *)lp)->lppos;
                check(received_data!=NULL);
                if(received_data)((WINDOWPOS *)received_data)->x=23;
            }
        }
        check(ReplyMessage(6));
        check(WaitForSingleObject(acknowledgement,5000)==WAIT_OBJECT_0);
        /* Observe without dereferencing first: an alias may be NOACCESS. */
        protection=query_protection(received);
        if(received_data)data_protection=query_protection(received_data);
        capture_write=data_capture_write=0;
        if(message!=WM_COPYDATA && received!=page &&
            writable(protection)){
            __try {
                write_output(message,received,TRUE);
                capture_write=1;
            } __except(EXCEPTION_EXECUTE_HANDLER){check(FALSE);}
        }
        if(message==WM_NCCALCSIZE && current_variant && received_data &&
                received_data!=source_data && writable(data_protection)){
            __try {((WINDOWPOS *)received_data)->x=42;data_capture_write=1;}
            __except(EXCEPTION_EXECUTE_HANDLER){check(FALSE);}
        }
        InterlockedExchange(&continued,1);
        return 0x447a0099;
    }
    return DefWindowProcA(target,message,wp,lp);
}

static DWORD WINAPI send_cases(PVOID ignored)
{
    static const struct {UINT message;UINT wp;} cases[]={
        {WM_GETTEXT,64},{WM_GETMINMAXINFO,0},{WM_WINDOWPOSCHANGING,0},{WM_COPYDATA,0},
        {WM_MEASUREITEM,0},{WM_NCCALCSIZE,0},{WM_NCCALCSIZE,TRUE},
        {WM_ASKCBFORMATNAME,64},{WM_MDIGETACTIVE,0},{WM_NEXTMENU,VK_LEFT}};
    unsigned i;DWORD old,unused;DWORD_PTR result=0;SIZE_T read;
    BYTE byte;
    (void)ignored;
    for(i=0;i<(extended ? sizeof(cases)/sizeof(cases[0]) : 4u);++i){
        UINT message=cases[i].message;
        WPARAM wp=cases[i].wp;
        LRESULT sent;DWORD send_error;DWORD_PTR reply_result;int output_at_reply=-1;
        check(ResetEvent(acknowledgement));
        page=VirtualAlloc(NULL,4096,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
        if(!page){check(FALSE);break;}
        source_data=NULL;
        if(message==WM_COPYDATA){
            COPYDATASTRUCT *copy=(COPYDATASTRUCT *)page;
            copy->dwData=0x456789ab;copy->cbData=8;copy->lpData=page+64;
            memcpy(copy->lpData,"M4copy!!",8);
            source_data=copy->lpData;
        }else if(message==WM_WINDOWPOSCHANGING){
            WINDOWPOS *position=(WINDOWPOS *)page;
            position->hwnd=window;position->hwndInsertAfter=HWND_TOP;
            position->cx=100;position->cy=60;
        }else if(message==WM_MEASUREITEM){
            ((MEASUREITEMSTRUCT *)page)->CtlType=ODT_BUTTON;
        }else if(message==WM_NCCALCSIZE && wp){
            WINDOWPOS *position=(WINDOWPOS *)(page+128);
            ((NCCALCSIZE_PARAMS *)page)->lppos=position;
            position->hwnd=window;position->cx=100;position->cy=60;
            source_data=position;
        }else if(message==WM_NEXTMENU){
            ((MDINEXTMENU *)page)->hmenuIn=menu;
        }
        received=NULL;received_data=NULL;protection=data_protection=0;
        InterlockedExchange(&continued,0);current_variant=(UINT)wp;current_message=message;
        SetLastError(ERROR_SUCCESS);
        sent=SendMessageTimeoutA(window,current_message,wp,(LPARAM)page,
            SMTO_ABORTIFHUNG|SMTO_ERRORONEXIT,5000,&result);
        send_error=GetLastError();reply_result=result;
        check(sent!=0 && result==6);
        if(sent)output_at_reply=output_before_reply(current_message);
        check(VirtualProtect(page,4096,PAGE_NOACCESS,&old));
        read=0;check(!ReadProcessMemory(GetCurrentProcess(),page,&byte,1,&read) && !read);
        check(SetEvent(acknowledgement));
        /* The receiver performs no USER operation between acknowledgement
         * and return. This second cross-thread send brackets full return. */
        if(!SendMessageTimeoutA(window,WM_APP+1,0,0,SMTO_ERRORONEXIT,5000,&result) ||
            result!=0x447a0098 || continued!=1){check(FALSE);break;}
        printf("message=%04X bits=%u alias=%u data_alias=%u protection=%08lX data_protection=%08lX sender=%llX receiver=%llX data=%llX status=%lld error=%lu reply=%llX output_at_reply=%d capture_write=%ld full_return=1\n",
            current_message,(unsigned)(sizeof(void *)*8),received==page,
            source_data && received_data==source_data,protection,data_protection,
            (unsigned long long)(ULONG_PTR)page,(unsigned long long)(ULONG_PTR)received,
            (unsigned long long)(ULONG_PTR)received_data,(long long)sent,send_error,
            (unsigned long long)reply_result,output_at_reply,capture_write);
        if(extended)printf("detail case=%u message=%04X wparam=%u nested_write=%ld nested_required=%u\n",
            i,current_message,current_variant,data_capture_write,
            current_message==WM_NCCALCSIZE && current_variant!=0);
        check(VirtualProtect(page,4096,PAGE_READWRITE,&unused));
        check(VirtualFree(page,0,MEM_RELEASE));page=NULL;
        if(errors)break;
    }
    check(PostMessageA(window,WM_CLOSE,0,0));
    return errors ? 1 : 0;
}

int main(int argc,char **argv)
{
    WNDCLASSA cls={0};MSG msg;HANDLE sender;BOOL got;DWORD code=1;
    if(argc>2 || (argc==2 && strcmp(argv[1],"--extended")))return 2;
    extended=argc==2;
    cls.hInstance=GetModuleHandleA(NULL);cls.lpfnWndProc=receive;
    cls.lpszClassName="T447_CAPTURE_NATIVE_CONTROL";
    acknowledgement=CreateEventA(NULL,TRUE,FALSE,NULL);
    if(!acknowledgement || !RegisterClassA(&cls))return 2;
    window=CreateWindowA(cls.lpszClassName,"Capture control",WS_POPUP,
        0,0,100,60,NULL,NULL,cls.hInstance,NULL);
    if(!window)return 3;
    if(extended){menu=CreateMenu();if(!menu)return 5;}
    sender=CreateThread(NULL,0,send_cases,NULL,0,NULL);
    if(!sender)return 4;
    while((got=GetMessageA(&msg,NULL,0,0))>0)DispatchMessageA(&msg);
    check(got==0 && !IsWindow(window));
    check(WaitForSingleObject(sender,10000)==WAIT_OBJECT_0 &&
        GetExitCodeThread(sender,&code) && code==0);
    check(CloseHandle(sender));check(CloseHandle(acknowledgement));
    if(menu)check(DestroyMenu(menu));
    check(UnregisterClassA(cls.lpszClassName,cls.hInstance));
    printf("errors=%ld\n",errors);
    return errors ? 1 : 0;
}
