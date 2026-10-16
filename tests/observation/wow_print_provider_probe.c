/* Native provider contract only. Original WOW registers via Escape SETABORTPROC.
 * Explicit Microsoft Print to PDF output: never default/physical printing. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

static DWORD owner_thread, slot=TLS_OUT_OF_INDEXES;
static HDC active_dc;
static unsigned calls, foreign, outside, phase, cancellations;
static BOOL cancel_requested;

static BOOL CALLBACK abort_callback(HDC dc, int code)
{
    ++calls;
    if(GetCurrentThreadId()!=owner_thread || TlsGetValue(slot)!=(PVOID)1 || dc!=active_dc)++foreign;
    if(!phase)++outside;
    printf("abort-callback tid=%lu phase=%u code=%d cancel=%u\n",GetCurrentThreadId(),phase,code,cancel_requested);
    if(cancel_requested && phase==4){++cancellations;return FALSE;}
    return TRUE;
}

static int run_document(const WCHAR *path, BOOL cancel)
{
    DOCINFOW document={0};
    int started=0, ended=0, escaped, result=1;
    unsigned before=calls, attempt;
    active_dc=CreateDCW(L"WINSPOOL",L"Microsoft Print to PDF",NULL,NULL);
    if(!active_dc){printf("create-dc-error=%lu\n",GetLastError());return 1;}
    cancel_requested=cancel;
    __try {
        phase=1;
        escaped=Escape(active_dc,SETABORTPROC,0,(LPCSTR)abort_callback,NULL);
        printf("register=%d error=%lu\n",escaped,GetLastError());
        if(escaped<=0)__leave;
        document.cbSize=sizeof(document);
        document.lpszDocName=L"T447 owned abort-callback witness";
        document.lpszOutput=path;
        phase=2;started=StartDocW(active_dc,&document);
        printf("start=%d error=%lu\n",started,GetLastError());
        if(started>0){
            phase=3;
            if(StartPage(active_dc)<=0 && !cancel)__leave;
            phase=4;
            for(attempt=0;attempt<20;++attempt){
                BOOL drawn=Rectangle(active_dc,10,10,110,110);
                if(!drawn && !cancel)__leave;
                if(cancellations && cancel)break;
                Sleep(100);
            }
            phase=5;
            if(cancel){
                int page=EndPage(active_dc);
                ended=EndDoc(active_dc);
                printf("cancel-page=%d cancel-end=%d error=%lu\n",page,ended,GetLastError());
                /* Original EndDoc returns1 when AbortDoc already cleared
                 * DOC_STARTED. EndPage rejects the cancelled document. */
                if(page>0 || ended!=1)__leave;
            }else{
                if(EndPage(active_dc)<=0)__leave;
                phase=6;ended=EndDoc(active_dc);
                if(ended<=0)__leave;
            }
        }else if(!cancel)__leave;
        if(calls==before || foreign || outside || (cancel && !cancellations))__leave;
        result=0;
    } __finally {
        if(started>0 && ended<=0)AbortDoc(active_dc);
        phase=0;
        DeleteDC(active_dc);active_dc=NULL;
    }
    printf("document cancel=%u result=%d callbacks=%u\n",cancel,result,calls-before);
    return result;
}

int wmain(int argc, WCHAR **argv)
{
    int result;
    setvbuf(stdout,NULL,_IONBF,0);
    if(argc!=3)return 2;
    owner_thread=GetCurrentThreadId();slot=TlsAlloc();
    if(slot==TLS_OUT_OF_INDEXES || !TlsSetValue(slot,(PVOID)1))return 3;
    result=run_document(argv[1],FALSE);
    if(!result)result=run_document(argv[2],TRUE);
    TlsFree(slot);
    printf("PRINT_PROVIDER result=%d calls=%u foreign=%u outside=%u cancellations=%u\n",result,calls,foreign,outside,cancellations);
    return result;
}
