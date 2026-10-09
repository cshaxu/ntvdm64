#include "worker-base/publication.h"
#include "ntvdm-exe/softpc/mvdm_softpc_mouse_input.h"
#include "common/protocol/console_mouse.h"
#include "common/protocol/console_video.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SAMPLE_COUNT 64

typedef struct publisher_fixture {
    HANDLE delivered;
    LARGE_INTEGER frequency;
    LARGE_INTEGER offered[SAMPLE_COUNT];
    LONGLONG latency_us[SAMPLE_COUNT];
    volatile LONG count;
} publisher_fixture;

typedef struct video_fixture {
    console_video_description description;
    DWORD sequence;
} video_fixture;

static int compare_ticks(const void *left,const void *right)
{
    const LONGLONG a=*(const LONGLONG *)left,b=*(const LONGLONG *)right;
    return a<b ? -1 : a>b;
}

static DWORD publish(void *context,const void *payload,SIZE_T bytes)
{
    publisher_fixture *fixture=context;
    LARGE_INTEGER now;
    LONG index=InterlockedIncrement(&fixture->count)-1;
    (void)payload;
    if(index<0 || index>=SAMPLE_COUNT || bytes!=sizeof(video_fixture))return ERROR_INVALID_DATA;
    QueryPerformanceCounter(&now);
    fixture->latency_us[index]=(now.QuadPart-fixture->offered[index].QuadPart)*1000000/
        fixture->frequency.QuadPart;
    SetEvent(fixture->delivered);
    return ERROR_SUCCESS;
}

static DWORD offer(worker_base_publication *publisher,publisher_fixture *fixture,DWORD sequence)
{
    video_fixture frame={0};
    BOOL queued=FALSE;
    LARGE_INTEGER now;
    frame.description.kind=CONSOLE_VIDEO_TEXT_FRAME;
    frame.description.width=1;
    frame.description.height=1;
    frame.description.stride=sizeof(DWORD);
    frame.description.bytes=sizeof(DWORD);
    frame.sequence=sequence;
    QueryPerformanceCounter(&now);
    fixture->offered[sequence]=now;
    return worker_base_publication_offer(publisher,&frame,sizeof(frame),&queued) ? ERROR_GEN_FAILURE :
        queued ? ERROR_SUCCESS : ERROR_INVALID_STATE;
}

static int report_publisher(void)
{
    publisher_fixture fixture={0};
    worker_base_publication *publisher;
    HANDLE shutdown;
    LONGLONG sorted[SAMPLE_COUNT],sum=0;
    DWORD index;

    QueryPerformanceFrequency(&fixture.frequency);
    shutdown=CreateEventW(NULL,TRUE,FALSE,NULL);
    fixture.delivered=CreateEventW(NULL,FALSE,FALSE,NULL);
    if(!shutdown || !fixture.delivered)return 1;
    publisher=worker_base_publication_create(publish,&fixture,shutdown);
    if(!publisher || worker_base_publication_active(publisher,TRUE))return 1;
    for(index=0;index<SAMPLE_COUNT;++index) {
        if(offer(publisher,&fixture,index) || WaitForSingleObject(fixture.delivered,5000)!=WAIT_OBJECT_0) return 1;
    }
    if(InterlockedCompareExchange(&fixture.count,0,0)!=SAMPLE_COUNT)return 1;
    memcpy(sorted,fixture.latency_us,sizeof(sorted));
    qsort(sorted,SAMPLE_COUNT,sizeof(sorted[0]),compare_ticks);
    for(index=0;index<SAMPLE_COUNT;++index)sum+=sorted[index];
    printf("publisher samples=%u offer-to-send-us min=%lld p50=%lld p95=%lld max=%lld mean=%lld\n",
        SAMPLE_COUNT,sorted[0],sorted[SAMPLE_COUNT/2],sorted[(SAMPLE_COUNT*95+99)/100-1],
        sorted[SAMPLE_COUNT-1],sum/SAMPLE_COUNT);
    worker_base_publication_destroy(publisher);
    CloseHandle(fixture.delivered);CloseHandle(shutdown);
    return 0;
}

static int report_mouse_queue(void)
{
    enum { iterations=200000 };
    mvdm_mouse_input queue={0};
    mvdm_mouse_input_sample input={1,-1,0,CONSOLE_MOUSE_MOVE},output;
    LARGE_INTEGER frequency,start,end;
    unsigned index;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);
    for(index=0;index<iterations;++index) {
        if(!mvdm_mouse_input_push(&queue,&input) || !mvdm_mouse_input_take(&queue,&output))return 1;
    }
    QueryPerformanceCounter(&end);
    printf("mouse-queue iterations=%u push-plus-take-us-total=%lld mean-ns=%lld\n",iterations,
        (end.QuadPart-start.QuadPart)*1000000/frequency.QuadPart,
        (end.QuadPart-start.QuadPart)*1000000000/(frequency.QuadPart*iterations));
    return 0;
}

int main(void)
{
    if(report_publisher() || report_mouse_queue()) {
        fputs("FAIL worker primitive measurement\n",stderr);
        return 1;
    }
    return 0;
}
