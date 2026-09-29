/* Deterministic API-edge fault injection around the production source.
 * The ordinary capture fixture still links the unwrapped production object. */
#include <windows.h>
#include <stdio.h>
static unsigned injected;
static BOOL WINAPI race_read(HANDLE,CHAR_INFO *,COORD,COORD,SMALL_RECT *);
#define ReadConsoleOutputW race_read
#include "../../src/ntcon-exe/console_state.c"
#undef ReadConsoleOutputW
static BOOL WINAPI race_read(HANDLE output,CHAR_INFO *cells,COORD size,COORD origin,SMALL_RECT *rect)
{
    unsigned mode=injected;injected=0;
    if(mode==1 && !SetConsoleScreenBufferSize(output,(COORD){80,25}))return FALSE;
    if(mode) {SetLastError(mode==2 ? ERROR_ACCESS_DENIED : ERROR_INVALID_PARAMETER);return FALSE;}
    return ReadConsoleOutputW(output,cells,size,origin,rect);
}
int wmain(int argc,WCHAR **argv)
{
    HANDLE output;FILE *log;unsigned mode,failures=0;
    if(argc!=2 || _wfopen_s(&log,argv[1],L"wx"))return 2;
    output=CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
    if(output==INVALID_HANDLE_VALUE){fclose(log);return 3;}
    for(mode=0;mode<4;++mode) {
        ntcon_capture capture={0};CHAR_INFO cells[80];SMALL_RECT rect={0,0,19,1};
        DWORD count=99,error,expected=mode==1 ? ERROR_RETRY :
            mode==2 ? ERROR_ACCESS_DENIED : mode==3 ? ERROR_INVALID_PARAMETER : ERROR_SUCCESS;
        if(!SetConsoleWindowInfo(output,TRUE,&rect) ||
            !SetConsoleScreenBufferSize(output,(COORD){80,300}) ||
            ntcon_capture_begin_output(&capture,output)) {++failures;break;}
        injected=mode;
        error=ntcon_capture_read(&capture,80*200,cells,80,&rect,&count);
        fprintf(log,"case=%u error=%lu expected=%lu count=%lu\n",mode,error,expected,count);
        if(error!=expected || count!=(mode ? 0u : 80u))++failures;
        ntcon_capture_end(&capture);
    }
    CloseHandle(output);
    fprintf(log,"NTCON-CAPTURE-RACE failures=%u changed-only-retry=yes unchanged-errors-preserved=yes\n",failures);
    fclose(log);return failures ? 1 : 0;
}
