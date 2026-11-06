#include "precomp.h"
#include <stdio.h>
/* Calls selected original hook table/cleanup bodies and real USER hooks.
 * Native callbacks are test-owned; this is not a guest callback substitute. */
static unsigned errors, calls_a, calls_b;
#define CHECK(x) do { if(!(x)){++errors;fprintf(stderr,"FAIL line=%d error=%lu\n",__LINE__,GetLastError());} } while(0)
static LRESULT CALLBACK on_a(int code, WPARAM wp, LPARAM lp)
{ if(code==HCBT_CREATEWND)++calls_a; return CallNextHookEx(NULL,code,wp,lp); }
static LRESULT CALLBACK on_b(int code, WPARAM wp, LPARAM lp)
{ if(code==HCBT_CREATEWND)++calls_b; return CallNextHookEx(NULL,code,wp,lp); }
static void create_one(void)
{
 HWND w=CreateWindowExA(0,"STATIC","hook cleanup test",WS_POPUP,0,0,10,10,NULL,NULL,NULL,NULL);
 CHECK(w!=NULL);if(w)CHECK(DestroyWindow(w));
}
static HOOKSTATEDATA register_slot(INT task, HHOOK hook)
{
 HOOKSTATEDATA slot={0};
 CHECK(W32GetNotInUseHookStateData(&slot));
 slot.InUse=TRUE;slot.TaskId=task;slot.iHook=WH_CBT;slot.hHook=hook;
 CHECK(W32SetHookStateData(&slot));return slot;
}
int __cdecl main(void)
{
 typedef HHOOK (WINAPI *set_hook_fn)(int,HOOKPROC,HINSTANCE,DWORD);
 set_hook_fn install=(set_hook_fn)GetProcAddress(GetModuleHandleA("user32.dll"),"SetWindowsHookExA");
 HHOOK a,b;HOOKSTATEDATA sa,sb,bad,again;unsigned before_a,before_b;
 CHECK(install!=NULL);if(!install)return 1;
 CHECK(W32InitHookState(GetModuleHandleA(NULL)));
 a=install(WH_CBT,on_a,NULL,GetCurrentThreadId());b=install(WH_CBT,on_b,NULL,GetCurrentThreadId());
 CHECK(a&&b);if(!a||!b)return 1;
 sa=register_slot(1001,a);sb=register_slot(1002,b);bad=register_slot(1001,NULL);
 create_one();CHECK(calls_a>0&&calls_b>0);before_a=calls_a;before_b=calls_b;
 CHECK(W32FreeOwnedHooks(1001));
 CHECK(W32GetHookStateData(&sa)&&!sa.InUse&&sa.TaskId==0);
 CHECK(W32GetHookStateData(&bad)&&!bad.InUse&&bad.TaskId==0);
 CHECK(W32GetHookStateData(&sb)&&sb.InUse&&sb.TaskId==1002);
 SetLastError(0);CHECK(!UnhookWindowsHookEx(a));CHECK(GetLastError()==ERROR_INVALID_HOOK_HANDLE);
 create_one();CHECK(calls_a==before_a&&calls_b>before_b);
 CHECK(W32FreeOwnedHooks(1001)); /* empty repeat must preserve other task */
 CHECK(W32GetHookStateData(&sb)&&sb.InUse&&sb.TaskId==1002);
 CHECK(W32FreeOwnedHooks(1002));before_b=calls_b;
 create_one();CHECK(calls_a==before_a&&calls_b==before_b);
 CHECK(W32GetNotInUseHookStateData(&again));CHECK(again.iIndex==sa.iIndex);
 again.TaskId=1003;again.hHook=NULL;CHECK(W32SetHookStateData(&again));CHECK(W32FreeOwnedHooks(1003));
 fprintf(stderr,"WOW_ORIGINAL_HOOK_CLEANUP errors=%u removed=1 foreign_preserved=1 failed_unhook_cleared=1 reusable=1\n",errors);
 return errors?1:0;
}
