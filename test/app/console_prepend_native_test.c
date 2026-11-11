/* Historical Console ABI availability probe; run only in a disposable
 * private-desktop Console. No product binding or fallback is selected here. */
#include <windows.h>
#include <stdio.h>

typedef BOOL (WINAPI *write_vdm_input)(HANDLE,PINPUT_RECORD,DWORD,LPDWORD);

int main(void)
{
    HANDLE input=GetStdHandle(STD_INPUT_HANDLE);
    HMODULE kernel=GetModuleHandleW(L"kernel32.dll");
    write_vdm_input prepend=(write_vdm_input)GetProcAddress(kernel,"WriteConsoleInputVDMW");
    INPUT_RECORD tail[2]={0},head={0},actual[4]={0};
    DWORD written=0,read=0,error;
    BOOL result;
    unsigned i;
    if (!prepend) { printf("UNAVAILABLE: WriteConsoleInputVDMW error=%lu\n",GetLastError());return 2; }
    if (!FlushConsoleInputBuffer(input)) { printf("FAIL flush=%lu\n",GetLastError());return 1; }
    for (i=0;i<2;++i) {
        tail[i].EventType=KEY_EVENT;
        tail[i].Event.KeyEvent.bKeyDown=TRUE;
        tail[i].Event.KeyEvent.wRepeatCount=1;
        tail[i].Event.KeyEvent.wVirtualKeyCode=(WORD)('B'+i);
        tail[i].Event.KeyEvent.uChar.UnicodeChar=(WCHAR)('B'+i);
    }
    head=tail[0];head.Event.KeyEvent.wVirtualKeyCode='A';
    head.Event.KeyEvent.uChar.UnicodeChar=L'A';
    if (!WriteConsoleInputW(input,tail,2,&written) || written!=2) {
        printf("FAIL seed=%lu count=%lu\n",GetLastError(),written);return 1;
    }
    SetLastError(0);written=0;
    result=prepend(input,&head,1,&written);error=GetLastError();
    if (!PeekConsoleInputW(input,actual,4,&read)) { printf("FAIL peek=%lu\n",GetLastError());return 1; }
    printf("historical-prepend result=%d error=%lu written=%lu ready=%lu order=",result,error,written,read);
    for (i=0;i<read;++i) printf("%04x ",actual[i].Event.KeyEvent.uChar.UnicodeChar);
    puts("");
    if (!FlushConsoleInputBuffer(input)) return 1;
    if (!result) {
        if (written || read!=2 || actual[0].Event.KeyEvent.uChar.UnicodeChar!=L'B' ||
                actual[1].Event.KeyEvent.uChar.UnicodeChar!=L'C') return 1;
        puts("UNAVAILABLE: original prepend rejected; existing input preserved");return 2;
    }
    if (written!=1 || read!=3 || actual[0].Event.KeyEvent.uChar.UnicodeChar!=L'A' ||
            actual[1].Event.KeyEvent.uChar.UnicodeChar!=L'B' ||
            actual[2].Event.KeyEvent.uChar.UnicodeChar!=L'C') return 1;
    puts("PASS: native historical VDM input prepends ahead of existing Console records");
    return 0;
}
