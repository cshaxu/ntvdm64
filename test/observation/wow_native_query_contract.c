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
static void property_and_regions(HWND full, BOOL alive)
{
    HWND forms[4]={full,(HWND)(ULONG_PTR)LOWORD(full),
        (HWND)(INT_PTR)(SHORT)LOWORD(full),(HWND)(ULONG_PTR)(0xffff0000u|LOWORD(full))};
    unsigned i;
    for(i=0;i<4;++i){
        HANDLE value=(HANDLE)(ULONG_PTR)0xfedcba98u;
        ATOM atom=GlobalAddAtomA("T483-atom-property");
        RECT box={1,2,10,12},before={0},after={0};
        HRGN region=CreateRectRgn(1,2,10,12);
        HDC dc=CreateCompatibleDC(NULL);
        BOOL set,invr,valr;HANDLE get,removed;int clip;unsigned count;
        callbacks=0;
        set=SetPropA(forms[i],"T483-property",value);
        get=GetPropA(forms[i],"T483-property");
        removed=RemovePropA(forms[i],"T483-property");
        count=callbacks;
        printf("PROP live=%d form=%u set=%d get=%p removed=%p callbacks=%u\n",alive,i,set,get,removed,count);
        CHECK(set==alive);CHECK(get==(alive?value:NULL));CHECK(removed==(alive?value:NULL));CHECK(count==0);
        CHECK(atom!=0);
        callbacks=0;
        CHECK(SetPropA(forms[i],MAKEINTATOM(atom),value)==alive);
        CHECK(GetPropA(forms[i],MAKEINTATOM(atom))==(alive?value:NULL));
        CHECK(RemovePropA(forms[i],MAKEINTATOM(atom))==(alive?value:NULL));
        CHECK(callbacks==0);
        GlobalDeleteAtom(atom);
        callbacks=0;
        if (alive) InvalidateRect(full,&box,FALSE);
        GetUpdateRect(full,&before,FALSE);
        if (alive) ValidateRect(full,NULL);
        GetUpdateRect(full,&after,FALSE);
        invr=InvalidateRgn(forms[i],region,FALSE);
        SelectClipRgn(dc,region);
        clip=ExcludeUpdateRgn(dc,forms[i]);
        valr=ValidateRgn(forms[i],NULL);
        count=callbacks;
        printf("REGION live=%d form=%u invr=%d valr=%d clip=%d before=%ld,%ld,%ld,%ld after=%ld,%ld,%ld,%ld callbacks=%u\n",alive,i,invr,valr,clip,before.left,before.top,before.right,before.bottom,after.left,after.top,after.right,after.bottom,count);
        CHECK(invr==alive);CHECK(valr==alive);CHECK((clip!=ERROR)==alive);CHECK(count==0);
        callbacks=0;
        CHECK(InvalidateRgn(forms[i],region,TRUE)==alive);
        CHECK(callbacks==0);
        CHECK(ValidateRgn(forms[i],region)==alive);
        CHECK(callbacks==0);
        DeleteObject(region);DeleteDC(dc);
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
    property_and_regions(child,TRUE);
    CHECK(DestroyWindow(parent));
    queries(child,FALSE,name);
    property_and_regions(child,FALSE);
    CHECK(UnregisterClassA(name,cls.hInstance));
    CHECK(SetThreadDesktop(old));CHECK(CloseDesktop(desktop));
    printf("WOW_NATIVE_QUERY_CONTRACT checks=%u errors=%u rows=8\n",checks,errors);
    return errors?1:0;
}
