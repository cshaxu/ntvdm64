/* Diagnostic only: retain the entire selected original callback body and
 * inject at one of its actual resolver edges. Never selected for publication. */
#define wow_original_CallBack16 diagnostic_original_callback
#define GetPModeVDMPointerAssert diagnostic_callback_resolver
#include "../../src/mvdm/wow32/wcall16.c"
#undef wow_original_CallBack16
#undef GetPModeVDMPointerAssert
#include <stdio.h>
extern PVOID FASTCALL GetPModeVDMPointerAssert(DWORD address);
static __declspec(thread) unsigned resolution;
static __declspec(thread) BOOL observing,injected;
static void record_callback(const char *stage,DWORD stack,DWORD cpu,WORD cs,WORD ip)
{
    char path[MAX_PATH],line[256];DWORD count,written,error=GetLastError();HANDLE file;
    count=GetEnvironmentVariableA("MVDM_TEST_SHELL_EXCEPTION",path,sizeof(path));
    if(count && count+6<sizeof(path)){
        strcat_s(path,sizeof(path),".frame");
        file=CreateFileA(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
        if(file!=INVALID_HANDLE_VALUE){
            count=(DWORD)sprintf_s(line,sizeof(line),"%lu %lu %u %s resolve=%u entry=%08lX logical=%08lX cpu=%08lX entrycpu=%08lX cs=%04X/%04X ip=%04X/%04X\r\n",
                GetCurrentProcessId(),GetCurrentThreadId(),CURRENTPTD()->htask16,stage,resolution,
                stack,CURRENTPTD()->vpStack,VDMSTACK(),cpu,getCS(),cs,getIP(),ip);
            WriteFile(file,line,count,&written,NULL);CloseHandle(file);
        }
    }
    SetLastError(error);
}
PVOID FASTCALL diagnostic_callback_resolver(DWORD address)
{
    PVOID result=GetPModeVDMPointerAssert(address);
    if(observing && !injected){
        char phase[24];DWORD count=GetEnvironmentVariableA("MVDM_TEST_CALLBACK_ABORT",phase,sizeof(phase));
        ++resolution;
        if(count && count<sizeof(phase) && resolution==(!strcmp(phase,"Preparation")?2u:3u)){
            injected=TRUE;SetLastError(0x7281);RaiseException(0xe4470004,0,0,NULL);
        }
    }
    return result;
}
BOOL wow_original_CallBack16(INT ret,PPARM16 args,VPPROC proc,PVPVOID output)
{
    BOOL result,selected=FALSE;DWORD entry=0,cpu=0;WORD cs=0,ip=0;char phase[24];
    if(ret==RET_WINEXEC && !observing && !injected &&
            GetEnvironmentVariableA("MVDM_TEST_CALLBACK_ABORT",phase,sizeof(phase))){
        selected=TRUE;entry=CURRENTPTD()->vpStack;cpu=VDMSTACK();cs=getCS();ip=getIP();
        observing=TRUE;resolution=0;record_callback("enter",entry,cpu,cs,ip);
    }
    __try {result=diagnostic_original_callback(ret,args,proc,output);}
    __finally {
        if(selected){
            record_callback(AbnormalTermination()?"unwind":"normal",entry,cpu,cs,ip);
            observing=FALSE;
        }
    }
    return result;
}
