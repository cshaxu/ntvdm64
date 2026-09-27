/* Test-only original event-owner substitution. Observe the actual returned
 * records and caller; preserve the original prepend and all device policy. */
#define WriteConsoleInputVDMW observed_WriteConsoleInputVDMW
#include "../../src/mvdm/softpc.new/host/src/nt_event.c"
#undef WriteConsoleInputVDMW
#include <intrin.h>
extern void observed_keyboard_event(LONG kind,const LONG *data,unsigned count);
extern BOOL WINAPI WriteConsoleInputVDMW(HANDLE,PINPUT_RECORD,DWORD,LPDWORD);

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
