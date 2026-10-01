#include "worker-base/next_command.h"
#include <stdio.h>

static HANDLE source[4];
static DWORD source_error,completed;

DWORD OpenNtBaseClientGetNextNativeCommand(HANDLE *channel,HANDLE *sender,HANDLE *execution,
    HANDLE *frontend,DWORD *request)
{
    *channel=source[0];*sender=source[1];*execution=source[2];*frontend=source[3];
    *request=77;
    return source_error;
}

DWORD OpenNtBaseClientCompleteWorkerChannel(DWORD request)
{ completed=request;return ERROR_SUCCESS; }

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
    worker_base_next_command command={0};
    unsigned index;

    CHECK(open_sources());source_error=ERROR_SUCCESS;
    CHECK(worker_base_get_next_command(&command)==ERROR_SUCCESS);
    CHECK(command.channel==source[0] && command.sender==source[1] &&
        command.execution==source[2] && command.frontend==source[3] && command.request==77);
    worker_base_dispose_next_command(&command);
    CHECK(!command.channel && !command.sender && !command.execution && !command.frontend && !command.request);
    ZeroMemory(source,sizeof(source));

    CHECK(open_sources());source_error=ERROR_ACCESS_DENIED;
    CHECK(worker_base_get_next_command(&command)==ERROR_ACCESS_DENIED);
    CHECK(!command.channel && !command.sender && !command.execution && !command.frontend && !command.request);
    for(index=0;index<ARRAYSIZE(source);++index) {
        CHECK(WaitForSingleObject(source[index],0)==WAIT_FAILED);
        source[index]=NULL;
    }

    completed=0;
    CHECK(worker_base_complete_next_command(0)==ERROR_INVALID_PARAMETER);
    CHECK(worker_base_complete_next_command(91)==ERROR_SUCCESS && completed==91);
    puts("PASS worker-base get-next command ownership, failure disposal and completion");
    return 0;
}
