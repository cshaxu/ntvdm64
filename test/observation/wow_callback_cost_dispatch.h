/* callback_cost_body is the mechanically renamed, otherwise exact original
 * project binding (or the explicitly labelled unit double). No guest/mirror
 * algorithm lives here. Rejected/exceptional calls never dereference result. */
BOOL WINAPI wow_window_dispatch_immutable(HWND window,UINT message,WPARAM wp,
    LPARAM lp,const wow_window_dispatch_target *target,LONG *result)
{
    LONG slot=callback_cost_begin(window,message);
    LONG previous=callback_cost_active;
    BOOL accepted=FALSE,normal=FALSE;
    callback_cost_active=(slot>=0 && (ULONG)slot<CALLBACK_COST_CAPACITY)?slot:-1;
    __try{
        accepted=callback_cost_body(window,message,wp,lp,target,result);
        normal=TRUE;
    }__finally{
        DWORD error=GetLastError();
        callback_cost_end(slot,accepted,normal,
            accepted && normal ? *result : 0,error);
        callback_cost_active=previous;
    }
    return accepted;
}
