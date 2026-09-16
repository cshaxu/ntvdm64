/* Modern Console membership mechanism selected by the S1 Console boundary.
 * Original BaseSrv still owns Console records and command selection. */
#include "console_membership.h"
DWORD broker_console_membership(DWORD caller,const DWORD *candidates,DWORD count,BYTE *members)
{
    DWORD current,capacity=32,actual,i,j,error=0;
    DWORD *snapshot=NULL;
    BYTE *result=NULL;
    BOOL found=FALSE;
    if (!caller || (count && (!candidates || !members))) return ERROR_INVALID_PARAMETER;
    /* Never detach a caller's existing Console. Product invokes an isolated
     * detached helper instead of attaching its persistent broker process. */
    if (GetConsoleProcessList(&current,1)) return ERROR_ACCESS_DENIED;
    if (!AttachConsole(caller)) return GetLastError();
    snapshot=HeapAlloc(GetProcessHeap(),0,capacity*sizeof(*snapshot));
    if (!snapshot) {error=ERROR_NOT_ENOUGH_MEMORY;goto done;}
    actual=GetConsoleProcessList(snapshot,capacity);
    if (!actual) {error=GetLastError();goto done;}
    if (actual>capacity) {
        if (actual>MAXDWORD/sizeof(*snapshot)) {error=ERROR_ARITHMETIC_OVERFLOW;goto done;}
        capacity=actual;
        HeapFree(GetProcessHeap(),0,snapshot);
        snapshot=HeapAlloc(GetProcessHeap(),0,capacity*sizeof(*snapshot));
        if (!snapshot) {error=ERROR_NOT_ENOUGH_MEMORY;goto done;}
        actual=GetConsoleProcessList(snapshot,capacity);
        if (!actual) {error=GetLastError();goto done;}
        if (actual>capacity) {error=ERROR_RETRY;goto done;}
    }
    for (i=0;i<actual;++i) if (snapshot[i]==caller) found=TRUE;
    if (!found) {error=ERROR_RETRY;goto done;}
    if (count) {
        result=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,count);
        if (!result) {error=ERROR_NOT_ENOUGH_MEMORY;goto done;}
        for(i=0;i<count;++i) for(j=0;j<actual;++j)
            if (candidates[i]==snapshot[j]) {result[i]=1;break;}
    }
done:
    if (!FreeConsole() && !error) error=GetLastError();
    if (!error && count) CopyMemory(members,result,count);
    if (result) HeapFree(GetProcessHeap(),0,result);
    if (snapshot) HeapFree(GetProcessHeap(),0,snapshot);
    return error;
}
