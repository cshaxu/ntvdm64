/* Pure native counterexample to PID-based snapshot atomicity. This reduced
 * relationship subgraph does not attribute an earlier product failure. */
#include <windows.h>
#include <stdio.h>
typedef struct { struct { ULONG window,next,parent,owner,child; } links; DWORD process_id; } window_observation;
static BOOL deny_allocation;
static PVOID WINAPI control_heap_alloc(HANDLE heap,DWORD flags,SIZE_T size)
{ return deny_allocation?NULL:HeapAlloc(heap,flags,size); }
#define HeapAlloc control_heap_alloc
#include "chain_exact_body.h"
#undef HeapAlloc
static ULONG index_by_slot[65536];
static HWND relation(HWND window,unsigned edge)
{ return edge==0 ? GetWindow(window,GW_HWNDNEXT) : edge==1 ? GetAncestor(window,GA_PARENT) : GetWindow(window,GW_OWNER); }
int main(void)
{
 HDESK old=GetThreadDesktop(GetCurrentThreadId()), desktop;
 HWND parent,a,b,first,second; unsigned edge; int errors=0;
 char name[64];
 sprintf_s(name,sizeof(name),"T447-native-chain-%lu",GetCurrentProcessId());
 desktop=CreateDesktopA(name,NULL,NULL,0,GENERIC_ALL,NULL);
 if(!desktop || !SetThreadDesktop(desktop))return 2;
 parent=CreateWindowExA(0,"STATIC","root",WS_OVERLAPPED,0,0,100,100,NULL,NULL,GetModuleHandleA(NULL),NULL);
 if(!parent)return 3;
 for(edge=0;edge<3;++edge){
  window_observation rows[2]={{0}}; BOOL rejected,live,raced=FALSE; DWORD error;
  b=CreateWindowExA(0,"STATIC","b",edge==2?WS_POPUP:WS_CHILD,0,0,20,20,edge==2?NULL:parent,NULL,GetModuleHandleA(NULL),NULL);
  a=CreateWindowExA(0,"STATIC","a",edge==2?WS_POPUP:WS_CHILD,0,0,20,20,edge==2?b:edge==1?b:parent,NULL,GetModuleHandleA(NULL),NULL);
  if(!a || !b)return 4;
  if(edge==0 && !SetWindowPos(a,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE))return 5;
  first=relation(a,edge);
  if(edge==0){if(!SetWindowPos(b,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE))return 6;}
  else if(edge==1){if(!SetParent(a,parent) || !SetParent(b,a))return 7;}
  else {SetWindowLongPtrA(a,GWLP_HWNDPARENT,0);SetWindowLongPtrA(b,GWLP_HWNDPARENT,(LONG_PTR)a);}
  second=relation(b,edge);
  if(first!=b || second!=a)return 8;
  rows[0].links.window=(ULONG)(ULONG_PTR)a;rows[1].links.window=(ULONG)(ULONG_PTR)b;
  rows[0].process_id=rows[1].process_id=GetCurrentProcessId();
  ZeroMemory(index_by_slot,sizeof(index_by_slot));index_by_slot[LOWORD(a)]=1;index_by_slot[LOWORD(b)]=2;
  *observation_edge(&rows[0],edge)=(ULONG)(ULONG_PTR)first;
  *observation_edge(&rows[1],edge)=(ULONG)(ULONG_PTR)second;
  SetLastError(0);rejected=!normalize_observation_chains(rows,2,index_by_slot,&raced);error=GetLastError();
  if(!raced)++errors;
  first=relation(a,edge);second=relation(b,edge);
  *observation_edge(&rows[0],edge)=(first==b)?(ULONG)(ULONG_PTR)b:0;
  *observation_edge(&rows[1],edge)=(second==a)?(ULONG)(ULONG_PTR)a:0;
  live=normalize_observation_chains(rows,2,index_by_slot,&raced);
  if(raced)++errors;
  printf("edge=%u same-pid=1 valid-native-interleave=1 mixed-rejected=%u error=%lu live-accepted=%u\n",edge,rejected,error,live);
  if(!rejected || error!=ERROR_INVALID_DATA || !live)++errors;
  *observation_edge(&rows[0],edge)=(ULONG)(ULONG_PTR)b;
  *observation_edge(&rows[1],edge)=(ULONG)(ULONG_PTR)a;
  rows[0].process_id=GetCurrentProcessId()+1;raced=TRUE;
  if(!normalize_observation_chains(rows,2,index_by_slot,&raced) || raced ||
      *observation_edge(&rows[0],edge) || *observation_edge(&rows[1],edge)!=(ULONG)(ULONG_PTR)a)++errors;
  deny_allocation=TRUE;raced=TRUE;SetLastError(0);
  if(normalize_observation_chains(rows,2,index_by_slot,&raced) || raced || GetLastError()!=ERROR_NOT_ENOUGH_MEMORY)++errors;
  deny_allocation=FALSE;
  if(edge==2){SetWindowLongPtrA(b,GWLP_HWNDPARENT,0);SetWindowLongPtrA(a,GWLP_HWNDPARENT,0);}
  else {SetParent(b,parent);SetParent(a,parent);}
  DestroyWindow(a);DestroyWindow(b);
 }
 DestroyWindow(parent);SetThreadDesktop(old);CloseDesktop(desktop);
 printf("NATIVE_CHAIN_CONTROL errors=%d production-cause-proven=0\n",errors);return errors?1:0;
}
