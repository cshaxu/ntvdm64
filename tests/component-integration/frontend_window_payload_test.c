#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include "lib/types/types_interface.h"

/* Instrument only leaf payload allocation, never exchange metadata or product
 * code. Includes the unchanged public inline allocator/retirement routines. */
static LONG live,peak,allocations,releases;
static BOOL fail_next;
static lib_size requested;
static void *payload_allocate(lib_size count,lib_size size)
{
    void *memory;
    requested=count*size;
    if(fail_next){fail_next=FALSE;return NULL;}
    memory=lib_allocate_zero(count,size);
    if(memory){++allocations;if(++live>peak)peak=live;}
    return memory;
}
static void payload_release(void *memory)
{
    if(memory){++releases;--live;}
    lib_release(memory);
}
#define lib_allocate_zero payload_allocate
#define lib_release payload_release
#include "lib/kvm-window/frame_interface.h"
#undef lib_allocate_zero
#undef lib_release

typedef struct reader {
    kvm_frame_exchange *exchange;
    HANDLE ready,finish;
} reader;
static DWORD WINAPI hold_frame(void *opaque)
{
    reader *state=opaque;
    const kvm_window_frame_storage *held=kvm_frame_exchange_take(state->exchange,NULL);
    BOOL valid=held && held->frame.text && held->frame.sequence==1;
    SetEvent(state->ready);
    if(WaitForSingleObject(state->finish,10000)!=WAIT_OBJECT_0)return 2;
    valid=valid && held->frame.text->base.capacity==2000 &&
        held->frame.text->base.cells[1999].glyph_index==123;
    kvm_frame_exchange_finish(state->exchange,valid ? LIB_TRUE : LIB_FALSE);
    return valid ? 0 : 3;
}
#define CHECK(x) do { if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);return 1;} } while(0)
int main(void)
{
    reader state={0};HANDLE thread;DWORD code;
    kvm_window_frame_storage *writer;
    CHECK(kvm_frame_exchange_create(&state.exchange,sizeof(*writer),kvm_window_frame_retire)==LIB_STATUS_OK);
    CHECK(!live && !allocations);
    writer=kvm_frame_exchange_writer(state.exchange);
    CHECK(kvm_window_frame_prepare_text(writer,80,25,80)==LIB_STATUS_OK);
    CHECK(requested==sizeof(kvm_window_text_frame)+2000*sizeof(kvm_text_cell) && live==1);
    void *same=writer->data.text;
    CHECK(kvm_window_frame_prepare_text(writer,80,25,80)==LIB_STATUS_OK && writer->data.text==same && allocations==1);
    writer->frame.sequence=1;writer->data.text->base.cells[1999].glyph_index=123;
    CHECK(kvm_window_frame_commit(state.exchange)==LIB_STATUS_OK);
    state.ready=CreateEventW(NULL,TRUE,FALSE,NULL);state.finish=CreateEventW(NULL,TRUE,FALSE,NULL);
    CHECK(state.ready && state.finish);
    thread=CreateThread(NULL,0,hold_frame,&state,0,NULL);CHECK(thread);
    CHECK(WaitForSingleObject(state.ready,10000)==WAIT_OBJECT_0);
    for(unsigned i=2;i<34;++i) {
        writer=kvm_frame_exchange_writer(state.exchange);
        CHECK(kvm_window_frame_prepare_graphics(writer,i,2,i)==LIB_STATUS_OK);
        CHECK(requested==sizeof(kvm_window_graphics_frame)+i*2*sizeof(lib_u32));
        fail_next=TRUE;
        CHECK(kvm_window_frame_prepare_text(writer,i,3,i)==LIB_STATUS_NO_MEMORY);
        CHECK(!writer->frame.text && !writer->frame.graphics);
        CHECK(kvm_window_frame_commit(state.exchange)==LIB_STATUS_INVALID_ARGUMENT);
        CHECK(kvm_window_frame_prepare_text(writer,i,3,i)==LIB_STATUS_OK);
        CHECK(requested==sizeof(kvm_window_text_frame)+i*3*sizeof(kvm_text_cell));
        writer->frame.sequence=i;
        CHECK(kvm_window_frame_commit(state.exchange)==LIB_STATUS_OK);
    }
    CHECK(SetEvent(state.finish));
    CHECK(WaitForSingleObject(thread,10000)==WAIT_OBJECT_0 && GetExitCodeThread(thread,&code) && !code);
    CHECK(CloseHandle(thread) && CloseHandle(state.ready) && CloseHandle(state.finish));
    kvm_frame_exchange_detach(state.exchange);
    kvm_frame_exchange_destroy(state.exchange);
    CHECK(!live && allocations==releases && peak<=4);
    printf("PASS actual-size allocation, lazy slots, concurrent held reader and 32 allocation failures; allocations=%ld releases=%ld peak=%ld\n",
        allocations,releases,peak);
    return 0;
}
