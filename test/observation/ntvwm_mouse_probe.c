/* Native target of run16, not a backend replacement. The private-desktop
 * observer supplies Window events through its existing library-output hook. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
int main(void)
{
    HANDLE input=GetStdHandle(STD_INPUT_HANDLE),output=GetStdHandle(STD_OUTPUT_HANDLE);
    SMALL_RECT view={0,0,79,49};COORD size={80,50};DWORD mode,count;
    BOOL moved=FALSE,pressed=FALSE;ULONGLONG deadline=GetTickCount64()+25000;
    if(!GetConsoleMode(input,&mode) ||
        !SetConsoleMode(input,ENABLE_MOUSE_INPUT|ENABLE_WINDOW_INPUT|ENABLE_EXTENDED_FLAGS) ||
        !SetConsoleScreenBufferSize(output,size) || !SetConsoleWindowInfo(output,TRUE,&view))return 2;
    puts("S7_TEXT_CURSOR_READY");fflush(stdout);
    while(GetTickCount64()<deadline) {
        INPUT_RECORD event;
        if(WaitForSingleObject(input,100)!=WAIT_OBJECT_0)continue;
        if(!ReadConsoleInputW(input,&event,1,&count) || count!=1)return 3;
        if(event.EventType!=MOUSE_EVENT)continue;
        if(event.Event.MouseEvent.dwMousePosition.X<0 || event.Event.MouseEvent.dwMousePosition.X>=80 ||
            event.Event.MouseEvent.dwMousePosition.Y<0 || event.Event.MouseEvent.dwMousePosition.Y>=50)return 4;
        if(!pressed && event.Event.MouseEvent.dwEventFlags==MOUSE_MOVED) {
            if(event.Event.MouseEvent.dwButtonState)return 5;
            moved=TRUE;
        }
        if(event.Event.MouseEvent.dwButtonState&FROM_LEFT_1ST_BUTTON_PRESSED) {
            if(!moved)return 6;
            pressed=TRUE;
        } else if(pressed) {
            puts("NTVWM-MOUSE-PASS 80x50 movement-before-click release native-records");
            fflush(stdout);SetConsoleMode(input,mode);return 0;
        }
    }
    puts("NTVWM-MOUSE-FAIL timeout");fflush(stdout);return 7;
}
