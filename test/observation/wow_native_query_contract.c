/* Native query contract for the original WOW HWND32 input. No product hooks,
 * guest execution, host desktop interaction or timing-based acceptance. */
#include <windows.h>
#include <stdio.h>
#include <string.h>
static unsigned callbacks, errors, checks;
#define CHECK(x) do { ++checks; if (!(x)) { ++errors; printf("FAIL line=%d expression=%s error=%lu\n",__LINE__,#x,GetLastError()); } } while (0)
static LRESULT CALLBACK window_proc(HWND w,UINT m,WPARAM p,LPARAM l)
{ ++callbacks;return DefWindowProcA(w,m,p,l); }
static void queries(HWND full,BOOL alive,const char *class_name)
{
    HWND forms[4]={full,(HWND)(ULONG_PTR)LOWORD(full),
        (HWND)(INT_PTR)(SHORT)LOWORD(full),(HWND)(ULONG_PTR)(0xffff0000u|LOWORD(full))};
    RECT expected_client={0},expected_window={0},rect;
    unsigned i;
    if(alive){CHECK(GetClientRect(full,&expected_client));CHECK(GetWindowRect(full,&expected_window));}
    for(i=0;i<4;++i){
        char name[80]={0};BOOL result;int count;DWORD process=0,thread,error;
        callbacks=0;SetLastError(0);memset(&rect,0xcc,sizeof(rect));
        result=GetClientRect(forms[i],&rect);error=GetLastError();
        CHECK(result==alive);CHECK(callbacks==0);
        if(alive)CHECK(!memcmp(&rect,&expected_client,sizeof(rect)));
        else CHECK(error==ERROR_INVALID_WINDOW_HANDLE);
        callbacks=0;SetLastError(0);memset(&rect,0xcc,sizeof(rect));
        result=GetWindowRect(forms[i],&rect);error=GetLastError();
        CHECK(result==alive);CHECK(callbacks==0);
        if(alive)CHECK(!memcmp(&rect,&expected_window,sizeof(rect)));
        else CHECK(error==ERROR_INVALID_WINDOW_HANDLE);
        callbacks=0;SetLastError(0);
        count=GetClassNameA(forms[i],name,sizeof(name));error=GetLastError();
        CHECK(callbacks==0);
        if(alive){CHECK(count==(int)strlen(class_name));CHECK(!strcmp(name,class_name));}
        else {CHECK(count==0);CHECK(error==ERROR_INVALID_WINDOW_HANDLE);}
        /* Negative control: success of these three queries is not authority
         * to remove the separately required thread-identity adaptation. */
        SetLastError(0);thread=GetWindowThreadProcessId(forms[i],&process);error=GetLastError();
        if(alive && !i){CHECK(thread==GetCurrentThreadId());CHECK(process==GetCurrentProcessId());}
        else {CHECK(!thread);CHECK(error==ERROR_INVALID_WINDOW_HANDLE);}
        printf("QUERY_ROW live=%d form=%u errors=%u\n",alive,i,errors);
    }
}
int main(void)
{
    HDESK old=GetThreadDesktop(GetCurrentThreadId()),desktop;
    WNDCLASSA cls={0};HWND parent,child;char name[64];
    sprintf_s(name,sizeof(name),"T483-query-%lu",GetCurrentProcessId());
    desktop=CreateDesktopA(name,NULL,NULL,0,GENERIC_ALL,NULL);
    if(!desktop||!SetThreadDesktop(desktop))return 2;
    cls.lpfnWndProc=window_proc;cls.hInstance=GetModuleHandleA(NULL);cls.lpszClassName=name;
    if(!RegisterClassA(&cls))return 3;
    parent=CreateWindowExA(0,name,"parent",WS_OVERLAPPEDWINDOW,20,30,240,180,NULL,NULL,cls.hInstance,NULL);
    child=CreateWindowExA(0,name,"child",WS_CHILD,3,7,40,50,parent,(HMENU)123,cls.hInstance,NULL);
    if(!parent||!child)return 4;
    CHECK(HIWORD(child)!=0 && HIWORD(child)!=0xffffu);
    queries(child,TRUE,name);
    CHECK(DestroyWindow(parent));
    queries(child,FALSE,name);
    CHECK(UnregisterClassA(name,cls.hInstance));
    CHECK(SetThreadDesktop(old));CHECK(CloseDesktop(desktop));
    printf("WOW_NATIVE_QUERY_CONTRACT checks=%u errors=%u rows=8\n",checks,errors);
    return errors?1:0;
}
