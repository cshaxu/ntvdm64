/* Hidden real-Console smoke test for Microsoft's VT-input Edit. */
#define _WIN32_WINNT 0x0A00
#include "ntvwm-exe/console_state.h"
#include <stdio.h>
#include <string.h>

static BOOL snapshot(char rows[12][81])
{
    HANDLE output=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    CONSOLE_SCREEN_BUFFER_INFO info;
    unsigned row;
    if(output==INVALID_HANDLE_VALUE)return FALSE;
    if(!GetConsoleScreenBufferInfo(output,&info)) { CloseHandle(output);return FALSE; }
    for(row=0;row<12;++row) {
        WCHAR characters[80];DWORD count=0;unsigned col;
        COORD origin={info.srWindow.Left,(SHORT)(info.srWindow.Top+row)};
        if(!ReadConsoleOutputCharacterW(output,characters,80,origin,&count) || count!=80)
            {CloseHandle(output);return FALSE;}
        for(col=0;col<80;++col)rows[row][col]=
            characters[col]>=32 && characters[col]<127 ? (char)characters[col] : ' ';
        rows[row][80]=0;
    }
    CloseHandle(output);return TRUE;
}
static BOOL contains(char rows[12][81],const char *needle)
{
    unsigned row;
    for(row=0;row<12;++row)if(strstr(rows[row],needle))return TRUE;
    return FALSE;
}
int main(int argc,char **argv)
{
    PROCESS_INFORMATION child={0};STARTUPINFOW startup={sizeof(startup)};
    HANDLE input=INVALID_HANDLE_VALUE;DWORD mode=0,accepted=0,error=1;unsigned attempt,row;
    INPUT_RECORD mouse={0},native[2];char before[12][81],raw[12][81],after[12][81];
    WCHAR command[MAX_PATH];DWORD length;
    FILE *report=argc==2 ? fopen(argv[1],"wb") : stdout;
    if(!report)return 8;
    length=GetSystemDirectoryW(command,MAX_PATH);
    if(!length || length>=MAX_PATH-10 ||
        wcscat_s(command,MAX_PATH,L"\\edit.exe")){error=11;goto done;}
    if(GetFileAttributesW(command)==INVALID_FILE_ATTRIBUTES) {
        length=GetWindowsDirectoryW(command,MAX_PATH);
        if(!length || length>=MAX_PATH-10 ||
            wcscat_s(command,MAX_PATH,L"\\edit.exe")){error=11;goto done;}
    }
    input=CreateFileW(L"CONIN$",GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    if(input==INVALID_HANDLE_VALUE){error=2;goto done;}
    if(!CreateProcessW(command,command,NULL,NULL,FALSE,0,NULL,NULL,&startup,&child))
        {error=GetLastError();goto done;}
    for(attempt=0;attempt<100;++attempt) {
        if(GetConsoleMode(input,&mode) && (mode&ENABLE_VIRTUAL_TERMINAL_INPUT) && snapshot(before))break;
        if(WaitForSingleObject(child.hProcess,0)==WAIT_OBJECT_0)break;
        Sleep(100);
    }
    if(attempt==100 || !(mode&ENABLE_VIRTUAL_TERMINAL_INPUT)) {error=3;goto done;}
    fprintf(report,"BEFORE:\n");for(row=0;row<12;++row)fprintf(report,"%02u %s\n",row,before[row]);
    mouse.EventType=MOUSE_EVENT;
    mouse.Event.MouseEvent.dwMousePosition.X=3;
    mouse.Event.MouseEvent.dwMousePosition.Y=0;
    mouse.Event.MouseEvent.dwButtonState=FROM_LEFT_1ST_BUTTON_PRESSED;
    native[0]=mouse;native[1]=mouse;native[1].Event.MouseEvent.dwButtonState=0;
    if(!WriteConsoleInputW(input,native,2,&accepted) || accepted!=2)
        {error=9;goto done;}
    Sleep(250);
    if(!snapshot(raw) || contains(before,"New File") || contains(raw,"New File"))
        {error=10;goto done;}
    fprintf(report,"NATIVE-MOUSE-RECORD ignored by VT Edit\n");
    if(ntvwm_input_write(input,&mouse,1,&accepted) || accepted!=1){error=4;goto done;}
    mouse.Event.MouseEvent.dwButtonState=0;
    if(ntvwm_input_write(input,&mouse,1,&accepted) || accepted!=1){error=5;goto done;}
    Sleep(500);
    if(!snapshot(after)){error=6;goto done;}
    fprintf(report,"AFTER:\n");for(row=0;row<12;++row)fprintf(report,"%02u %s\n",row,after[row]);
    error=contains(after,"New File") ? 0 : 7;
done:
    if(child.hProcess) {
        if(WaitForSingleObject(child.hProcess,0)==WAIT_TIMEOUT)TerminateProcess(child.hProcess,0);
        WaitForSingleObject(child.hProcess,3000);
        CloseHandle(child.hThread);CloseHandle(child.hProcess);
    }
    if(input!=INVALID_HANDLE_VALUE)CloseHandle(input);
    fprintf(report,"EDIT-VT-MOUSE exit=%lu mode=%08lx\n",error,mode);
    if(report!=stdout)fclose(report);
    return (int)error;
}
