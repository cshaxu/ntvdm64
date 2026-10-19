/* Test-only identity witness. No Hook, helper window or product state. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
typedef struct { WORD short_handle; HWND full; } identity;
static __declspec(thread) identity captured;
static BOOL CALLBACK find_child(HWND window, LPARAM parameter)
{
    identity *value=(identity *)parameter;
    if(LOWORD((ULONG_PTR)window)!=value->short_handle)return TRUE;
    value->full=window;return FALSE;
}
static BOOL CALLBACK find_root(HWND window, LPARAM parameter)
{
    identity *value=(identity *)parameter;
    if(!find_child(window,parameter))return FALSE;
    EnumChildWindows(window,find_child,parameter);
    return !value->full;
}
static HWND full_handle(WORD value)
{
    identity found={value,NULL};
    EnumThreadWindows(GetCurrentThreadId(),find_root,(LPARAM)&found);
    if(!found.full)EnumDesktopWindows(GetThreadDesktop(GetCurrentThreadId()),find_root,(LPARAM)&found);
    return found.full;
}
__declspec(dllexport) DWORD WINAPI CommonDialogIdentity(DWORD request)
{
    DWORD record[16]={0x31444943}, written=0, result=0, operation=HIWORD(request);
    WORD short_handle=LOWORD(request);
    DWORD owner=0;
    HWND current=full_handle(short_handle);
    HANDLE file;
    record[1]=operation;record[2]=short_handle;record[3]=(DWORD)current;
    record[4]=(DWORD)captured.full;record[5]=IsWindow(captured.full);
    record[6]=IsWindow(current);record[7]=GetCurrentProcessId();
    record[8]=GetCurrentThreadId();
    if(current)record[9]=GetWindowThreadProcessId(current,&owner);
    record[10]=owner;
    if(operation==1){
        if(current && LOWORD((ULONG_PTR)current)==short_handle &&
                record[6] && owner==GetCurrentProcessId() && record[9]==GetCurrentThreadId()){
            captured.short_handle=short_handle;captured.full=current;result=0x447a0001;
        }
    }else if((operation==2 || operation==3) && captured.full &&
            captured.short_handle==short_handle && !record[5]){
        /* An observed new full generation is required before accepting a
         * guest TRUE. Missing/racing evidence remains a test failure. */
        if(operation==2 || (current && current!=captured.full && record[6])){
            result=0x447a0000|operation;
            captured.full=NULL;
        }
    }
    record[11]=result;
    file=CreateFileA("CDLID.BIN",FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,
        NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    if(file==INVALID_HANDLE_VALUE)return 0;
    if(!WriteFile(file,record,sizeof(record),&written,NULL) || written!=sizeof(record))result=0;
    CloseHandle(file);
    return result;
}
#ifdef COMMON_IDENTITY_STANDALONE
int main(void)
{
    HWND window=CreateWindowExA(0,"STATIC","identity control",WS_POPUP,
        0,0,80,40,NULL,NULL,GetModuleHandleA(NULL),NULL);
    DWORD request;
    if(!window)return 1;
    request=LOWORD((ULONG_PTR)window);
    if(CommonDialogIdentity(0x10000|request)!=0x447a0001)return 2;
    if(CommonDialogIdentity(0x20000|request)!=0 || !IsWindow(window))return 3;
    if(CommonDialogIdentity(0x20000|(request^1))!=0)return 4;
    if(!DestroyWindow(window))return 5;
    if(CommonDialogIdentity(0x20000|request)!=0x447a0002)return 6;
    puts("COMMON_IDENTITY omitted_destroy=rejected wrong_identity=rejected real_destroy=accepted");
    return 0;
}
#endif
