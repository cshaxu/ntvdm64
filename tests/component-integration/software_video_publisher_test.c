#include "worker-base/publication.h"
#include "common/protocol/console_video.h"
#include <stdio.h>
#include <string.h>

static HANDLE entered,unblock,ack;
static volatile LONG calls;
static DWORD values[32],fail_value;
static LARGE_INTEGER times[32],frequency;
#define CHECK(x) do {if(!(x)){fprintf(stderr,"FAIL line %d error %lu\n",__LINE__,GetLastError());return 1;}} while(0)
typedef struct fixture_copy { console_video_description description;DWORD value; } fixture_copy;
static DWORD send_frame(void *context,const void *payload,SIZE_T bytes)
{
    const fixture_copy *copy=payload;
    LONG index=calls;(void)context;
    if(index>=32)return ERROR_BUFFER_OVERFLOW;
    values[index]=copy->value;QueryPerformanceCounter(&times[index]);
    if(index==0){SetEvent(entered);WaitForSingleObject(unblock,INFINITE);}
    if(values[index]==fail_value)return ERROR_BROKEN_PIPE;
    if(bytes!=sizeof(*copy) || copy->description.bytes!=sizeof(DWORD))return ERROR_INVALID_DATA;
    InterlockedIncrement(&calls);ReleaseSemaphore(ack,1,NULL);return ERROR_SUCCESS;
}
static DWORD offer(worker_base_publication *p,DWORD value)
{
    fixture_copy copy={0};BOOL queued=FALSE;DWORD error;
    console_video_description *description=&copy.description;copy.value=value;
    description->kind=CONSOLE_VIDEO_TEXT_FRAME;description->width=2;description->height=1;
    description->stride=4;description->bytes=sizeof(value);
    error=worker_base_publication_offer(p,&copy,sizeof(copy),&queued);
    return error ? error : queued ? 0 : ERROR_NOT_READY;
}
int main(void)
{
    HANDLE shutdown=CreateEventW(NULL,TRUE,FALSE,NULL);
    worker_base_publication *p;DWORD i;LARGE_INTEGER start,end;
    QueryPerformanceFrequency(&frequency);
    entered=CreateEventW(NULL,TRUE,FALSE,NULL);unblock=CreateEventW(NULL,TRUE,FALSE,NULL);
    ack=CreateSemaphoreW(NULL,0,32,NULL);CHECK(shutdown && entered && unblock && ack);
    p=worker_base_publication_create(send_frame,NULL,shutdown);CHECK(p);
    CHECK(worker_base_publication_active(p,TRUE)==0);
    CHECK(offer(p,1)==0 && WaitForSingleObject(entered,5000)==WAIT_OBJECT_0);
    QueryPerformanceCounter(&start);
    for(i=2;i<=201;++i)CHECK(offer(p,i)==0);
    QueryPerformanceCounter(&end);
    CHECK((end.QuadPart-start.QuadPart)*1000/frequency.QuadPart<1000);
    SetEvent(unblock);
    CHECK(WaitForSingleObject(ack,5000)==WAIT_OBJECT_0);
    CHECK(WaitForSingleObject(ack,5000)==WAIT_OBJECT_0);
    CHECK(calls==2 && values[0]==1 && values[1]==201);
    CHECK((times[1].QuadPart-times[0].QuadPart)*1000000/frequency.QuadPart>=20000);
    CHECK(offer(p,201)==0 && WaitForSingleObject(ack,80)==WAIT_TIMEOUT && calls==2);
    /* Palette/cursor/font differences are represented in the complete
     * descriptor/payload, not just character memory. Change a palette only. */
    {
        fixture_copy copy={0};BOOL queued=FALSE;
        console_video_description *description=&copy.description;copy.value=201;
        description->kind=CONSOLE_VIDEO_TEXT_FRAME;description->width=2;description->height=1;
        description->stride=4;description->bytes=4;description->palette[0]=123;
        CHECK(!worker_base_publication_offer(p,&copy,sizeof(copy),&queued) && queued);
        CHECK(WaitForSingleObject(ack,5000)==WAIT_OBJECT_0 && calls==3);
    }
    CHECK(offer(p,202)==0 && !worker_base_publication_active(p,FALSE));
    CHECK(calls==4 && values[3]==202);
    CHECK(WaitForSingleObject(ack,0)==WAIT_OBJECT_0); /* Drain's real send ack. */
    CHECK(WaitForSingleObject(ack,60)==WAIT_TIMEOUT && calls==4);
    CHECK(!worker_base_publication_active(p,TRUE) && offer(p,202)==0);
    CHECK(WaitForSingleObject(ack,5000)==WAIT_OBJECT_0 && calls==5);
    fail_value=203;CHECK(offer(p,203)==0);
    CHECK(worker_base_publication_active(p,FALSE)==ERROR_BROKEN_PIPE);
    CHECK(offer(p,204)==ERROR_BROKEN_PIPE);
    CHECK(worker_base_publication_active(p,TRUE)==ERROR_BROKEN_PIPE);
    worker_base_publication_destroy(p);
    {
        fixture_copy copy={0};LONG before=calls;
        copy.description.bytes=4;copy.value=205;
        p=worker_base_publication_create(send_frame,NULL,shutdown);CHECK(p);
        CHECK(!worker_base_publication_commit(p,&copy,sizeof(copy)) && calls==before+1);
        CHECK(!worker_base_publication_commit(p,&copy,sizeof(copy)) && calls==before+1);
        copy.description.palette[0]=42;
        CHECK(!worker_base_publication_commit(p,&copy,sizeof(copy)) && calls==before+2);
        CHECK(worker_base_publication_offer(p,NULL,0,NULL)==ERROR_INVALID_PARAMETER);
        worker_base_publication_destroy(p);
    }
    {
        DWORD before,after;
        CHECK(GetProcessHandleCount(GetCurrentProcess(),&before));
        for(i=0;i<50;++i){
            p=worker_base_publication_create(send_frame,NULL,shutdown);CHECK(p);
            worker_base_publication_destroy(p);
        }
        CHECK(GetProcessHandleCount(GetCurrentProcess(),&after) && after==before);
        CHECK(WaitForSingleObject(shutdown,0)==WAIT_TIMEOUT);
    }
    CloseHandle(shutdown);CloseHandle(entered);CloseHandle(unblock);CloseHandle(ack);
    puts("PASS production publisher: latest of200, >=20ms, unchanged idle, palette-only change, forced final drain, resume, sticky failed send, 50 stop/join cycles without handle leaks");
    return 0;
}
