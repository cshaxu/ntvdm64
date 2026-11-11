/* Exact transaction-body controls with lower USER/page-domain stand-ins.
 * Not guest, CCPU, native concurrency, scheduling or performance evidence. */
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"

typedef mvdm_softpc_wow_window_observation window_observation;
typedef struct native_row {
    window_observation value;
    BOOL alive;
    ULONG captures;
} native_row;
static window_observation fixture_initial[12], fixture_committed[12];
static native_row fixture_native[12];
static ULONG fixture_count, fixture_native_count, fixture_commit_count;
static ULONG fixture_committed_count, fixture_fail_window;
static BOOL fixture_fail_copy;
static int fixture_errors;
#define CHECK(test) do { if (!(test)) { ++fixture_errors; \
    printf("root-capture-failure line=%d test=%s\n",__LINE__,#test); } } while (0)

static native_row *fixture_find(ULONG value)
{
    ULONG i;
    for (i=0;i<fixture_native_count;++i)
        if (fixture_native[i].value.links.window==value) return &fixture_native[i];
    return NULL;
}
static BOOL fixture_is_window(HWND window)
{
    native_row *row=fixture_find((ULONG)(ULONG_PTR)window);
    return row && row->alive;
}
static BOOL capture_window(HWND window,window_observation *value)
{
    native_row *row=fixture_find((ULONG)(ULONG_PTR)window);
    if (row) ++row->captures;
    if (!row || !row->alive) {SetLastError(ERROR_INVALID_WINDOW_HANDLE);return FALSE;}
    if (row->value.links.window==fixture_fail_window) {
        SetLastError(ERROR_ACCESS_DENIED);return FALSE;
    }
    *value=row->value;
    SetLastError(ERROR_SUCCESS);
    return TRUE;
}
int __cdecl mvdm_softpc_wow_page_domain_copy_window_view(ULONG capacity,
    window_observation *rows,ULONG *count)
{
    *count=fixture_count;
    if (!rows) return capacity==0;
    if (fixture_fail_copy || capacity<fixture_count) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);return FALSE;
    }
    CopyMemory(rows,fixture_initial,fixture_count*sizeof(*rows));
    return TRUE;
}
static HWND WINAPI wow_user_window_native_handle(HWND window) {return window;}
static BOOL WINAPI wow_window_words_project_observed(ULONG count,
    const window_observation *rows)
{
    CHECK(count<=12u);
    if (count>12u) return FALSE;
    ++fixture_commit_count;fixture_committed_count=count;
    CopyMemory(fixture_committed,rows,count*sizeof(*rows));
    return TRUE;
}
#define IsWindow fixture_is_window
#include "required_root_capture_body.h"
#undef IsWindow

static void fixture_reset(void)
{
    ZeroMemory(fixture_initial,sizeof(fixture_initial));
    ZeroMemory(fixture_native,sizeof(fixture_native));
    ZeroMemory(fixture_committed,sizeof(fixture_committed));
    fixture_count=fixture_native_count=fixture_commit_count=0;
    fixture_committed_count=fixture_fail_window=0;fixture_fail_copy=FALSE;
}
static void fixture_add(ULONG window,BOOL observed,BOOL local,BOOL alive)
{
    native_row *row=&fixture_native[fixture_native_count++];
    row->value.links.window=window;row->value.descriptor.atom=10;
    row->value.process_id=GetCurrentProcessId()+(local?0u:1u);
    row->value.thread_id=42;row->value.style=window^0x100u;
    row->value.context_help_id=window^0x200u;
    row->value.descriptor.name_bytes=2;
    row->value.descriptor.name[0]='x';row->alive=alive;
    if(observed){
        fixture_initial[fixture_count]=row->value;
        fixture_initial[fixture_count++].style=0; /* Deliberately stale. */
    }
}
static BOOL fixture_has(ULONG value,BOOL fresh)
{
    ULONG i;native_row *row=fixture_find(value);
    for(i=0;i<fixture_committed_count;++i)
        if(fixture_committed[i].links.window==value)
            return !fresh || (row && !memcmp(&fixture_committed[i],
                &row->value,sizeof(row->value)));
    return FALSE;
}
int main(void)
{
    HWND roots[2]={(HWND)0x10020,(HWND)0x10022};
    HWND payload[1]={(HWND)0x10020};
    const DWORD saved=0x447094u;
    fixture_reset();
    fixture_add(0x10020,TRUE,TRUE,TRUE);fixture_add(0x10022,TRUE,TRUE,TRUE);
    fixture_add(0x10024,TRUE,TRUE,TRUE);fixture_add(0x10026,TRUE,FALSE,TRUE);
    SetLastError(saved);CHECK(project_graph(roots,2,NULL,0));CHECK(GetLastError()==saved);
    CHECK(fixture_commit_count==1 && fixture_committed_count==4);
    CHECK(fixture_native[0].captures==1 && fixture_native[1].captures==1);
    CHECK(fixture_native[2].captures==1 && fixture_native[3].captures==0);
    CHECK(fixture_has(0x10020,TRUE) && fixture_has(0x10022,TRUE));
    CHECK(fixture_has(0x10024,TRUE) && fixture_has(0x10026,FALSE));
    CHECK(fixture_committed[3].style==0); /* External snapshot is retained. */

    fixture_reset();fixture_add(0x10020,FALSE,TRUE,TRUE);
    CHECK(project_graph(roots,1,NULL,0));CHECK(fixture_has(0x10020,TRUE));
    CHECK(fixture_native[0].captures==1);

    fixture_reset();fixture_add(0x20020,TRUE,TRUE,FALSE);
    fixture_add(0x10020,FALSE,TRUE,TRUE);
    CHECK(project_graph(roots,1,NULL,0));CHECK(!fixture_has(0x20020,FALSE));
    CHECK(fixture_has(0x10020,TRUE) && fixture_committed_count==1);

    fixture_reset();fixture_add(0x20020,TRUE,TRUE,TRUE);
    fixture_add(0x10020,FALSE,TRUE,TRUE);
    CHECK(!project_graph(roots,1,NULL,0));CHECK(GetLastError()==ERROR_INVALID_STATE);
    CHECK(fixture_commit_count==0); /* A low word is not an exact root match. */

    fixture_reset();fixture_add(0x10020,TRUE,TRUE,TRUE);
    fixture_fail_window=0x10020;
    CHECK(!project_graph(roots,1,NULL,0));CHECK(GetLastError()==ERROR_ACCESS_DENIED);
    CHECK(fixture_commit_count==0 && fixture_initial[0].style==0);

    fixture_reset();fixture_add(0x10020,TRUE,TRUE,FALSE);
    CHECK(!project_graph(roots,1,NULL,0));CHECK(GetLastError()==ERROR_INVALID_WINDOW_HANDLE);
    CHECK(fixture_commit_count==0);

    fixture_reset();fixture_add(0x10020,TRUE,TRUE,TRUE);
    fixture_add(0x10022,FALSE,TRUE,TRUE);fixture_fail_window=0x10022;
    CHECK(!project_graph(roots,2,NULL,0));CHECK(GetLastError()==ERROR_ACCESS_DENIED);
    CHECK(fixture_commit_count==0 && fixture_initial[0].style==0);

    fixture_reset();fixture_add(0x10020,TRUE,TRUE,TRUE);
    CHECK(project_graph(roots,1,payload,1));CHECK(fixture_native[0].captures==2);
    CHECK(fixture_has(0x10020,TRUE)); /* Explicit payload still samples again. */

    fixture_reset();fixture_add(0x10020,TRUE,FALSE,TRUE);
    CHECK(project_graph(roots,1,NULL,0));CHECK(fixture_native[0].captures==1);
    CHECK(fixture_has(0x10020,TRUE)); /* An explicit foreign root stays fresh. */

    fixture_reset();fixture_add(0x10020,TRUE,TRUE,TRUE);fixture_fail_copy=TRUE;
    CHECK(!project_graph(roots,1,NULL,0));CHECK(GetLastError()==ERROR_NOT_ENOUGH_MEMORY);
    CHECK(fixture_commit_count==0 && fixture_native[0].captures==0);
    printf("REQUIRED_ROOT_CAPTURE errors=%d overwritten_read=0 final_fresh=1 "
        "generation=1 failure_atomic=1 payload=1 external=1 last_error=1\n",fixture_errors);
    return fixture_errors?1:0;
}
