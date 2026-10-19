/* Actual public HWND reads and exact production acquisition/normalization.
 * Only the page-domain read/commit services are doubles; not guest proof. */
#include <windows.h>
#include <stdio.h>
#include "../../src/ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"
typedef mvdm_softpc_wow_window_observation window_observation;
typedef struct { HWND window,next,parent,child,owner; } wow_user_window_links;
enum { NORMAL,ONCE,ALWAYS,COPY_FAIL,COMMIT_FAIL };
static HWND parent,a,b;
static unsigned selected_edge,mode,attempts,commit_calls,commits,injections,cases,errors;
static BOOL armed;
static void check(BOOL ok,const char *name)
{ if(!ok){++errors;printf("FAIL %s edge=%u mode=%u attempts=%u commits=%u error=%lu\n",name,selected_edge,mode,attempts,commits,GetLastError());} }
static void layout(BOOL initial)
{
 if(selected_edge==0)check(SetWindowPos(initial?a:b,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE),"native-order");
 else if(selected_edge==1){
  if(initial){check(SetParent(b,parent)!=NULL,"parent-b-root");check(SetParent(a,b)!=NULL,"parent-a-b");}
  else {check(SetParent(a,parent)!=NULL,"parent-a-root");check(SetParent(b,a)!=NULL,"parent-b-a");}
 }else{
  SetWindowLongPtrA(b,GWLP_HWNDPARENT,0);SetWindowLongPtrA(a,GWLP_HWNDPARENT,initial?(LONG_PTR)b:0);
  if(!initial)SetWindowLongPtrA(b,GWLP_HWNDPARENT,(LONG_PTR)a);
  check(GetWindow(a,GW_OWNER)==(initial?b:NULL) && GetWindow(b,GW_OWNER)==(initial?NULL:a),"native-owner");
 }
}
static void interleave(HWND window,unsigned selected)
{ if(armed && window==a && selected_edge==selected){armed=FALSE;++injections;layout(FALSE);} }
static HWND WINAPI query_window(HWND window,UINT command)
{
 HWND result=GetWindow(window,command);
 if(command==GW_HWNDNEXT)interleave(window,0);
 if(command==GW_OWNER)interleave(window,2);
 return result;
}
static HWND WINAPI query_ancestor(HWND window,UINT command)
{ HWND result=GetAncestor(window,command);if(command==GA_PARENT)interleave(window,1);return result; }
int __cdecl mvdm_softpc_wow_page_domain_copy_window_view(ULONG capacity,
 mvdm_softpc_wow_window_observation *rows,ULONG *count)
{
 (void)capacity;
 if(!rows){
  ++attempts;
  if(mode==COPY_FAIL){SetLastError(ERROR_INVALID_DATA);return 0;}
  armed=(mode==ALWAYS || (mode==ONCE && attempts==1));
  if(armed)layout(TRUE);
 }
 *count=0;return 1;
}
static HWND WINAPI wow_user_window_native_handle(HWND window)
{ check(FALSE,"unexpected-short-handle");return window; }
static BOOL WINAPI wow_window_words_project_observed(ULONG count,
 const mvdm_softpc_wow_window_observation *rows)
{
 ULONG i;BOOL found_a=FALSE,found_b=FALSE;
 ++commit_calls;
 if(mode==COMMIT_FAIL){SetLastError(ERROR_INVALID_DATA);return FALSE;}
 for(i=0;i<count;++i){
  const mvdm_softpc_wow_window_links *links=&rows[i].links;
  if(links->window==(ULONG)(ULONG_PTR)a){
   HWND actual=selected_edge==0?GetWindow(a,GW_HWNDNEXT):selected_edge==1?GetAncestor(a,GA_PARENT):GetWindow(a,GW_OWNER);
   ULONG observed=selected_edge==0?links->next:selected_edge==1?links->parent:links->owner;
   check(observed==(ULONG)(ULONG_PTR)actual,"fresh-a-at-commit");found_a=TRUE;
  }
  if(links->window==(ULONG)(ULONG_PTR)b){
   HWND actual=selected_edge==0?GetWindow(b,GW_HWNDNEXT):selected_edge==1?GetAncestor(b,GA_PARENT):GetWindow(b,GW_OWNER);
   ULONG observed=selected_edge==0?links->next:selected_edge==1?links->parent:links->owner;
   check(observed==(ULONG)(ULONG_PTR)actual,"fresh-b-at-commit");found_b=TRUE;
  }
 }
 check(found_a && found_b,"full-identities-at-commit");++commits;return TRUE;
}
#define GetWindow query_window
#define GetAncestor query_ancestor
#include "capture_exact_body.h"
#undef GetWindow
#undef GetAncestor
static void run(BOOL baseline,unsigned selected_mode)
{
 HWND roots[4];BOOL result;DWORD error;unsigned expected_attempts;
 mode=selected_mode;attempts=commits=commit_calls=injections=0;armed=FALSE;
 b=CreateWindowExA(0,"STATIC","b",selected_edge==2?WS_POPUP:WS_CHILD,0,0,20,20,selected_edge==2?NULL:parent,NULL,GetModuleHandleA(NULL),NULL);
 a=CreateWindowExA(0,"STATIC","a",selected_edge==2?WS_POPUP:WS_CHILD,0,0,20,20,selected_edge==2?b:parent,NULL,GetModuleHandleA(NULL),NULL);
 check(a && b,"native-create");if(!a || !b)return;
 layout(TRUE);roots[0]=GetDesktopWindow();roots[1]=parent;roots[2]=a;roots[3]=b;
 SetLastError(0x447a);result=baseline?baseline_project_graph(roots,4,NULL,0):project_graph(roots,4,NULL,0);error=GetLastError();
 expected_attempts=!baseline && (mode==ONCE || mode==ALWAYS)?2:1;
 check(attempts==expected_attempts,"bounded-acquisition");
 if(mode==NORMAL || (!baseline && mode==ONCE)){
  check(result && commits==1 && commit_calls==1,"one-complete-commit");check(error==0x447a,"entry-error-preserved");
 }else{
  check(!result && !commits && error==ERROR_INVALID_DATA,"failure-without-publication");
  check(commit_calls==(mode==COMMIT_FAIL?1u:0u),"non-race-failure-not-replayed");
 }
 if(mode==ONCE || mode==ALWAYS)check(injections==(mode==ONCE?1u:expected_attempts),"actual-native-interleaving");
 printf("CAPTURE edge=%u baseline=%u mode=%u result=%u acquisitions=%u commits=%u error=%lu\n",selected_edge,baseline,mode,result,attempts,commits,error);++cases;
 if(selected_edge==2){SetWindowLongPtrA(b,GWLP_HWNDPARENT,0);SetWindowLongPtrA(a,GWLP_HWNDPARENT,0);}
 else {SetParent(b,parent);SetParent(a,parent);}
 DestroyWindow(a);DestroyWindow(b);
}
int main(void)
{
 HDESK previous=GetThreadDesktop(GetCurrentThreadId()),desktop;char name[64];unsigned selected;
 sprintf_s(name,sizeof(name),"T447-capture-acquire-%lu",GetCurrentProcessId());
 desktop=CreateDesktopA(name,NULL,NULL,0,GENERIC_ALL,NULL);
 if(!desktop || !SetThreadDesktop(desktop))return 2;
 parent=CreateWindowExA(0,"STATIC","root",WS_OVERLAPPED,0,0,100,100,NULL,NULL,GetModuleHandleA(NULL),NULL);
 if(!parent)return 3;
 for(selected_edge=0;selected_edge<3;++selected_edge){run(TRUE,NORMAL);run(TRUE,ONCE);for(selected=NORMAL;selected<=COMMIT_FAIL;++selected)run(FALSE,selected);}
 DestroyWindow(parent);SetThreadDesktop(previous);CloseDesktop(desktop);
 printf("CAPTURE_REACQUIRE cases=%u errors=%u production-cause-proven=0\n",cases,errors);return errors?1:0;
}
