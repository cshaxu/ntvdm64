/* Retail USER default-button geometry, through the actual style DLL. */
#define NTW16S_STATIC
#include "ntw16s-dll/ntw16s.h"
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"DEFAULT FAIL line=%d: %s error=%lu\n",__LINE__,#x,GetLastError());ExitProcess(1);}}while(0)
typedef BOOL (__cdecl *install_fn)(HWND,const win16_style_palette *);
typedef BOOL (__cdecl *remove_fn)(HWND);
static unsigned clicked;
static INT_PTR CALLBACK dialog(HWND w,UINT m,WPARAM a,LPARAM b)
{(void)w;(void)b;if(m==WM_COMMAND){clicked=LOWORD(a);return TRUE;}return FALSE;}
static void paint(HWND w,HDC dc)
{RECT r={0,0,160,50};FillRect(dc,&r,(HBRUSH)GetStockObject(WHITE_BRUSH));SendMessageW(w,WM_PRINTCLIENT,(WPARAM)dc,PRF_CLIENT);GdiFlush();}
static void border(HWND w,HDC dc,int u,BOOL def,BOOL pressed)
{
    int x,b=(def?2:1)*u;paint(w,dc);
    for(x=0;x<b;++x)CHECK(GetPixel(dc,x,25)==RGB(0,0,0));
    CHECK(GetPixel(dc,0,0)==RGB(255,255,255));
    CHECK(GetPixel(dc,b,25)==(pressed?RGB(128,128,128):RGB(255,255,255)));
    CHECK(GetPixel(dc,160-b-1,25)==(pressed?RGB(192,192,192):RGB(128,128,128)));
    CHECK(GetPixel(dc,80,b)==(pressed?RGB(128,128,128):RGB(255,255,255)));
    CHECK(GetPixel(dc,80,50-b-1)==(pressed?RGB(192,192,192):RGB(128,128,128)));
}
int wmain(int argc,WCHAR **argv)
{
    struct {DLGTEMPLATE d;WORD menu,cls,title;} t={0};
    HDESK desktop,original;HMODULE dll;install_fn install;remove_fn remove;
    HWND w,button,other,edit;HDC dc;HBITMAP bitmap,old;MSG msg={0};int u;
    DWORD before,after;WCHAR name[64];int iteration;BOOL diagnostic;
    CHECK(argc==2 || (argc==3 && !wcscmp(argv[2],L"--font-recreation")));
    diagnostic=argc==3;SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    original=GetThreadDesktop(GetCurrentThreadId());swprintf_s(name,64,L"DefaultButton-%lu",GetCurrentProcessId());
    desktop=CreateDesktopW(name,NULL,NULL,0,GENERIC_ALL,NULL);CHECK(desktop && SetThreadDesktop(desktop));
    dll=LoadLibraryExW(argv[1],NULL,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);CHECK(dll);
    install=(install_fn)(void *)GetProcAddress(dll,"win16_style_control_install");
    remove=(remove_fn)(void *)GetProcAddress(dll,"win16_style_control_remove");CHECK(install && remove);
    t.d.style=WS_POPUP|WS_VISIBLE|WS_CAPTION|DS_MODALFRAME;t.d.cx=300;t.d.cy=150;
    w=CreateDialogIndirectParamW(GetModuleHandleW(NULL),&t.d,NULL,dialog,0);CHECK(w);
    button=CreateWindowW(L"BUTTON",L"OK",WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_DEFPUSHBUTTON,10,10,160,50,w,(HMENU)101,NULL,NULL);
    other=CreateWindowW(L"BUTTON",L"Cancel",WS_CHILD|WS_VISIBLE|WS_TABSTOP,180,10,160,50,w,(HMENU)102,NULL,NULL);
    edit=CreateWindowW(L"EDIT",L"Input",WS_CHILD|WS_VISIBLE|WS_TABSTOP,10,80,160,30,w,(HMENU)103,NULL,NULL);
    CHECK(button && other && edit);u=max(1,MulDiv(1,(int)GetDpiForWindow(button),96));
    dc=CreateCompatibleDC(NULL);bitmap=CreateBitmap(160,50,1,32,NULL);CHECK(dc && bitmap);old=SelectObject(dc,bitmap);
    CHECK(install(button,NULL) && install(other,NULL));
    if(diagnostic)goto lifetime;
    SetFocus(edit);CHECK(GetFocus()==edit);border(button,dc,u,TRUE,FALSE);border(other,dc,u,FALSE,FALSE);
    SendMessageW(button,BM_SETSTATE,TRUE,0);CHECK(SendMessageW(button,BM_GETSTATE,0,0)&BST_PUSHED);
    border(button,dc,u,TRUE,TRUE);SendMessageW(button,BM_SETSTATE,FALSE,0);
    EnableWindow(button,FALSE);border(button,dc,u,FALSE,FALSE);EnableWindow(button,TRUE);
    border(button,dc,u,TRUE,FALSE);
    SendMessageW(button,BM_SETSTYLE,BS_PUSHBUTTON,TRUE);border(button,dc,u,FALSE,FALSE);
    SendMessageW(button,BM_SETSTYLE,BS_DEFPUSHBUTTON,TRUE);border(button,dc,u,TRUE,FALSE);
    SetFocus(button);CHECK(SendMessageW(button,BM_GETSTATE,0,0)&BST_FOCUS);paint(button,dc);
    /* Old renderer drew the focus rectangle around the whole face. */
    {int x,y;for(y=12*u;y<15*u && y<40;++y)for(x=6*u;x<9*u;++x)CHECK(GetPixel(dc,x,y)==RGB(192,192,192));}
    SendMessageW(w,DM_SETDEFID,101,0);SetFocus(edit);
    CHECK(LOWORD(SendMessageW(w,DM_GETDEFID,0,0))==101);
    msg.hwnd=edit;msg.message=WM_KEYDOWN;msg.wParam=VK_RETURN;msg.lParam=1;
    CHECK(IsDialogMessageW(w,&msg) && clicked==101);
    SendMessageW(w,DM_SETDEFID,102,0);
    CHECK(LOWORD(SendMessageW(w,DM_GETDEFID,0,0))==102);
    border(button,dc,u,FALSE,FALSE);border(other,dc,u,TRUE,FALSE);
    clicked=0;CHECK(IsDialogMessageW(w,&msg) && clicked==102);
lifetime:
    CHECK(remove(button));
    if(diagnostic)CHECK(remove(other));
    /* Keep the sibling font owner alive, as an attached styled parent does.
     * Separate attachment tests cover last release. */
    if(!diagnostic)for(iteration=0;iteration<30;++iteration){CHECK(install(button,NULL));paint(button,dc);CHECK(remove(button));}
    GdiFlush();before=GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS);
    for(iteration=0;iteration<30;++iteration){CHECK(install(button,NULL));paint(button,dc);CHECK(remove(button));}
    GdiFlush();after=GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS);
    printf("LIFETIME GDI=%lu/%lu\n",before,after);
    if(!diagnostic){CHECK(after==before);CHECK(remove(other));}
    CHECK(DestroyWindow(w));SelectObject(dc,old);CHECK(DeleteObject(bitmap) && DeleteDC(dc));
    CHECK(FreeLibrary(dll));CHECK(SetThreadDesktop(original) && CloseDesktop(desktop));
    if(diagnostic)printf("FONT RECREATION DIAGNOSTIC (not a default-button pass) GDI=%lu/%lu\n",before,after);
    else printf("DEFAULT PASS continuous1/2-unit frame, corners, pressed relief, disabled, focus, BM_SETSTYLE, DM_SETDEFID, Enter, detach/reinstall/unload GDI=%lu/%lu\n",before,after);
    return 0;
}
