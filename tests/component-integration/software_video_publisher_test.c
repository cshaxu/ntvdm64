#include "worker-base/publication.h"
#include "common/protocol/console_video.h"
#include <stdio.h>
#include <string.h>

static HANDLE entered,unblock,ack;
static volatile LONG calls;
static DWORD values[32],fail_value;
static LARGE_INTEGER times[32],frequency;
static volatile LONG source_captures;
static DWORD source_value;
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
static DWORD capture_source(void *context,void **payload,SIZE_T *bytes)
{
    fixture_copy *copy;
    (void)context;
    if(!payload || !bytes)return ERROR_INVALID_PARAMETER;
    *payload=NULL;*bytes=0;
    copy=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*copy));
    if(!copy)return ERROR_NOT_ENOUGH_MEMORY;
    copy->description.kind=CONSOLE_VIDEO_TEXT_FRAME;copy->description.width=2;
    copy->description.height=1;copy->description.stride=4;copy->description.bytes=sizeof(copy->value);
    copy->value=source_value;InterlockedIncrement(&source_captures);
    *payload=copy;*bytes=sizeof(*copy);return ERROR_SUCCESS;
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
    {
        LONG before=calls,uncaptured;
        while(WaitForSingleObject(ack,0)==WAIT_OBJECT_0) {}
        p=worker_base_publication_create(send_frame,NULL,shutdown);CHECK(p);
        CHECK(!worker_base_publication_set_capture(p,capture_source,NULL));
        CHECK(!worker_base_publication_active(p,TRUE));
        uncaptured=source_captures;
        CHECK(WaitForSingleObject(ack,80)==WAIT_TIMEOUT && source_captures==uncaptured);
        source_value=301;CHECK(!worker_base_publication_signal(p));
        CHECK(WaitForSingleObject(ack,5000)==WAIT_OBJECT_0);
        CHECK(calls==before+1 && values[before]==301 && source_captures==uncaptured+1);
        source_value=302;CHECK(!worker_base_publication_signal(p));
        CHECK(!worker_base_publication_active(p,FALSE));
        CHECK(WaitForSingleObject(ack,5000)==WAIT_OBJECT_0);
        CHECK(calls==before+2 && values[before+1]==302 && source_captures==uncaptured+2);
        CHECK(worker_base_publication_signal(NULL)==ERROR_SUCCESS);
        worker_base_publication_destroy(p);
    }
    CloseHandle(shutdown);CloseHandle(entered);CloseHandle(unblock);CloseHandle(ack);
    puts("PASS production publisher: latest of200, >=20ms, unchanged idle, palette-only change, deferred source capture, forced final drain, resume, sticky failed send, 50 stop/join cycles without handle leaks");
    return 0;
}
