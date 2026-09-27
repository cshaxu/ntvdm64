/* Test-only link substitution, never a product input. Record the original
 * keyboard's input/count/port boundaries without file I/O at handoff. */
#define keys_in_6805_buff observed_original_keys_in_6805_buff
#define AT_kbd_init observed_original_AT_kbd_init
#include "../../src/mvdm/softpc.new/base/keymouse/keyba.c"
#undef keys_in_6805_buff
#undef AT_kbd_init
#include <stdlib.h>

#define OBSERVED_CAPACITY 4096
#define OBSERVED_MAGIC 0x3144424b /* KBD1, fixed little-endian DWORD records */
typedef struct observed_event {
    volatile LONG kind; /* Published last; decoder owns the record field names. */
    DWORD tick,thread;
    LONG data[13];
} observed_event;
typedef struct observed_report {
    DWORD magic;
    volatile LONG count,dropped;
    DWORD capacity;
    observed_event events[OBSERVED_CAPACITY];
} observed_report;
static observed_report *report;
static void record_event(LONG kind,const LONG *data,unsigned count)
{
    LONG index;observed_event *event;
    if(!report)return;
    index=InterlockedIncrement(&report->count)-1;
    if(index>=OBSERVED_CAPACITY) { InterlockedIncrement(&report->dropped);return; }
    event=&report->events[index];event->tick=GetTickCount();
    event->thread=GetCurrentThreadId();
    memcpy(event->data,data,count*sizeof(*data));
    InterlockedExchange(&event->kind,kind);
}
void observed_keyboard_event(LONG kind,const LONG *data,unsigned count)
{
    record_event(kind,data,count);
}
static void observe_key(int key,int down)
{
    LONG data[9]={key,down,keyboard_disabled,scanning_discontinued,
        LastKeyDown,KbdHdwFull,key>=0 && key<127 ? key_down_count[key] : -1,0,0};
    if(down)host_key_down(key);else host_key_up(key);
    data[7]=key>=0 && key<127 ? key_down_count[key] : -1;
    data[8]=output_contents;
    record_event(1,data,9);
}
static void observed_down(int key) { observe_key(key,1); }
static void observed_up(int key) { observe_key(key,0); }
static void observed_inb(io_addr port,half_word *value)
{
    kbd_inb(port,value);
    if((port&0x64)==0x60) {
        LONG data[7]={*value,output_contents,KbdData,output_full,
            bKbdEoiPending,bDelayIntPending,DelayIrqLine};
        record_event(5,data,7);
    }
}
void AT_kbd_init(void)
{
    const char *path=getenv("MVDM_TEST_KEYBOARD_REPORT");
    observed_original_AT_kbd_init();
    if(path && !report) {
        HANDLE file=CreateFileA(path,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,
            NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL),mapping;
        if(file==INVALID_HANDLE_VALUE)ExitProcess(ERROR_OPEN_FAILED);
        mapping=CreateFileMappingW(file,NULL,PAGE_READWRITE,0,sizeof(*report),NULL);
        CloseHandle(file);
        if(!mapping)ExitProcess(ERROR_NOT_ENOUGH_MEMORY);
        report=MapViewOfFile(mapping,FILE_MAP_WRITE,0,0,sizeof(*report));
        CloseHandle(mapping);
        if(!report)ExitProcess(ERROR_NOT_ENOUGH_MEMORY);
        report->magic=OBSERVED_MAGIC;report->capacity=OBSERVED_CAPACITY;
        /* The process owns the mapping until exit; read it after exit or
         * cleanup. No per-key or handoff file write/flush is performed. */
    }
    /* A handoff-only run leaves the hot input/port function pointers original,
     * reducing observer perturbation without changing the device behavior. */
    if(!getenv("MVDM_TEST_KEYBOARD_HISTORY_ONLY")) {
        host_key_down_fn_ptr=observed_down;host_key_up_fn_ptr=observed_up;
        io_define_inb(AT_KEYB_ADAPTOR,observed_inb);
    }
}

extern int GetHistoryKeyEvent(PKEY_EVENT_RECORD event,int number);
int keys_in_6805_buff(int *part_key_transferred)
{
    LONG data[8]={held_event_count,output_full,pending_8042,KbdData,
        output_contents,buff_6805_out_ptr,buff_6805_in_ptr,0};
    int index,result;
    KEY_EVENT_RECORD key;
    record_event(2,data,7);
    /* nt_block_event_thread has stopped the input thread at this boundary.
     * Read the actual source-owned history, newest first, before its later
     * ReturnUnusedKeyEvents call. This does not dequeue or alter a record. */
    for(index=1;index<=4096 && GetHistoryKeyEvent(&key,index);++index) {
        LONG history[7]={index,key.wVirtualKeyCode,key.wVirtualScanCode,
            key.bKeyDown,key.uChar.UnicodeChar,key.dwControlKeyState,key.wRepeatCount};
        record_event(6,history,7);
    }
    for(index=buff_6805_out_ptr;index!=buff_6805_in_ptr;
        index=(index+1)&BUFF_6805_PMASK) {
        LONG marker[2]={index,key_marker_buffer[index]};
        record_event(3,marker,2);
    }
    result=observed_original_keys_in_6805_buff(part_key_transferred);
    data[0]=result;data[1]=*part_key_transferred;record_event(4,data,2);
    return result;
}
