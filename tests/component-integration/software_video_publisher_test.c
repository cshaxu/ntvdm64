#include "ntvdm-exe/win32/console_video_publisher.h"
#include <stdio.h>
#include <string.h>

static HANDLE entered,unblock,ack;
static volatile LONG calls;
static DWORD values[32],fail_value;
static LARGE_INTEGER times[32],frequency;
#define CHECK(x) do {if(!(x)){fprintf(stderr,"FAIL line %d error %lu\n",__LINE__,GetLastError());return 1;}} while(0)
static DWORD send_frame(void *context,const console_video_description *description,const void *payload)
{
    LONG index=calls;(void)context;
    if(index>=32)return ERROR_BUFFER_OVERFLOW;
    values[index]=*(const DWORD *)payload;QueryPerformanceCounter(&times[index]);
    if(index==0){SetEvent(entered);WaitForSingleObject(unblock,INFINITE);}
    if(values[index]==fail_value)return ERROR_BROKEN_PIPE;
    if(description->bytes!=sizeof(DWORD))return ERROR_INVALID_DATA;
    InterlockedIncrement(&calls);ReleaseSemaphore(ack,1,NULL);return ERROR_SUCCESS;
}
static DWORD offer(ntvdm_video_publisher *p,DWORD value)
{
    console_video_description description={0};BOOL queued=FALSE;DWORD error;
    description.kind=CONSOLE_VIDEO_TEXT_FRAME;description.width=2;description.height=1;
    description.stride=4;description.bytes=sizeof(value);
    error=ntvdm_video_publisher_offer(p,&description,&value,&queued);
    return error ? error : queued ? 0 : ERROR_NOT_READY;
}
int main(void)
{
    HANDLE shutdown=CreateEventW(NULL,TRUE,FALSE,NULL);
    ntvdm_video_publisher *p;DWORD i;LARGE_INTEGER start,end;
    QueryPerformanceFrequency(&frequency);
    entered=CreateEventW(NULL,TRUE,FALSE,NULL);unblock=CreateEventW(NULL,TRUE,FALSE,NULL);
    ack=CreateSemaphoreW(NULL,0,32,NULL);CHECK(shutdown && entered && unblock && ack);
    p=ntvdm_video_publisher_create(send_frame,NULL,shutdown);CHECK(p);
    CHECK(ntvdm_video_publisher_active(p,TRUE)==0);
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
        console_video_description description={0};BOOL queued=FALSE;DWORD value=201;
        description.kind=CONSOLE_VIDEO_TEXT_FRAME;description.width=2;description.height=1;
        description.stride=4;description.bytes=4;description.palette[0]=123;
        CHECK(!ntvdm_video_publisher_offer(p,&description,&value,&queued) && queued);
        CHECK(WaitForSingleObject(ack,5000)==WAIT_OBJECT_0 && calls==3);
    }
    CHECK(offer(p,202)==0 && !ntvdm_video_publisher_active(p,FALSE));
    CHECK(calls==4 && values[3]==202);
    CHECK(WaitForSingleObject(ack,0)==WAIT_OBJECT_0); /* Drain's real send ack. */
    CHECK(WaitForSingleObject(ack,60)==WAIT_TIMEOUT && calls==4);
    CHECK(!ntvdm_video_publisher_active(p,TRUE) && offer(p,202)==0);
    CHECK(WaitForSingleObject(ack,5000)==WAIT_OBJECT_0 && calls==5);
    fail_value=203;CHECK(offer(p,203)==0);
    CHECK(ntvdm_video_publisher_active(p,FALSE)==ERROR_BROKEN_PIPE);
    CHECK(offer(p,204)==ERROR_BROKEN_PIPE);
    ntvdm_video_publisher_destroy(p);
    {
        DWORD before,after;
        CHECK(GetProcessHandleCount(GetCurrentProcess(),&before));
        for(i=0;i<50;++i){
            p=ntvdm_video_publisher_create(send_frame,NULL,shutdown);CHECK(p);
            ntvdm_video_publisher_destroy(p);
        }
        CHECK(GetProcessHandleCount(GetCurrentProcess(),&after) && after==before);
        CHECK(WaitForSingleObject(shutdown,0)==WAIT_TIMEOUT);
    }
    CloseHandle(shutdown);CloseHandle(entered);CloseHandle(unblock);CloseHandle(ack);
    puts("PASS production publisher: latest of200, >=20ms, unchanged idle, palette-only change, forced final drain, resume, sticky failed send, 50 stop/join cycles without handle leaks");
    return 0;
}
