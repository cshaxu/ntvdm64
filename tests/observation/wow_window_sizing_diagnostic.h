/* Diagnostic-only header: inserted by the sizing build recipe, never shipped. */
static void sizing_witness(const char *stage, HWND window, UINT message, LPARAM lp)
{
    DWORD saved=GetLastError(),n,written; HANDLE file; char path[MAX_PATH],line[512];
    long x=0,y=0,a=0,b=0; RECT r={0};
    if(message && message!=WM_GETMINMAXINFO && message!=WM_WINDOWPOSCHANGING && message!=WM_WINDOWPOSCHANGED)return;
    if(!GetEnvironmentVariableA("MVDM_SIZING_TRACE",path,sizeof(path))){SetLastError(saved);return;}
    if(!message && lp){const CREATESTRUCTA *p=(const CREATESTRUCTA*)lp;x=p->cx;y=p->cy;a=p->style;b=p->dwExStyle;}
    if(message==WM_GETMINMAXINFO && lp){const MINMAXINFO *p=(const MINMAXINFO*)lp;x=p->ptMinTrackSize.x;y=p->ptMinTrackSize.y;a=p->ptMaxTrackSize.x;b=p->ptMaxTrackSize.y;}
    if((message==WM_WINDOWPOSCHANGING || message==WM_WINDOWPOSCHANGED) && lp){const WINDOWPOS *p=(const WINDOWPOS*)lp;x=p->cx;y=p->cy;a=p->flags;}
    if(window)GetWindowRect(window,&r);
    n=(DWORD)sprintf_s(line,sizeof(line),"%s hwnd=%p msg=%04X values=%ld,%ld,%ld,%ld outer=%ld,%ld metrics=%d,%d tid=%lu\r\n",stage,window,message,x,y,a,b,r.right-r.left,r.bottom-r.top,GetSystemMetrics(SM_CXMINTRACK),GetSystemMetrics(SM_CYMINTRACK),GetCurrentThreadId());
    file=CreateFileA(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    if(file!=INVALID_HANDLE_VALUE){if(n)WriteFile(file,line,n,&written,NULL);CloseHandle(file);}
    SetLastError(saved);
}
