/* S34 diagnostic: compare a Console API size with its VT terminal's own
 * response before and after a window-size request. Not a product component. */
#define _WIN32_WINNT 0x0A00
#include <windows.h>
#include <stdio.h>
#include <string.h>

static void query(HANDLE input,HANDLE output,FILE *report,const char *phase)
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    DWORD written,read,until=GetTickCount()+2500;
    char reply[512]={0};
    size_t used=0;
    if(GetConsoleScreenBufferInfo(output,&info))
        fprintf(report,"%s conhost=%d,%d window=%d,%d,%d,%d cursor=%d,%d\n",
            phase,info.dwSize.X,info.dwSize.Y,info.srWindow.Left,info.srWindow.Top,
            info.srWindow.Right,info.srWindow.Bottom,info.dwCursorPosition.X,
            info.dwCursorPosition.Y);
    WriteConsoleA(output,"\x1b[18t\x1b[6n",9,&written,NULL);
    while(GetTickCount()<until && used<sizeof(reply)-1) {
        if(WaitForSingleObject(input,100)!=WAIT_OBJECT_0)continue;
        if(!ReadFile(input,reply+used,(DWORD)(sizeof(reply)-used-1),&read,NULL) || !read)break;
        used+=read;reply[used]=0;
        if(strchr(reply,'t') && strchr(reply,'R'))break;
    }
    fprintf(report,"%s vt-response-bytes=%zu hex=",phase,used);
    for(size_t i=0;i<used;i++)fprintf(report,"%02X",(unsigned char)reply[i]);
    fprintf(report,"\n");fflush(report);
}

int main(int argc,char **argv)
{
    HANDLE input,output;
    DWORD input_mode,output_mode,written;
    FILE *report;
    if((argc!=2 && argc!=3) || fopen_s(&report,argv[1],"wb") || !report)return 64;
    input=CreateFileW(L"CONIN$",GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    output=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    if(input==INVALID_HANDLE_VALUE || output==INVALID_HANDLE_VALUE ||
        !GetConsoleMode(input,&input_mode) || !GetConsoleMode(output,&output_mode))return 65;
    if(!SetConsoleMode(input,(input_mode|ENABLE_VIRTUAL_TERMINAL_INPUT)&
        ~(ENABLE_LINE_INPUT|ENABLE_ECHO_INPUT)) ||
        !SetConsoleMode(output,output_mode|ENABLE_VIRTUAL_TERMINAL_PROCESSING))return 66;
    Sleep(1500);
    query(input,output,report,"before");
    if(argc==3 && !strcmp(argv[2],"shadow")) {
        HANDLE logical=CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,
            FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
        SMALL_RECT logical_window={0,0,79,27},target={0,0,79,27};
        COORD logical_size={80,28},origin={0,0},cursor={9,27};
        CHAR_INFO cells[80*28];
        BOOL ok=logical!=INVALID_HANDLE_VALUE;
        if(ok)ok=SetConsoleWindowInfo(logical,TRUE,&logical_window);
        if(ok)ok=SetConsoleScreenBufferSize(logical,logical_size);
        for(int row=0;row<28;row++)for(int column=0;column<80;column++) {
            CHAR_INFO *cell=&cells[row*80+column];
            cell->Char.AsciiChar=' ';cell->Attributes=7;
        }
        for(int column=0;column<9;column++)
            cells[27*80+column].Char.AsciiChar="O:\\WINNT>"[column];
        /* The previous physical bottom rows must be included in the same
         * presentation transaction, even though the guest cannot address them. */
        if(ok)ok=WriteConsoleOutputA(logical,cells,logical_size,origin,&target);
        target=(SMALL_RECT){0,0,79,27};
        if(ok)ok=WriteConsoleOutputA(output,cells,logical_size,origin,&target);
        if(ok)ok=FillConsoleOutputCharacterA(output,' ',160,(COORD){0,28},&written);
        if(ok)ok=SetConsoleCursorPosition(output,cursor);
        fprintf(report,"shadow-project=%lu error=%lu\n",(unsigned long)ok,
            (unsigned long)(ok ? 0 : GetLastError()));
        if(logical!=INVALID_HANDLE_VALUE)CloseHandle(logical);
    } else if(argc==3 && !strcmp(argv[2],"api")) {
        COORD size={80,28};
        SMALL_RECT window={0,0,79,27};
        BOOL windowed=SetConsoleWindowInfo(output,TRUE,&window);
        BOOL resized=SetConsoleScreenBufferSize(output,size);
        fprintf(report,"api-window=%lu api-resize=%lu error=%lu\n",
            (unsigned long)windowed,(unsigned long)resized,
            (unsigned long)(resized ? 0 : GetLastError()));
    } else {
        WriteConsoleA(output,"\x1b[8;28;80t",11,&written,NULL);
    }
    Sleep(1000);
    query(input,output,report,"after");
    SetConsoleMode(input,input_mode);
    SetConsoleMode(output,output_mode);
    CloseHandle(input);CloseHandle(output);fclose(report);
    Sleep(500);
    return 0;
}
