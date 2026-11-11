#include <windows.h>
#include <stdio.h>
/* Research only: original xxxSetWindowData resolves both CPD address and
 * encoding, unlike class registration. Only owned message-only windows. */
static unsigned failures, calls;
static unsigned helper_creates, helper_destroys;
static BOOL creating_helper;
static HWND observed_helper;
/* Same-thread CBT observer models an existing application hook. A hidden
 * message-only helper must not be assumed invisible to native hook traffic. */
static LRESULT CALLBACK observe_cbt(int code, WPARAM wp, LPARAM lp)
{
    if (code==HCBT_CREATEWND && creating_helper) {
        ++helper_creates; observed_helper=(HWND)wp;
    }
    if (code==HCBT_DESTROYWND && (HWND)wp==observed_helper) {
        ++helper_destroys; observed_helper=NULL;
    }
    return CallNextHookEx(NULL,code,wp,lp);
}
#define CHECK(c) do { if (!(c)) { ++failures; printf("FAIL line=%d error=%lu\n",__LINE__,GetLastError()); } } while (0)
static LRESULT CALLBACK source_a(HWND w, UINT m, WPARAM p, LPARAM l)
{ ++calls; return DefWindowProcA(w,m,p,l); }
static LRESULT CALLBACK source_w(HWND w, UINT m, WPARAM p, LPARAM l)
{ ++calls; return DefWindowProcW(w,m,p,l); }
static LRESULT CALLBACK scratch_proc(HWND w, UINT m, WPARAM p, LPARAM l)
{ return DefWindowProcW(w,m,p,l); }
typedef struct decoded_proc { LONG_PTR address; BOOL ansi; } decoded_proc;
/* Candidate only. Output commits after cleanup; an opaque/ambiguous result
 * is never executable. The caller has already installed scratch_proc. */
static BOOL decode_slot(HWND holder, LONG_PTR token, decoded_proc *output)
{
    LONG_PTR a,w,old;
    BOOL good=FALSE, a_raw,w_raw;
    DWORD saved=GetLastError(), error=ERROR_INVALID_HANDLE;
    decoded_proc candidate={0};
    if (HIWORD((ULONG_PTR)token)!=0xffff) { SetLastError(error); return FALSE; }
    __try {
        SetLastError(0);
        old=SetWindowLongPtrW(holder,DWLP_DLGPROC,token);
        if (!old && GetLastError()) { error=GetLastError(); __leave; }
        a=GetWindowLongPtrA(holder,DWLP_DLGPROC);
        w=GetWindowLongPtrW(holder,DWLP_DLGPROC);
        a_raw=a && HIWORD((ULONG_PTR)a)!=0xffff;
        w_raw=w && HIWORD((ULONG_PTR)w)!=0xffff;
        SendMessageW(holder,WM_NULL,0,0);
        if (a_raw==w_raw || !a || !w) __leave;
        candidate.address=a_raw ? a : w; candidate.ansi=a_raw;
        good=TRUE;
    } __finally {
        SetLastError(0);
        old=SetWindowLongPtrW(holder,DWLP_DLGPROC,0);
        if (!old && GetLastError()) { good=FALSE; error=GetLastError(); }
    }
    if (good) *output=candidate;
    SetLastError(good ? saved : error);
    return good;
}
static void probe(BOOL source_ansi, BOOL setter_ansi, BOOL dialog_slot)
{
    HINSTANCE module=GetModuleHandleW(NULL);
    WNDCLASSEXA a={sizeof(a)};
    WNDCLASSEXW w={sizeof(w)};
    HWND source=NULL,scratch=NULL;
    ATOM atom=0, scratch_atom=0;
    LONG_PTR token, previous, read_a, read_w;
    LONG_PTR holder_proc=0;
    int index=dialog_slot ? DWLP_DLGPROC : GWLP_WNDPROC;
    unsigned before;
    a.hInstance=module; a.lpfnWndProc=source_a; a.lpszClassName="T447CPDWindowSource";
    w.hInstance=module; w.lpfnWndProc=source_w; w.lpszClassName=L"T447CPDWindowSource";
    atom=source_ansi ? RegisterClassExA(&a) : RegisterClassExW(&w);
    CHECK(atom); if (!atom) return;
    w.lpfnWndProc=scratch_proc; w.lpszClassName=L"T447CPDWindowScratch";
    scratch_atom=RegisterClassExW(&w);
    CHECK(scratch_atom); if (!scratch_atom) goto cleanup;
    source=CreateWindowExW(0,MAKEINTATOM(atom),L"",0,0,0,0,0,HWND_MESSAGE,NULL,module,NULL);
    creating_helper=TRUE;
    scratch=CreateWindowExW(0,dialog_slot ? L"#32770" : MAKEINTATOM(scratch_atom),L"",0,0,0,0,0,HWND_MESSAGE,NULL,module,NULL);
    creating_helper=FALSE;
    CHECK(source && scratch); if (!source || !scratch) goto cleanup;
    /* Keep all dispatched messages on a known safe window procedure while
     * USER normalizes the independent dialog procedure slot. */
    if (dialog_slot) {
        holder_proc=SetWindowLongPtrW(scratch,GWLP_WNDPROC,(LONG_PTR)scratch_proc);
        CHECK(holder_proc); if (!holder_proc) goto cleanup;
    }
    token=source_ansi ? GetWindowLongPtrW(source,GWLP_WNDPROC) : GetWindowLongPtrA(source,GWLP_WNDPROC);
    CHECK(HIWORD((ULONG_PTR)token)==0xffff);
    CHECK(DestroyWindow(source)); source=NULL;
    before=calls;
    SetLastError(0);
    previous=setter_ansi ? SetWindowLongPtrA(scratch,index,token) : SetWindowLongPtrW(scratch,index,token);
    CHECK(previous!=0 || (dialog_slot && GetLastError()==0));
    read_a=GetWindowLongPtrA(scratch,index);
    read_w=GetWindowLongPtrW(scratch,index);
    if (dialog_slot) SendMessageW(scratch,WM_NULL,0,0);
    printf("dialog_slot=%u source_ansi=%u setter_ansi=%u token=%p A=%p W=%p unicode=%u callbacks=%u\n",
        dialog_slot,source_ansi,setter_ansi,(void*)token,(void*)read_a,(void*)read_w,IsWindowUnicode(scratch),calls-before);
    CHECK((source_ansi ? read_a : read_w)==(LONG_PTR)(source_ansi ? source_a : source_w));
    CHECK(HIWORD((ULONG_PTR)(source_ansi ? read_w : read_a))==0xffff);
    CHECK(IsWindowUnicode(scratch)==(dialog_slot ? TRUE : !source_ansi));
    CHECK(calls==before);
    CHECK(SetWindowLongPtrW(scratch,index,dialog_slot ? 0 : (LONG_PTR)scratch_proc)!=0);
    if (dialog_slot) {
        decoded_proc result={(LONG_PTR)123,TRUE};
        before=calls;
        CHECK(!decode_slot(scratch,(LONG_PTR)0xffff0000,&result));
        CHECK(result.address==123 && result.ansi==TRUE);
        CHECK(GetWindowLongPtrW(scratch,DWLP_DLGPROC)==0);
        SetLastError(0x447);
        CHECK(decode_slot(scratch,token,&result));
        CHECK(GetLastError()==0x447);
        CHECK(result.address==(LONG_PTR)(source_ansi ? source_a : source_w) && result.ansi==source_ansi);
        CHECK(GetWindowLongPtrW(scratch,DWLP_DLGPROC)==0);
        /* No original class/window reference remains for this token. */
        CHECK(UnregisterClassW(MAKEINTATOM(atom),module)); atom=0;
        result.address=123; result.ansi=TRUE;
        CHECK(!decode_slot(scratch,token,&result));
        CHECK(result.address==123 && result.ansi==TRUE);
        CHECK(GetWindowLongPtrW(scratch,DWLP_DLGPROC)==0 && calls==before);
    }
cleanup:
    if (holder_proc) CHECK(SetWindowLongPtrW(scratch,GWLP_WNDPROC,holder_proc)!=0);
    if (scratch) CHECK(DestroyWindow(scratch));
    if (source) CHECK(DestroyWindow(source));
    if (scratch_atom) CHECK(UnregisterClassW(MAKEINTATOM(scratch_atom),module));
    if (atom) CHECK(UnregisterClassW(MAKEINTATOM(atom),module));
}
int main(void)
{
    HHOOK hook=SetWindowsHookExW(WH_CBT,observe_cbt,NULL,GetCurrentThreadId());
    CHECK(hook!=NULL); if (!hook) return 1;
    probe(TRUE,FALSE,FALSE); probe(TRUE,TRUE,FALSE); probe(FALSE,FALSE,FALSE); probe(FALSE,TRUE,FALSE);
    probe(TRUE,FALSE,TRUE); probe(TRUE,TRUE,TRUE); probe(FALSE,FALSE,TRUE); probe(FALSE,TRUE,TRUE);
    CHECK(UnhookWindowsHookEx(hook));
    CHECK(helper_creates==8 && helper_destroys==8);
    printf("helper_cbt_creates=%u helper_cbt_destroys=%u\n",helper_creates,helper_destroys);
    printf("cpd_window_normalization_probe failures=%u\n",failures);
    return failures ? 1 : 0;
}
