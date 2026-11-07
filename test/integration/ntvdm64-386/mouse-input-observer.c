/* Diagnostic pass-through of the selected original input loop. */
#define ntvdm_console_read_pc_input observed_pc_input
#include "mvdm-host/softpc.new/host/src/nt_event.c"
#undef ntvdm_console_read_pc_input
extern BOOL ntvdm_console_read_pc_input(HANDLE,PINPUT_RECORD,DWORD,LPDWORD,USHORT);
extern void test_mouse_record(DWORD,const MOUSE_EVENT_RECORD *,DWORD,DWORD);
BOOL observed_pc_input(HANDLE handle,PINPUT_RECORD records,DWORD capacity,LPDWORD count,USHORT flags)
{
    BOOL result=ntvdm_console_read_pc_input(handle,records,capacity,count,flags);
    DWORD error=GetLastError(),i;
    if(result)for(i=0;i<*count;++i)
        if(records[i].EventType==MOUSE_EVENT)
            test_mouse_record(GetCurrentProcessId(),&records[i].Event.MouseEvent,
                sc.PC_W_Width,sc.PC_W_Height);
    SetLastError(error);
    return result;
}
