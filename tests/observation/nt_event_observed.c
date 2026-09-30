/* Test-only original event-owner substitution. Observe the actual returned
 * records and caller; preserve the original prepend and all device policy. */
#define WriteConsoleInputVDMW observed_WriteConsoleInputVDMW
#define nt_resume_event_thread observed_original_nt_resume_event_thread
#include "../../src/mvdm/softpc.new/host/src/nt_event.c"
#undef WriteConsoleInputVDMW
#undef nt_resume_event_thread
#include <intrin.h>
extern void observed_keyboard_event(LONG kind,const LONG *data,unsigned count);
extern BOOL WINAPI WriteConsoleInputVDMW(HANDLE,PINPUT_RECORD,DWORD,LPDWORD);

static void observed_keyboard_bios(LONG kind)
{
    word head=0,tail=0,start=0,end=0;
    LONG data[13]={0};unsigned count=0;
    sas_loadw(BIOS_KB_BUFFER_HEAD,&head);sas_loadw(BIOS_KB_BUFFER_TAIL,&tail);
    sas_loadw(BIOS_KB_BUFFER_START,&start);sas_loadw(BIOS_KB_BUFFER_END,&end);
    data[0]=head;data[1]=tail;data[2]=start;data[3]=end;
    while(head!=tail && count<9) {
        word value=0;sas_loadw(BIOS_VAR_START+head,&value);
        data[4+count++]=value;head+=2;if(head>=end)head=start;
    }
    observed_keyboard_event(kind,data,13);
}

void nt_resume_event_thread(void)
{
    observed_keyboard_bios(10);
    observed_original_nt_resume_event_thread();
}

BOOL WINAPI observed_WriteConsoleInputVDMW(HANDLE input,PINPUT_RECORD records,
    DWORD count,LPDWORD written)
{
    DWORD caller=(DWORD)(ULONG_PTR)_ReturnAddress(),index,error;
    BOOL result=WriteConsoleInputVDMW(input,records,count,written);
    LONG data[7];
    error=GetLastError();
    data[0]=(LONG)caller;data[1]=count;data[2]=result;
    data[3]=written ? *written:0;data[4]=error;data[5]=KeyQueue.KeyCount;
    observed_keyboard_event(7,data,6);
    for(index=0;index<count;++index) {
        const KEY_EVENT_RECORD *key=&records[index].Event.KeyEvent;
        LONG record[9]={(LONG)caller,index,records[index].EventType,
            key->wVirtualKeyCode,key->wVirtualScanCode,key->bKeyDown,
            key->uChar.UnicodeChar,key->dwControlKeyState,key->wRepeatCount};
        observed_keyboard_event(8,record,9);
    }
    SetLastError(error);return result;
}
