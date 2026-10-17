/* Included only in the real-CCPU dedicated registered-owner process. */
#include "wow32-dll/include/wow_task_cleanup_entry.h"
#include "wow32-dll/include/wow_dialog_context_bindings.h"
extern unsigned wow_dialog_fail_publication,wow_dialog_publication_failures;
extern unsigned wow_dialog_fail_callback,wow_dialog_callback_rejections;
extern unsigned wow_dialog_throw_capture,wow_dialog_capture_exceptions;
extern void *wow_dialog_failed_allocation;
static unsigned real_dialog_init,real_dialog_terminal,real_dialog_nested,real_dialog_exceptions;
static BOOL real_dialog_veto;
static wow_window_words_binding *real_dialog_veto_borrow;
static BOOL real_allocation_live(void *pointer)
{
    HANDLE heap=GetProcessHeap();PROCESS_HEAP_ENTRY entry={0};BOOL found=FALSE;
    BORROW_CHECK(HeapLock(heap));
    while(HeapWalk(heap,&entry))if(entry.lpData==pointer && (entry.wFlags&PROCESS_HEAP_ENTRY_BUSY)){
        found=TRUE;break;
    }
    BORROW_CHECK(HeapUnlock(heap));return found;
}
static LRESULT CALLBACK real_dialog_veto_hook(int code,WPARAM wp,LPARAM lp)
{
    if(code==HCBT_CREATEWND && real_dialog_veto){
        real_dialog_veto=FALSE;
        real_dialog_veto_borrow=wow_window_words_acquire((HWND)wp);
        BORROW_CHECK(real_dialog_veto_borrow!=NULL);
        return 1;
    }
    return CallNextHookEx(NULL,code,wp,lp);
}
static INT_PTR CALLBACK real_dialog_callback(HWND window,UINT message,WPARAM wp,LPARAM lp)
{
    ULONG backing=c_sas_dw_at(callback_teb+0x70),current=c_sas_dw_at(callback_teb+0x6c);
    wow_user_runtime_thread *runtime=wow_user_runtime_current();
    (void)wp;
    BORROW_CHECK(runtime && !runtime->exclusive_held && runtime->thread &&
        runtime->thread->ppi->pwpi->CSOwningThread==runtime->thread);
    BORROW_CHECK(current==(ULONG)window && backing && c_sas_dw_at(backing)==(ULONG)window);
    if(message==WM_INITDIALOG){
        wow_window_words_binding *retained=wow_window_words_acquire(window);
        PWND state=wow_window_words_client_state_value(retained);
        ++real_dialog_init;
        BORROW_CHECK(lp==0x4473 && GetDlgItem(window,100)!=NULL);
        BORROW_CHECK(state && GETFNID(state)==FNID_DIALOG && TestWF(state,WFDIALOGWINDOW) && ((PDIALOG)state)->pdlg);
        wow_window_words_release(retained);
    }
    if(message==WM_NCDESTROY)++real_dialog_terminal;
    if(message==WM_APP+110){
        ++real_dialog_nested;
        BORROW_CHECK(SendMessageA(window,WM_APP+111,0,0)==0);
        BORROW_CHECK(c_sas_dw_at(callback_teb+0x70)==backing && c_sas_dw_at(callback_teb+0x6c)==current);
        BORROW_CHECK(wow_native_DestroyWindow(window));
        BORROW_CHECK(!IsWindow(window) && c_sas_dw_at(callback_teb+0x70)==backing &&
            c_sas_dw_at(backing)==(ULONG)window);
    }
    if(message==WM_APP+111){
        __try{RaiseException(0xe0420058,0,0,NULL);}
        __except(GetExceptionCode()==0xe0420058 ? EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH){++real_dialog_exceptions;}
    }
    return FALSE;
}
static void verify_real_dialog_lifecycle(wow_user_task_lifecycle *lifecycle)
{
    DWORD storage[128]={0};DLGTEMPLATE *dialog=(DLGTEMPLATE *)storage;
    DLGITEMTEMPLATE *item;WORD *tail;HWND window;HHOOK veto;
    unsigned before=borrow_errors,kind,failure,rollbacks=0,normal_init,normal_terminal;
    UNICODE_STRING name;WNDCLASSEXA cls={sizeof(cls)};DWORD words[2]={0};WW ww={0};
    wow_window_dispatch_target target={(DWORD)DefWindowProcA,NULL,FALSE};ATOM atom;
    CREATESTRUCTA request={0};wow_user_runtime_thread *runtime=wow_user_runtime_current();
    callback_teb=(ULONG)mvdm_softpc_wow_page_domain_guest_teb();
    dialog->style=WS_POPUP|DS_SETFONT;dialog->cdit=1;dialog->cx=100;dialog->cy=40;
    tail=(WORD *)(dialog+1);*tail++=0;*tail++=0;*tail++=0;*tail++=8;
    memcpy(tail,L"MS Shell Dlg",sizeof(L"MS Shell Dlg"));tail+=ARRAYSIZE(L"MS Shell Dlg");
    item=(DLGITEMTEMPLATE *)(((UINT_PTR)tail+3)&~(UINT_PTR)3);
    item->style=WS_CHILD|WS_VISIBLE|ES_LEFT;item->x=2;item->y=2;item->cx=80;item->cy=12;item->id=100;
    tail=(WORD *)(item+1);*tail++=0xffff;*tail++=0x0081;*tail++=0;*tail++=0;
    BORROW_CHECK(!wow_user_server_create_dialog(GetModuleHandleA(NULL),NULL,0,NULL,
        real_dialog_callback,0,SCDLG_CLIENT|SCDLG_ANSI) && GetLastError()==ERROR_INVALID_PARAMETER);
    window=wow_user_server_create_dialog(GetModuleHandleA(NULL),dialog,sizeof(storage),NULL,
        real_dialog_callback,0x4473,SCDLG_CLIENT|SCDLG_ANSI|SCDLG_NOREVALIDATE);
    BORROW_CHECK(window && real_dialog_init==1);
    BORROW_CHECK(wow_native_DestroyWindow(window) && real_dialog_terminal==1);
    window=wow_user_server_create_dialog(GetModuleHandleA(NULL),dialog,sizeof(storage),NULL,
        real_dialog_callback,0x4473,SCDLG_CLIENT|SCDLG_ANSI|SCDLG_NOREVALIDATE);
    BORROW_CHECK(window && real_dialog_init==2);
    {
        BOOL caught=FALSE,held=runtime->exclusive_held;
        wow_dialog_context *previous=wow_dialog_current;
        unsigned callbacks=real_dialog_nested;
        wow_dialog_fail_callback=1;
        BORROW_CHECK(wow_dialog_window_procA(window,WM_APP+110,0,0)==0 &&
            GetLastError()==ERROR_NOT_ENOUGH_MEMORY && wow_dialog_callback_rejections==1 &&
            real_dialog_nested==callbacks && IsWindow(window));
        BORROW_CHECK(runtime->exclusive_held==held && wow_dialog_current==previous &&
            c_sas_dw_at(callback_teb+0x6c)==0 && c_sas_dw_at(callback_teb+0x70)==0);
        wow_dialog_throw_capture=1;
        __try{(void)wow_dialog_window_procA(window,WM_APP+110,0,0);}
        __except(GetExceptionCode()==0xe0420059 ? EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH){caught=TRUE;}
        BORROW_CHECK(caught && wow_dialog_capture_exceptions==1 && GetLastError()==0x44730012);
        BORROW_CHECK(runtime->exclusive_held==held && wow_dialog_current==previous &&
            c_sas_dw_at(callback_teb+0x6c)==0 && c_sas_dw_at(callback_teb+0x70)==0 &&
            runtime->thread->ppi->pwpi->CSOwningThread==runtime->thread);
        fprintf(stderr,"WOW_REAL_DIALOG_SCOPE errors=%u callback_reject=1 capture_exception=1 exact_production_finally=1 cache_restored=1 context_restored=1 owner_restored=1\n",borrow_errors-before);
    }
    SendMessageA(window,WM_APP+110,0,0);
    BORROW_CHECK(!IsWindow(window) && real_dialog_terminal==2 && real_dialog_nested==1 && real_dialog_exceptions==1);
    BORROW_CHECK(c_sas_dw_at(callback_teb+0x6c)==0 && c_sas_dw_at(callback_teb+0x70)==0);
    normal_init=real_dialog_init;normal_terminal=real_dialog_terminal;
    cls.lpfnWndProc=wow_window_class_creation_proc;cls.hInstance=GetModuleHandleA(NULL);cls.lpszClassName="RealDialogRollback";
    RtlInitUnicodeString(&name,L"RealDialogRollback");
    atom=wow_class_words_register_target(runtime->thread->ppi->classes,&cls,&name,words,NULL,&target);
    BORROW_CHECK(atom!=0);request.hInstance=cls.hInstance;request.lpszClass=cls.lpszClassName;
    request.lpszName="";request.style=WS_POPUP;request.cx=request.cy=16;
    veto=SetWindowsHookExA(WH_CBT,real_dialog_veto_hook,NULL,GetCurrentThreadId());BORROW_CHECK(veto!=NULL);
    for(kind=0;kind<2;++kind)for(failure=0;failure<2;++failure){
        void *allocation;
        real_dialog_veto_borrow=NULL;wow_dialog_failed_allocation=NULL;
        if(failure)real_dialog_veto=TRUE;else wow_dialog_fail_publication=1;
        window=kind?wow_window_create_boundA(runtime->thread,atom,&request,&ww,NULL):
            wow_user_server_create_dialog(GetModuleHandleA(NULL),dialog,sizeof(storage),NULL,
                real_dialog_callback,0x4473,SCDLG_CLIENT|SCDLG_ANSI|SCDLG_NOREVALIDATE);
        real_dialog_veto=FALSE;wow_dialog_fail_publication=0;BORROW_CHECK(!window);
        allocation=failure?(void *)real_dialog_veto_borrow:wow_dialog_failed_allocation;
        BORROW_CHECK(allocation!=NULL);
        if(failure){BORROW_CHECK(real_allocation_live(allocation));wow_window_words_release(real_dialog_veto_borrow);}
        BORROW_CHECK(!real_allocation_live(allocation));++rollbacks;
        BORROW_CHECK(real_dialog_init==normal_init && real_dialog_terminal==normal_terminal);
    }
    BORROW_CHECK(wow_dialog_publication_failures==2 && UnhookWindowsHookEx(veto));
    BORROW_CHECK(wow_class_words_unregister(runtime->thread->ppi->classes,atom,cls.hInstance,NULL));
    fprintf(stderr,"WOW_REAL_DIALOG_ROLLBACK errors=%u cases=%u physical_free=1 publications=2 vetoes=2 no_app_callback=1\n",borrow_errors-before,rollbacks);
    window=wow_user_server_create_dialog(GetModuleHandleA(NULL),dialog,sizeof(storage),NULL,
        real_dialog_callback,0x4473,SCDLG_CLIENT|SCDLG_ANSI|SCDLG_NOREVALIDATE);
    BORROW_CHECK(window!=NULL);
    if(window){
        wow_window_words_binding *retained=wow_window_words_acquire(window);
        PWND state=wow_window_words_client_state_value(retained);unsigned calls=real_dialog_terminal;
        BORROW_CHECK(state && ((PDIALOG)state)->pdlg);
        BORROW_CHECK(wow_user_task_prepare_exit());
        BORROW_CHECK(!IsWindow(window) && calls==real_dialog_terminal && state &&
            (state->fnid&FNID_CLEANEDUP_BIT) && TestWF(state,WFDESTROYED) &&
            !TestWF(state,WFDIALOGWINDOW) && ((PDIALOG)state)->pdlg==NULL);
        wow_window_words_release(retained);
    }
    BORROW_CHECK(lifecycle==runtime->runtime->lifecycle);
    fprintf(stderr,"WOW_REAL_DIALOG_LIFECYCLE errors=%u init=3 terminal=2 nested=1 exception_local=1 callback_cache=1 pdlg_retired=1 dying_callback_denied=1\n",borrow_errors-before);
}
