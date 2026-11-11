#include "ntvwm-exe/next_command.h"
#include <stdio.h>

static HANDLE source[3];
static DWORD source_error,completed,source_bytes=4;

DWORD OpenNtBaseClientGetNextNativeCommand(DWORD capacity,BYTE *payload,DWORD *bytes,HANDLE *sender,HANDLE *execution,
    HANDLE *frontend,DWORD *request,DWORD *caller_generation)
{
    if(capacity<source_bytes)return ERROR_INSUFFICIENT_BUFFER;
    if(source_bytes)CopyMemory(payload,"RPC!",source_bytes);
    *bytes=source_bytes;
    *sender=source[0];*execution=source[1];*frontend=source[2];
    *request=77;
    *caller_generation=88;
    return source_error;
}

DWORD OpenNtBaseClientCompleteWorkerChannel(DWORD request,DWORD exit_code)
{ (void)exit_code;completed=request;return ERROR_SUCCESS; }

DWORD OpenNtBaseClientCompleteNativeRequest(DWORD request,DWORD exit_code,DWORD io_error,DWORD io_flags)
{ (void)exit_code;(void)io_error;(void)io_flags;completed=request;return ERROR_SUCCESS; }

#define CHECK(value) do { if(!(value)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#value); return 1; } } while(0)

static void close_sources(void)
{
    unsigned index;
    for(index=0;index<ARRAYSIZE(source);++index) {
        if(source[index])CloseHandle(source[index]);
        source[index]=NULL;
    }
}

static int open_sources(void)
{
    unsigned index;
    for(index=0;index<ARRAYSIZE(source);++index) {
        source[index]=CreateEventW(NULL,TRUE,FALSE,NULL);
        if(!source[index]) { close_sources();return 0; }
    }
    return 1;
}

int main(void)
{
    ntvwm_next_command command={0};
    unsigned index;

    CHECK(open_sources());source_error=ERROR_SUCCESS;
    CHECK(ntvwm_get_next_command(&command)==ERROR_SUCCESS);
    CHECK(command.payload && command.bytes==4 && !memcmp(command.payload,"RPC!",4));
    CHECK(command.sender==source[0] &&
        command.execution==source[1] && command.frontend==source[2] && command.request==77 && command.caller_generation==88);
    ntvwm_dispose_next_command(&command);
    CHECK(!command.payload && !command.bytes && !command.sender && !command.execution && !command.frontend && !command.request && !command.caller_generation);
    ZeroMemory(source,sizeof(source));

    CHECK(open_sources());source_error=ERROR_ACCESS_DENIED;
    CHECK(ntvwm_get_next_command(&command)==ERROR_ACCESS_DENIED);
    CHECK(!command.payload && !command.bytes && !command.sender && !command.execution && !command.frontend && !command.request && !command.caller_generation);
    for(index=0;index<ARRAYSIZE(source);++index) {
        CHECK(WaitForSingleObject(source[index],0)==WAIT_FAILED);
        source[index]=NULL;
    }

    CHECK(open_sources());source_error=ERROR_SUCCESS;source_bytes=0;
    CHECK(ntvwm_get_next_command(&command)==ERROR_SUCCESS);
    CHECK(!command.payload && !command.bytes && command.sender && command.request==77);
    ntvwm_dispose_next_command(&command);ZeroMemory(source,sizeof(source));

    completed=91;
    CHECK(ntvwm_complete_next_command(0,0)==ERROR_SUCCESS && completed==0);
    CHECK(ntvwm_complete_next_command(91,37)==ERROR_SUCCESS && completed==91);
    puts("PASS ntvwm get-next command ownership, failure disposal and completion");
    return 0;
}
