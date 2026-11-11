/* Disposable private-desktop research: no product, guest or registry changes.
 * argv[1] is a runtime report path; every observed value is recorded, not
 * asserted as successful merely because the Console API returned success. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>

static void sample(FILE *log,HANDLE output,const char *stage,BOOL result,DWORD error)
{
    CONSOLE_SCREEN_BUFFER_INFO info={0};
    CONSOLE_FONT_INFOEX font={sizeof(font)};
    BOOL a=GetConsoleScreenBufferInfo(output,&info);
    BOOL b=GetCurrentConsoleFontEx(output,FALSE,&font);
    fprintf(log,"%s result=%d error=%lu query=%d/%d buffer=%d,%d view=%d,%d,%d,%d font=%d,%d\n",
        stage,result,error,a,b,info.dwSize.X,info.dwSize.Y,info.srWindow.Left,
        info.srWindow.Top,info.srWindow.Right,info.srWindow.Bottom,
        font.dwFontSize.X,font.dwFontSize.Y);
    fflush(log);
}
static void attempt(FILE *log,HANDLE output,SHORT width,SHORT height)
{
    SMALL_RECT tiny={0,0,0,0},view={0,0,width-1,height-1};
    COORD size={width,height};char label[80];BOOL ok;DWORD error;
    ok=SetConsoleWindowInfo(output,TRUE,&tiny);error=ok?0:GetLastError();
    sample(log,output,"shrink-viewport",ok,error);
    ok=SetConsoleScreenBufferSize(output,size);error=ok?0:GetLastError();
    sprintf_s(label,sizeof(label),"buffer-%dx%d",width,height);sample(log,output,label,ok,error);
    ok=SetConsoleWindowInfo(output,TRUE,&view);error=ok?0:GetLastError();
    sprintf_s(label,sizeof(label),"viewport-%dx%d",width,height);sample(log,output,label,ok,error);
    {
        CONSOLE_SCREEN_BUFFER_INFOEX info={sizeof(info)};
        if(GetConsoleScreenBufferInfoEx(output,&info)) {
            info.dwSize=size;info.srWindow=view;
            ++info.srWindow.Right;++info.srWindow.Bottom;
            info.dwCursorPosition.X=info.dwCursorPosition.Y=0;
            ok=SetConsoleScreenBufferInfoEx(output,&info);error=ok?0:GetLastError();
            sprintf_s(label,sizeof(label),"extended-%dx%d",width,height);sample(log,output,label,ok,error);
            ok=SetConsoleScreenBufferInfoEx(output,&info);error=ok?0:GetLastError();
            sprintf_s(label,sizeof(label),"extended-repeat-%dx%d",width,height);sample(log,output,label,ok,error);
        }
    }
}
int main(int argc,char **argv)
{
    FILE *log;HANDLE output;CONSOLE_FONT_INFOEX font={sizeof(font)};BOOL ok;DWORD error;
    if(argc==2) {
        char name[80],image[MAX_PATH],command[2048];HDESK desktop;
        STARTUPINFOA startup={sizeof(startup)};PROCESS_INFORMATION child={0};DWORD code=90;
        sprintf_s(name,sizeof(name),"NTVDMGeometryProbe-%lu",GetCurrentProcessId());
        desktop=CreateDesktopA(name,NULL,NULL,0,GENERIC_ALL,NULL);if(!desktop)return 91;
        if(!GetModuleFileNameA(NULL,image,sizeof(image)))return 92;
        sprintf_s(command,sizeof(command),"\"%s\" \"%s\" child",image,argv[1]);
        startup.lpDesktop=name;startup.dwFlags=STARTF_USESHOWWINDOW;startup.wShowWindow=SW_HIDE;
        if(CreateProcessA(NULL,command,NULL,NULL,FALSE,CREATE_NEW_CONSOLE,NULL,NULL,&startup,&child)) {
            if(WaitForSingleObject(child.hProcess,15000)==WAIT_OBJECT_0)GetExitCodeProcess(child.hProcess,&code);
            else {TerminateProcess(child.hProcess,93);WaitForSingleObject(child.hProcess,1000);}
            CloseHandle(child.hThread);CloseHandle(child.hProcess);
        }
        CloseDesktop(desktop);return (int)code;
    }
    if(argc!=3 || strcmp(argv[2],"child"))return 94;
    if(fopen_s(&log,argv[1],"w") || !log)return 95;
    output=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    if(output==INVALID_HANDLE_VALUE)return 96;
    sample(log,output,"initial",TRUE,0);
    attempt(log,output,80,25);
    attempt(log,output,80,50);attempt(log,output,120,40);attempt(log,output,20,8);
    if(!GetCurrentConsoleFontEx(output,FALSE,&font))return 97;
    font.dwFontSize.X=1;font.dwFontSize.Y=1;
    ok=SetCurrentConsoleFontEx(output,FALSE,&font);error=ok?0:GetLastError();
    sample(log,output,"fixed-1x1-font",ok,error);
    attempt(log,output,80,25);attempt(log,output,80,50);attempt(log,output,120,40);
    font.dwFontSize.X=2;font.dwFontSize.Y=2;
    ok=SetCurrentConsoleFontEx(output,FALSE,&font);error=ok?0:GetLastError();
    sample(log,output,"fixed-2x2-font",ok,error);
    attempt(log,output,80,25);attempt(log,output,80,50);attempt(log,output,120,40);
    font.dwFontSize.X=2;font.dwFontSize.Y=4;
    ok=SetCurrentConsoleFontEx(output,FALSE,&font);error=ok?0:GetLastError();
    sample(log,output,"fixed-2x4-font",ok,error);
    attempt(log,output,80,25);attempt(log,output,80,50);attempt(log,output,120,40);
    attempt(log,output,20,8);
    CloseHandle(output);return fclose(log)?98:0;
}
