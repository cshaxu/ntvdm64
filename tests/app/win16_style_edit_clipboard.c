/* Clipboard belongs to a private window station, never the interactive user. */
#include <windows.h>
#include <stdio.h>
#include <wchar.h>
#include "win16-style/controls.h"
#define CHECK(x) do { if(!(x)){fprintf(stderr,"EDIT CLIPBOARD FAIL line=%d error=%lu\n",__LINE__,GetLastError());return 1;} } while(0)
static BOOL text_is(HWND w,const WCHAR *expected)
{
    WCHAR text[128];GetWindowTextW(w,text,128);return !wcscmp(text,expected);
}
static BOOL clipboard_is(HWND w,const WCHAR *expected)
{
    HANDLE data;const WCHAR *text;BOOL equal=FALSE;
    if(!OpenClipboard(w))return FALSE;
    data=GetClipboardData(CF_UNICODETEXT);
    text=data?(const WCHAR *)GlobalLock(data):NULL;
    if(text){equal=!wcscmp(text,expected);GlobalUnlock(data);}
    CloseClipboard();return equal;
}
static int exercise(HWND root,BOOL styled,BOOL multiline)
{
    HWND src,dst;HFONT font=(HFONT)GetStockObject(SYSTEM_FONT);
    DWORD style=WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|(multiline?ES_MULTILINE:0);
    src=CreateWindowW(L"EDIT",L"abcdef",style,0,0,260,80,root,NULL,GetModuleHandleW(NULL),NULL);
    dst=CreateWindowW(L"EDIT",L"xy",style,0,90,260,80,root,NULL,GetModuleHandleW(NULL),NULL);
    CHECK(src && dst);
    SendMessageW(src,WM_SETFONT,(WPARAM)font,FALSE);SendMessageW(dst,WM_SETFONT,(WPARAM)font,FALSE);
    if(styled){CHECK(win16_style_edit_install(src,TRUE,NULL));CHECK(win16_style_edit_install(dst,TRUE,NULL));}
    SendMessageW(src,EM_SETSEL,1,4);SendMessageW(src,WM_COPY,0,0);
    CHECK(clipboard_is(root,L"bcd") && text_is(src,L"abcdef"));
    SendMessageW(dst,EM_SETSEL,1,1);SendMessageW(dst,WM_PASTE,0,0);
    CHECK(text_is(dst,L"xbcdy"));CHECK(SendMessageW(dst,EM_UNDO,0,0));CHECK(text_is(dst,L"xy"));
    SendMessageW(src,EM_SETSEL,1,4);SendMessageW(src,WM_CUT,0,0);
    CHECK(text_is(src,L"aef") && clipboard_is(root,L"bcd"));
    CHECK(SendMessageW(src,EM_UNDO,0,0));CHECK(text_is(src,L"abcdef"));
    CHECK(SendMessageW(dst,EM_SETREADONLY,TRUE,0));SendMessageW(dst,EM_SETSEL,0,2);
    SendMessageW(dst,WM_PASTE,0,0);CHECK(text_is(dst,L"xy"));
    SendMessageW(dst,WM_CUT,0,0);CHECK(text_is(dst,L"xy"));
    if(styled){CHECK(win16_style_control_remove(src));CHECK(win16_style_control_remove(dst));
        CHECK((HFONT)SendMessageW(src,WM_GETFONT,0,0)==font);
        CHECK((HFONT)SendMessageW(dst,WM_GETFONT,0,0)==font);}
    CHECK(text_is(src,L"abcdef") && text_is(dst,L"xy"));
    CHECK(DestroyWindow(src));CHECK(DestroyWindow(dst));
    printf("EDIT CLIPBOARD PASS styled=%d multiline=%d copy/paste/cut/undo/readonly/font-restore\n",styled,multiline);
    return 0;
}
int main(void)
{
    WCHAR name[80],actual[80];DWORD needed;HWINSTA station,original;HDESK desk,old_desk;
    HWND root;unsigned styled,multiline;
    swprintf_s(name,80,L"Win16StyleClipboard-%lu",GetCurrentProcessId());
    original=GetProcessWindowStation();old_desk=GetThreadDesktop(GetCurrentThreadId());
    station=CreateWindowStationW(name,0,WINSTA_CREATEDESKTOP|WINSTA_ACCESSCLIPBOARD|
        WINSTA_ACCESSGLOBALATOMS|WINSTA_READATTRIBUTES,NULL);CHECK(station);
    CHECK(SetProcessWindowStation(station));
    CHECK(GetUserObjectInformationW(GetProcessWindowStation(),UOI_NAME,actual,sizeof(actual),&needed));
    CHECK(!wcscmp(name,actual));
    desk=CreateDesktopW(L"ClipboardProbe",NULL,NULL,0,GENERIC_ALL,NULL);CHECK(desk);
    CHECK(SetThreadDesktop(desk));
    wprintf(L"PRIVATE CLIPBOARD STATION %ls\n",actual);
    root=CreateWindowW(L"STATIC",L"Private clipboard fixture",WS_OVERLAPPEDWINDOW,
        0,0,400,300,NULL,NULL,GetModuleHandleW(NULL),NULL);CHECK(root);
    for(styled=0;styled<2;++styled)for(multiline=0;multiline<2;++multiline)
        CHECK(!exercise(root,(BOOL)styled,(BOOL)multiline));
    CHECK(DestroyWindow(root));CHECK(SetThreadDesktop(old_desk));
    CHECK(CloseDesktop(desk));CHECK(SetProcessWindowStation(original));CHECK(CloseWindowStation(station));
    return 0;
}
