/* Snapshot an owner's still-running Console after a reported native/DOS
 * handoff. Optional refresh-last-row rewrites exactly the existing cells. */
#define _WIN32_WINNT 0x0601
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

int main(int argc,char **argv)
{
    HANDLE output;
    CONSOLE_SCREEN_BUFFER_INFO info;
    DWORD *members=NULL,count=0,read=0;
    WCHAR *cells=NULL;
    CHAR_INFO *last_row=NULL;
    FILE *report=NULL;
    unsigned long pid;
    char *end;
    int result=1;
    size_t total,row,column;
    if(argc!=3 && (argc!=4 || (strcmp(argv[3],"refresh-last-row") &&
        strcmp(argv[3],"invalidate-last-row") && strcmp(argv[3],"marker-last-row") &&
        strcmp(argv[3],"vt-clear-replay") && strcmp(argv[3],"vt-refresh") &&
        strcmp(argv[3],"vt-erase-prompt-tail") &&
        strcmp(argv[3],"vt-scroll-region-28") &&
        strcmp(argv[3],"vt-resize-28") && strcmp(argv[3],"vt-resize-30") &&
        strcmp(argv[3],"buffer-toggle"))))return 64;
    pid=strtoul(argv[1],&end,10);
    if(!pid || *end)return 64;
    FreeConsole();
    if(!AttachConsole((DWORD)pid))return 65;
    output=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
        NULL,OPEN_EXISTING,0,NULL);
    if(output==INVALID_HANDLE_VALUE)return 66;
    if(!GetConsoleScreenBufferInfo(output,&info) ||
        info.dwSize.X<=0 || info.dwSize.Y<=0)goto done;
    total=(size_t)info.dwSize.X*(size_t)info.dwSize.Y;
    if(total>1000000 || total>SIZE_MAX/sizeof(WCHAR))goto done;
    cells=(WCHAR *)calloc(total,sizeof(*cells));
    members=(DWORD *)calloc(1024,sizeof(*members));
    if(!cells || !members)goto done;
    count=GetConsoleProcessList(members,1024);
    if(!ReadConsoleOutputCharacterW(output,cells,(DWORD)total,(COORD){0,0},&read) ||
        read!=total)goto done;
    if(fopen_s(&report,argv[2],"wb") || !report)goto done;
    fprintf(report,"buffer=%d,%d window=%d,%d,%d,%d cursor=%d,%d members=%lu console-window-visible=%d\n",
        info.dwSize.X,info.dwSize.Y,info.srWindow.Left,info.srWindow.Top,
        info.srWindow.Right,info.srWindow.Bottom,info.dwCursorPosition.X,
        info.dwCursorPosition.Y,(unsigned long)count,
        IsWindowVisible(GetConsoleWindow())!=FALSE);
    for(row=0;row<count && row<1024;++row)
        fprintf(report,"member=%lu\n",(unsigned long)members[row]);
    for(row=0;row<(size_t)info.dwSize.Y;++row) {
        fprintf(report,"%04zu|",row);
        for(column=0;column<(size_t)info.dwSize.X;++column) {
            WCHAR c=cells[row*(size_t)info.dwSize.X+column];
            fputc(c>=32 && c<127 ? (int)c : ' ',report);
        }
        fputc('\n',report);
    }
    if(argc==4 && !strcmp(argv[3],"buffer-toggle")) {
        HANDLE temporary=CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,
            FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
        if(temporary==INVALID_HANDLE_VALUE)goto done;
        if(!SetConsoleActiveScreenBuffer(temporary)) {
            CloseHandle(temporary);goto done;
        }
        if(!SetConsoleActiveScreenBuffer(output)) {
            CloseHandle(temporary);goto done;
        }
        CloseHandle(temporary);
        fprintf(report,"buffer-toggle=1\n");
    } else if(argc==4 && !strcmp(argv[3],"vt-scroll-region-28")) {
        DWORD mode,written;
        COORD position=info.dwCursorPosition;
        if(!GetConsoleMode(output,&mode) ||
            !SetConsoleMode(output,mode|ENABLE_VIRTUAL_TERMINAL_PROCESSING) ||
            !(GetEnvironmentVariableA("MVDM_TEST_S34_MARGIN_WRITEFILE",NULL,0) ?
                WriteFile(output,"\x1b[1;28r",7,&written,NULL) :
                WriteConsoleA(output,"\x1b[1;28r",7,&written,NULL)) || written!=7) {
            SetConsoleMode(output,mode);goto done;
        }
        SetConsoleMode(output,mode);
        if(!SetConsoleCursorPosition(output,position))goto done;
        fprintf(report,"vt-scroll-region-28=1\n");
    } else if(argc==4 && !strcmp(argv[3],"vt-erase-prompt-tail")) {
        DWORD mode,written;
        COORD position=info.dwCursorPosition;
        if(!GetConsoleMode(output,&mode) ||
            !SetConsoleMode(output,mode|ENABLE_VIRTUAL_TERMINAL_PROCESSING) ||
            !WriteConsoleA(output,"\x1b[K",3,&written,NULL) || written!=3) {
            SetConsoleMode(output,mode);goto done;
        }
        SetConsoleMode(output,mode);
        if(!SetConsoleCursorPosition(output,position))goto done;
        fprintf(report,"vt-erase-prompt-tail=1\n");
    } else if(argc==4 && (!strcmp(argv[3],"vt-refresh") ||
        !strcmp(argv[3],"vt-resize-28") || !strcmp(argv[3],"vt-resize-30"))) {
        DWORD mode,done;
        const char *sequence=!strcmp(argv[3],"vt-refresh") ?
            "\x1b[7t" : !strcmp(argv[3],"vt-resize-30") ?
            "\x1b[8;30;80t" : "\x1b[8;28;80t";
        DWORD length=(DWORD)strlen(sequence);
        if(!GetConsoleMode(output,&mode))goto done;
        if(!SetConsoleMode(output,mode|ENABLE_VIRTUAL_TERMINAL_PROCESSING) ||
            !WriteConsoleA(output,sequence,length,&done,NULL) || done!=length) {
            SetConsoleMode(output,mode);goto done;
        }
        SetConsoleMode(output,mode);
        fprintf(report,"vt-request=%s\n",argv[3]);
    } else if(argc==4 && !strcmp(argv[3],"vt-clear-replay")) {
        COORD span=info.dwSize,origin={0,0};
        SMALL_RECT rect={0,0,span.X-1,span.Y-1};
        CHAR_INFO *all=(CHAR_INFO *)calloc(total,sizeof(*all));
        DWORD mode,done;
        if(!all || !ReadConsoleOutputW(output,all,span,origin,&rect) ||
            !GetConsoleMode(output,&mode)) { free(all);goto done; }
        if(!SetConsoleMode(output,mode|ENABLE_VIRTUAL_TERMINAL_PROCESSING) ||
            !WriteConsoleA(output,"\x1b[H\x1b[2J",7,&done,NULL) || done!=7) {
            SetConsoleMode(output,mode);free(all);goto done;
        }
        rect.Left=rect.Top=0;rect.Right=span.X-1;rect.Bottom=span.Y-1;
        if(!WriteConsoleOutputW(output,all,span,origin,&rect)) {
            SetConsoleMode(output,mode);free(all);goto done;
        }
        SetConsoleCursorPosition(output,info.dwCursorPosition);
        SetConsoleMode(output,mode);
        free(all);
        fprintf(report,"vt-clear-replay=1\n");
    } else if(argc==4) {
        COORD span={info.dwSize.X,1},origin={0,0};
        SMALL_RECT rect={0,info.dwSize.Y-1,info.dwSize.X-1,info.dwSize.Y-1};
        CHAR_INFO *temporary=NULL;
        last_row=(CHAR_INFO *)calloc((size_t)span.X,sizeof(*last_row));
        if(!last_row || !ReadConsoleOutputW(output,last_row,span,origin,&rect))goto done;
        if(!strcmp(argv[3],"invalidate-last-row") || !strcmp(argv[3],"marker-last-row")) {
            temporary=(CHAR_INFO *)malloc((size_t)span.X*sizeof(*temporary));
            if(!temporary)goto done;
            memcpy(temporary,last_row,(size_t)span.X*sizeof(*temporary));
            if(!strcmp(argv[3],"marker-last-row")) {
                static const WCHAR marker[]=L"<S34-CONSOLE-CELL-PROBE>";
                for(column=0;column<sizeof(marker)/sizeof(marker[0])-1 &&
                    column+10<(size_t)span.X;++column)
                    temporary[column+10].Char.UnicodeChar=marker[column];
            } else for(column=0;column<(size_t)span.X;++column)
                temporary[column].Attributes^=BACKGROUND_INTENSITY;
            rect.Left=0;rect.Right=info.dwSize.X-1;
            rect.Top=rect.Bottom=info.dwSize.Y-1;
            if(!WriteConsoleOutputW(output,temporary,span,origin,&rect)) {
                free(temporary);goto done;
            }
            if(!strcmp(argv[3],"marker-last-row"))Sleep(5000);
            free(temporary);
        }
        rect.Left=0;rect.Right=info.dwSize.X-1;
        rect.Top=rect.Bottom=info.dwSize.Y-1;
        if(!WriteConsoleOutputW(output,last_row,span,origin,&rect))goto done;
        fprintf(report,"rewrote-last-row=1\n");
    }
    result=ferror(report) ? 1 : 0;
done:
    if(report)fclose(report);
    free(last_row);free(members);free(cells);CloseHandle(output);FreeConsole();
    return result;
}
