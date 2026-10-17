#include <windows.h>
#include <stdio.h>

/* Research only. Class registration consumes the address, not the source
 * procedure encoding. No helper HWND is used to normalize an input. The
 * source HWND below exists solely to produce a real native test token. */
static unsigned failures;
static unsigned calls, text_calls;
static BOOL check_text;
#define CHECK(x) do { if (!(x)) { ++failures; printf("FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)
static LRESULT CALLBACK source_a(HWND w, UINT m, WPARAM p, LPARAM l)
{
    ++calls;
    if (m == WM_SETTEXT && check_text) {
        ++text_calls; CHECK(l && !lstrcmpA((LPCSTR)l, "target")); return TRUE;
    }
    return DefWindowProcA(w,m,p,l);
}
static LRESULT CALLBACK source_w(HWND w, UINT m, WPARAM p, LPARAM l)
{
    ++calls;
    if (m == WM_SETTEXT && check_text) {
        ++text_calls; CHECK(l && !lstrcmpW((LPCWSTR)l, L"target")); return TRUE;
    }
    return DefWindowProcW(w,m,p,l);
}

/* Research-only lifecycle transaction on actual destination objects, never
 * auxiliary normalization windows. Both destinations outlive the producer.
 * This does not prove absence of concurrent external observations of the
 * transient WNDPROC; it is not a selected production implementation. */
static void destination_lifetime(BOOL source_ansi, BOOL destination_ansi)
{
    HINSTANCE module = GetModuleHandleW(NULL);
    WNDCLASSEXA a = {sizeof(a)};
    WNDCLASSEXW w = {sizeof(w)};
    const WCHAR *names[] = {L"T447OwnerSource", L"T447OwnerTarget0", L"T447OwnerTarget1"};
    const char *anames[] = {"T447OwnerSource", "T447OwnerTarget0", "T447OwnerTarget1"};
    HWND windows[3] = {0};
    ATOM atoms[3] = {0};
    WNDPROC owned[2] = {0}, incoming;
    unsigned i, before, before_text;
    for (i = 0; i < 3; ++i) {
        BOOL ansi = i ? destination_ansi : source_ansi;
        a.hInstance = module; a.lpfnWndProc = source_a; a.lpszClassName = anames[i];
        w.hInstance = module; w.lpfnWndProc = source_w; w.lpszClassName = names[i];
        atoms[i] = ansi ? RegisterClassExA(&a) : RegisterClassExW(&w);
        CHECK(atoms[i]); if (!atoms[i]) goto cleanup;
        windows[i] = CreateWindowExW(0, MAKEINTATOM(atoms[i]), L"", WS_POPUP,
            0, 0, 1, 1, NULL, NULL, module, NULL);
        CHECK(windows[i]); if (!windows[i]) goto cleanup;
    }
    incoming = (WNDPROC)(source_ansi ? GetWindowLongW(windows[0], GWL_WNDPROC) :
        GetWindowLongA(windows[0], GWL_WNDPROC));
    CHECK(HIWORD(incoming) == 0xffff);
    for (i = 1; i < 3; ++i) {
        WNDPROC previous = (WNDPROC)GetWindowLongW(windows[i], GWL_WNDPROC);
        BOOL unicode = IsWindowUnicode(windows[i]);
        before = calls;
        CHECK(SetWindowLongW(windows[i], GWL_WNDPROC, (LONG)incoming) == (LONG)previous);
        owned[i - 1] = (WNDPROC)GetWindowLongA(windows[i], GWL_WNDPROC);
        CHECK(owned[i - 1]);
        CHECK(SetWindowLongW(windows[i], GWL_WNDPROC, (LONG)previous));
        CHECK((WNDPROC)GetWindowLongW(windows[i], GWL_WNDPROC) == previous);
        CHECK(IsWindowUnicode(windows[i]) == unicode);
        CHECK(calls == before);
    }
    CHECK(DestroyWindow(windows[0])); windows[0] = NULL;
    CHECK(UnregisterClassW(MAKEINTATOM(atoms[0]), module)); atoms[0] = 0;
    check_text = TRUE;
    for (i = 1; i < 3; ++i) {
        before_text = text_calls;
        CHECK(CallWindowProcA(owned[i - 1], windows[i], WM_SETTEXT, 0, (LPARAM)"target"));
        CHECK(text_calls == before_text + 1);
        /* Retire one target before calling the other: no shared source or
         * first-destination class lifetime may support the second call. */
        CHECK(DestroyWindow(windows[i])); windows[i] = NULL;
        CHECK(UnregisterClassW(MAKEINTATOM(atoms[i]), module)); atoms[i] = 0;
    }
    check_text = FALSE;
    printf("DESTINATION_LIFETIME source_ansi=%u destination_ansi=%u failures=%u\n",
        source_ansi, destination_ansi, failures);
cleanup:
    check_text = FALSE;
    for (i = 0; i < 3; ++i) {
        if (windows[i]) CHECK(DestroyWindow(windows[i]));
        if (atoms[i]) CHECK(UnregisterClassW(MAKEINTATOM(atoms[i]), module));
    }
}

static DWORD inspect(WNDPROC input, BOOL ansi, const char *phase)
{
    HINSTANCE module = GetModuleHandleW(NULL);
    WNDCLASSEXA a = {sizeof(a)}, out_a = {sizeof(out_a)};
    WNDCLASSEXW w = {sizeof(w)}, out_w = {sizeof(out_w)};
    ATOM atom;
    WNDPROC output = NULL;
    DWORD error;
    a.hInstance=module; a.lpfnWndProc=input;
    a.lpszClassName="T447AddressOwnerScratch";
    w.hInstance=module; w.lpfnWndProc=input;
    w.lpszClassName=L"T447AddressOwnerScratch";
    SetLastError(0);
    atom=ansi ? RegisterClassExA(&a) : RegisterClassExW(&w);
    error=GetLastError();
    if (atom) {
        if (ansi) {
            CHECK(GetClassInfoExA(module,MAKEINTRESOURCEA(atom),&out_a));
            output=out_a.lpfnWndProc;
        } else {
            CHECK(GetClassInfoExW(module,MAKEINTATOM(atom),&out_w));
            output=out_w.lpfnWndProc;
        }
        /* Failed duplicate registration must neither consume the input nor
         * replace the existing registration's procedure. */
        CHECK(!(ansi ? RegisterClassExA(&a) : RegisterClassExW(&w)));
        CHECK(GetLastError()==ERROR_CLASS_ALREADY_EXISTS);
        CHECK(UnregisterClassW(MAKEINTATOM(atom),module));
        CHECK(!GetClassInfoExW(module,L"T447AddressOwnerScratch",&out_w));
    }
    printf("CLASS_ADDRESS phase=%s ansi=%u input=%p atom=%04x output=%p error=%lu\n",
        phase,ansi,input,atom,output,error);
    return (DWORD)output;
}

static void valid_and_retired(BOOL ansi)
{
    HINSTANCE module=GetModuleHandleW(NULL);
    WNDCLASSEXA a={sizeof(a)};
    WNDCLASSEXW w={sizeof(w)};
    HWND window;
    ATOM atom;
    WNDPROC token, expected=ansi ? source_a : source_w;
    unsigned i;
    a.hInstance=module;a.lpfnWndProc=source_a;a.lpszClassName="T447AddressSource";
    w.hInstance=module;w.lpfnWndProc=source_w;w.lpszClassName=L"T447AddressSource";
    atom=ansi ? RegisterClassExA(&a) : RegisterClassExW(&w);
    CHECK(atom!=0);if(!atom)return;
    window=CreateWindowExW(0,MAKEINTATOM(atom),L"",WS_POPUP,0,0,1,1,NULL,NULL,module,NULL);
    CHECK(window!=NULL);
    if(!window){CHECK(UnregisterClassW(MAKEINTATOM(atom),module));return;}
    token=(WNDPROC)(ansi ? GetWindowLongW(window,GWL_WNDPROC) : GetWindowLongA(window,GWL_WNDPROC));
    CHECK(HIWORD(token)==0xffff);
    for(i=0;i<16;++i) {
        CHECK(inspect(token,TRUE,"live-A")== (DWORD)expected);
        CHECK(inspect(token,FALSE,"live-W")== (DWORD)expected);
    }
    CHECK(DestroyWindow(window));
    CHECK(UnregisterClassW(MAKEINTATOM(atom),module));
    /* Observation, not an assertion that stale native tokens remain usable.
     * Native USER is the authority for their validation and expiry. */
    (void)inspect(token,TRUE,"retired-A");
    (void)inspect(token,FALSE,"retired-W");
}

/* Research: a token returned by a known ANSI getter for a Unicode source.
 * Retain native conversion through class registrations without creating
 * carrier HWNDs or mutating the subject's procedure. This does NOT discover
 * the encoding of an arbitrary incoming token; caller provenance is required.
 */
static void ansi_origin_class_lifetime(void)
{
    HINSTANCE module = GetModuleHandleW(NULL);
    WNDCLASSEXW w = {sizeof(w)};
    WNDCLASSEXA a = {sizeof(a)}, observed = {sizeof(observed)};
    const WCHAR *names[] = {L"T447AnsiOriginSource", L"T447AnsiOriginCopy0",
        L"T447AnsiOriginCopy1"};
    ATOM atoms[3] = {0}, subject_atom = 0;
    HWND source = NULL, subject = NULL;
    WNDPROC incoming, copies[2] = {0}, subject_proc;
    unsigned i, before, before_text;
    w.hInstance = module; w.lpfnWndProc = source_w; w.lpszClassName = names[0];
    atoms[0] = RegisterClassExW(&w);
    CHECK(atoms[0]); if (!atoms[0]) goto cleanup;
    source = CreateWindowExW(0, MAKEINTATOM(atoms[0]), L"", WS_POPUP,
        0, 0, 1, 1, NULL, NULL, module, NULL);
    CHECK(source); if (!source) goto cleanup;
    a.hInstance = module; a.lpfnWndProc = source_a;
    a.lpszClassName = "T447AnsiOriginSubject";
    subject_atom = RegisterClassExA(&a);
    CHECK(subject_atom); if (!subject_atom) goto cleanup;
    subject = CreateWindowExA(0, MAKEINTRESOURCEA(subject_atom), "", WS_POPUP,
        0, 0, 1, 1, NULL, NULL, module, NULL);
    CHECK(subject); if (!subject) goto cleanup;
    subject_proc = (WNDPROC)GetWindowLongA(subject, GWL_WNDPROC);
    incoming = (WNDPROC)GetWindowLongA(source, GWL_WNDPROC);
    CHECK(HIWORD(incoming) == 0xffff);
    before = calls;
    for (i = 1; i < 3; ++i) {
        w.lpfnWndProc = incoming; w.lpszClassName = names[i];
        atoms[i] = RegisterClassExW(&w);
        CHECK(atoms[i]); if (!atoms[i]) goto cleanup;
        CHECK(GetClassInfoExA(module, MAKEINTRESOURCEA(atoms[i]), &observed));
        copies[i - 1] = observed.lpfnWndProc;
        CHECK(copies[i - 1] && HIWORD(copies[i - 1]) == 0xffff);
    }
    CHECK(calls == before);
    CHECK((WNDPROC)GetWindowLongA(subject, GWL_WNDPROC) == subject_proc);
    CHECK(!IsWindowUnicode(subject));
    CHECK(DestroyWindow(source)); source = NULL;
    CHECK(UnregisterClassW(MAKEINTATOM(atoms[0]), module)); atoms[0] = 0;
    check_text = TRUE;
    for (i = 1; i < 3; ++i) {
        before_text = text_calls;
        CHECK(CallWindowProcA(copies[i - 1], subject, WM_SETTEXT, 0, (LPARAM)"target"));
        CHECK(text_calls == before_text + 1);
        CHECK(UnregisterClassW(MAKEINTATOM(atoms[i]), module)); atoms[i] = 0;
    }
    CHECK((WNDPROC)GetWindowLongA(subject, GWL_WNDPROC) == subject_proc);
    CHECK(!IsWindowUnicode(subject));
    printf("ANSI_ORIGIN_CLASS_LIFETIME carrier_windows=0 subject_unchanged=1 failures=%u\n", failures);
cleanup:
    check_text = FALSE;
    if (source) CHECK(DestroyWindow(source));
    if (subject) CHECK(DestroyWindow(subject));
    if (subject_atom) CHECK(UnregisterClassA(MAKEINTRESOURCEA(subject_atom), module));
    for (i = 0; i < 3; ++i)
        if (atoms[i]) CHECK(UnregisterClassW(MAKEINTATOM(atoms[i]), module));
}

/* Native research only: can a thread return observer see destruction even
 * when a replacement procedure never calls the original? No product callback,
 * guest scheduler or original-control finalizer is simulated here. */
static HWND lifetime_subject;
static unsigned lifetime_before, lifetime_body, lifetime_after, lifetime_order;
static BOOL lifetime_fail_create, lifetime_cancel_destroy, lifetime_synthetic;
static LRESULT CALLBACK lifetime_before_proc(int code, WPARAM p, LPARAM l)
{
    if (code >= 0) {
        const CWPSTRUCT *m = (const CWPSTRUCT *)l;
        if (m->hwnd == lifetime_subject && m->message == WM_NCDESTROY) {
            ++lifetime_before; CHECK(lifetime_order == 0); lifetime_order = 1;
        }
    }
    return CallNextHookEx(NULL, code, p, l);
}
static LRESULT CALLBACK lifetime_after_proc(int code, WPARAM p, LPARAM l)
{
    if (code >= 0) {
        const CWPRETSTRUCT *m = (const CWPRETSTRUCT *)l;
        if (m->hwnd == lifetime_subject && m->message == WM_NCDESTROY) {
            ++lifetime_after; CHECK(lifetime_order == 2); lifetime_order = 3;
            CHECK(m->lResult == 73);
            printf("TERMINAL_RETURN synthetic=%u live_hwnd=%u result=%ld send_flags=%lu\n",
                lifetime_synthetic, IsWindow(m->hwnd), m->lResult, InSendMessageEx(NULL));
        }
    }
    return CallNextHookEx(NULL, code, p, l);
}
static LRESULT CALLBACK lifetime_cbt_proc(int code, WPARAM p, LPARAM l)
{
    if (code == HCBT_DESTROYWND && (HWND)p == lifetime_subject && lifetime_cancel_destroy)
        return 1;
    return CallNextHookEx(NULL, code, p, l);
}
static LRESULT CALLBACK lifetime_window(HWND w, UINT m, WPARAM p, LPARAM l)
{
    if (m == WM_NCCREATE) lifetime_subject = w;
    if (m == WM_CREATE && lifetime_fail_create) return -1;
    if (m == WM_NCDESTROY) {
        ++lifetime_body; CHECK(lifetime_order == 1); lifetime_order = 2;
        return 73; /* Deliberately do not forward terminal processing. */
    }
    return DefWindowProcW(w, m, p, l);
}
static void terminal_return_observer(void)
{
    WNDCLASSEXW c = {sizeof(c)};
    HHOOK before, after, cbt;
    ATOM atom;
    HWND w;
    unsigned mode;
    c.hInstance = GetModuleHandleW(NULL); c.lpfnWndProc = lifetime_window;
    c.lpszClassName = L"T447TerminalReturnSubject";
    atom = RegisterClassExW(&c); CHECK(atom); if (!atom) return;
    before = SetWindowsHookExW(WH_CALLWNDPROC, lifetime_before_proc, NULL, GetCurrentThreadId());
    after = SetWindowsHookExW(WH_CALLWNDPROCRET, lifetime_after_proc, NULL, GetCurrentThreadId());
    cbt = SetWindowsHookExW(WH_CBT, lifetime_cbt_proc, NULL, GetCurrentThreadId());
    CHECK(before && after && cbt);
    if (before && after && cbt) for (mode = 0; mode < 4; ++mode) {
        lifetime_subject = NULL;
        lifetime_before = lifetime_body = lifetime_after = lifetime_order = 0;
        lifetime_fail_create = mode == 1; lifetime_cancel_destroy = FALSE;
        lifetime_synthetic = FALSE;
        w = CreateWindowExW(0, MAKEINTATOM(atom), L"", WS_POPUP,
            0, 0, 1, 1, NULL, NULL, c.hInstance, NULL);
        if (mode == 1) CHECK(!w);
        else {
            CHECK(w);
            if (w && mode == 3) {
                /* Negative: an ordinary same-thread send looks terminal to
                 * the observer, but USER has not destroyed the HWND. */
                lifetime_synthetic = TRUE;
                CHECK(SendMessageW(w, WM_NCDESTROY, 0, 0) == 73);
                CHECK(IsWindow(w));
                CHECK(lifetime_before == 1 && lifetime_body == 1 && lifetime_after == 1);
                CHECK(lifetime_order == 3);
                printf("TERMINAL_SYNTHETIC indistinguishable_message=1 still_alive=1\n");
                lifetime_synthetic = FALSE;
                lifetime_before = lifetime_body = lifetime_after = lifetime_order = 0;
            }
            if (w && mode == 2) {
                lifetime_cancel_destroy = TRUE;
                CHECK(!DestroyWindow(w)); CHECK(IsWindow(w));
                CHECK(!lifetime_before && !lifetime_body && !lifetime_after);
                lifetime_cancel_destroy = FALSE;
            }
            if (w) CHECK(DestroyWindow(w));
        }
        CHECK(lifetime_before == 1 && lifetime_body == 1 && lifetime_after == 1);
        CHECK(lifetime_order == 3);
        CHECK(!IsWindow(lifetime_subject));
        printf("TERMINAL_OBSERVER mode=%u before=%u body=%u after=%u failures=%u\n",
            mode, lifetime_before, lifetime_body, lifetime_after, failures);
    }
    if (cbt) CHECK(UnhookWindowsHookEx(cbt));
    if (after) CHECK(UnhookWindowsHookEx(after));
    if (before) CHECK(UnhookWindowsHookEx(before));
    CHECK(UnregisterClassW(MAKEINTATOM(atom), c.hInstance));
    lifetime_subject = NULL;
}

/* Source-led counterexample to treating a private project lock as native
 * USER serialization. The actual destination is held at a deterministic
 * transaction barrier while its native owner reads, sends and subclasses.
 * A successful probe establishes the risks, not a safe product repair. */
#define TRANSIENT_MESSAGE (WM_APP + 109)
static LRESULT CALLBACK transaction_a(HWND w, UINT m, WPARAM p, LPARAM l)
{ return m == TRANSIENT_MESSAGE ? 0xA1 : DefWindowProcA(w,m,p,l); }
static LRESULT CALLBACK transaction_b(HWND w, UINT m, WPARAM p, LPARAM l)
{ return m == TRANSIENT_MESSAGE ? 0xB2 : DefWindowProcW(w,m,p,l); }
static LRESULT CALLBACK transaction_c(HWND w, UINT m, WPARAM p, LPARAM l)
{ return m == TRANSIENT_MESSAGE ? 0xC3 : DefWindowProcA(w,m,p,l); }
typedef struct destination_transaction {
    HWND destination;
    WNDPROC incoming;
    HANDLE installed, resume;
    DWORD install_error, restore_error;
    BOOL installed_ok, restored_ok;
} destination_transaction;
static DWORD WINAPI transact_destination(void *argument)
{
    destination_transaction *t = argument;
    LONG previous;
    SetLastError(ERROR_SUCCESS);
    previous = SetWindowLongA(t->destination,GWL_WNDPROC,(LONG)t->incoming);
    t->install_error = GetLastError(); t->installed_ok = previous != 0;
    if (!SetEvent(t->installed)) return 1;
    if (WaitForSingleObject(t->resume,10000) != WAIT_OBJECT_0) return 2;
    if (t->installed_ok) {
        SetLastError(ERROR_SUCCESS);
        t->restored_ok = SetWindowLongA(t->destination,GWL_WNDPROC,previous) != 0;
        t->restore_error = GetLastError();
    }
    return 0;
}
static void destination_transaction_observability(void)
{
    WNDCLASSEXA a = {sizeof(a)};
    WNDCLASSEXW w = {sizeof(w)};
    destination_transaction t = {0};
    HINSTANCE module = GetModuleHandleW(NULL);
    HANDLE thread = NULL;
    HWND source = NULL;
    ATOM atom_a = 0, atom_w = 0;
    BOOL read_transient = FALSE, encoding_transient = FALSE, update_lost = FALSE;
    LRESULT delivered = 0;
    DWORD result = 1;
    a.hInstance = module; a.lpfnWndProc = transaction_a;
    a.lpszClassName = "T447ConcurrentDestination";
    w.hInstance = module; w.lpfnWndProc = transaction_b;
    w.lpszClassName = L"T447ConcurrentSource";
    atom_a = RegisterClassExA(&a); atom_w = RegisterClassExW(&w);
    CHECK(atom_a && atom_w); if (!atom_a || !atom_w) goto cleanup;
    source = CreateWindowExW(0,MAKEINTATOM(atom_w),L"",WS_POPUP,
        0,0,1,1,NULL,NULL,module,NULL);
    t.destination = CreateWindowExA(0,MAKEINTRESOURCEA(atom_a),"",WS_POPUP,
        0,0,1,1,NULL,NULL,module,NULL);
    CHECK(source && t.destination); if (!source || !t.destination) goto cleanup;
    t.incoming = (WNDPROC)GetWindowLongA(source,GWL_WNDPROC);
    CHECK(t.incoming && HIWORD(t.incoming) == 0xffff);
    CHECK(!IsWindowUnicode(t.destination));
    t.installed = CreateEventW(NULL,TRUE,FALSE,NULL);
    t.resume = CreateEventW(NULL,TRUE,FALSE,NULL);
    CHECK(t.installed && t.resume); if (!t.installed || !t.resume) goto cleanup;
    thread = CreateThread(NULL,0,transact_destination,&t,0,NULL);
    CHECK(thread); if (!thread) goto cleanup;
    CHECK(WaitForSingleObject(t.installed,10000) == WAIT_OBJECT_0);
    CHECK(t.installed_ok);
    if (t.installed_ok) {
        read_transient = (WNDPROC)GetWindowLongW(t.destination,GWL_WNDPROC) == transaction_b;
        encoding_transient = IsWindowUnicode(t.destination);
        delivered = SendMessageW(t.destination,TRANSIENT_MESSAGE,0,0);
        CHECK(read_transient && encoding_transient && delivered == 0xB2);
        CHECK(SetWindowLongA(t.destination,GWL_WNDPROC,(LONG)transaction_c));
        CHECK(SendMessageA(t.destination,TRANSIENT_MESSAGE,0,0) == 0xC3);
    }
    CHECK(SetEvent(t.resume));
    if (WaitForSingleObject(thread,10000) != WAIT_OBJECT_0) {
        printf("FAIL transaction helper did not terminate\n"); ExitProcess(2);
    }
    CHECK(GetExitCodeThread(thread,&result) && result == 0);
    CHECK(t.restored_ok);
    update_lost = (WNDPROC)GetWindowLongA(t.destination,GWL_WNDPROC) == transaction_a;
    CHECK(update_lost && !IsWindowUnicode(t.destination));
    CHECK(SendMessageA(t.destination,TRANSIENT_MESSAGE,0,0) == 0xA1);
    printf("TRANSIENT_DESTINATION read=%u encoding=%u message_result=%lX update_lost=%u install_error=%lu restore_error=%lu failures=%u\n",
        read_transient,encoding_transient,(unsigned long)delivered,update_lost,
        t.install_error,t.restore_error,failures);
cleanup:
    if (thread) CloseHandle(thread);
    if (t.resume) CloseHandle(t.resume);
    if (t.installed) CloseHandle(t.installed);
    if (t.destination) CHECK(DestroyWindow(t.destination));
    if (source) CHECK(DestroyWindow(source));
    if (atom_a) CHECK(UnregisterClassA(MAKEINTRESOURCEA(atom_a),module));
    if (atom_w) CHECK(UnregisterClassW(MAKEINTATOM(atom_w),module));
}

int __cdecl main(void)
{
    valid_and_retired(TRUE);
    valid_and_retired(FALSE);
    destination_lifetime(TRUE, TRUE);
    destination_lifetime(TRUE, FALSE);
    destination_lifetime(FALSE, TRUE);
    destination_lifetime(FALSE, FALSE);
    ansi_origin_class_lifetime();
    terminal_return_observer();
    destination_transaction_observability();
    /* These are unowned bit patterns, not assumed-valid native handles.
     * Do not call them. Record rejection versus unchanged acceptance. */
    (void)inspect((WNDPROC)0xffff0b98u,TRUE,"guest-shaped");
    (void)inspect((WNDPROC)0xffff0000u,TRUE,"zero-index");
    (void)inspect((WNDPROC)0xffffffffu,TRUE,"max-index");
    printf("CLASS_ADDRESS_OWNER failures=%u\n",failures);
    return failures ? 1 : 0;
}
