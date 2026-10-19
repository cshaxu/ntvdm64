#include "wow_standard_class_bindings.h"
#include "wow_window_words_binding.h"
#include "wow_edit_entry_bindings.h"
#include "wow_dialog_state_bindings.h"
#include "opennt-host/windows/core/ntuser/inc/user.h"
#include <stdio.h>
#include <string.h>
static unsigned errors;
#define CHECK(x) do { if (!(x)) { ++errors; fprintf(stderr, "FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)
static DWORD WINAPI unexpected_callback(HWND w, UINT m, UINT p, LONG l, DWORD f, PVOID v)
{ (void)w; (void)m; (void)p; (void)l; (void)f; (void)v; CHECK(FALSE); return 0; }
static PVOID WINAPI allocate(SIZE_T size)
{ return HeapAlloc(GetProcessHeap(), 0, size); }
static void WINAPI release(PVOID memory)
{ HeapFree(GetProcessHeap(), 0, memory); }
static BOOL WINAPI unexpected_destroy(wow_cleanup_context *context,
    wow_class_lookup_entry **entry)
{ (void)context; (void)entry; CHECK(FALSE); return FALSE; }

static void verify_recovered_targets(wow_class_lookup_context *classes)
{
    static const WORD cases[] = {
        FNID_EDIT, FNID_COMBOBOX, FNID_LISTBOX, FNID_COMBOLISTBOX, FNID_DIALOG,
        FNID_BUTTON
    };
    unsigned i;
    for (i = 0; i < ARRAYSIZE(cases); ++i) {
        WORD fnid = 0;
        WNDPROC canonical = NULL;
        WNDPROC ansi = cases[i] == FNID_DIALOG ? wow_dialog_window_procA :
            wow_control_client_procedure(cases[i], FALSE);
        WNDPROC wide = cases[i] == FNID_DIALOG ? wow_dialog_window_procW :
            wow_control_client_procedure(cases[i], TRUE);
        CHECK(ansi != NULL && wide != NULL && ansi != wide);
        SetLastError(0x2468);
        /* Optimized product links may fold identical original list bodies
         * and wrappers. No FNID may be guessed from an ambiguous address. */
        if ((cases[i] == FNID_LISTBOX || cases[i] == FNID_COMBOLISTBOX) &&
                wow_control_client_procedure(FNID_LISTBOX, FALSE) ==
                wow_control_client_procedure(FNID_COMBOLISTBOX, FALSE)) {
            CHECK(wow_control_client_procedure(FNID_LISTBOX, TRUE) ==
                wow_control_client_procedure(FNID_COMBOLISTBOX, TRUE));
            CHECK(!wow_class_words_match_native(classes, ansi, FALSE,
                &fnid, &canonical));
            CHECK(!wow_class_words_match_native(classes, wide, TRUE,
                &fnid, &canonical));
            CHECK(fnid == 0 && canonical == NULL && GetLastError() == 0x2468);
            continue;
        }
        CHECK(wow_class_words_match_native(classes, ansi, FALSE,
            &fnid, &canonical));
        CHECK(fnid == cases[i] && canonical == ansi);
        CHECK(GetLastError() == 0x2468);
        fnid = 0; canonical = NULL;
        CHECK(wow_class_words_match_native(classes, wide, TRUE,
            &fnid, &canonical));
        CHECK(fnid == cases[i] && canonical == ansi);
        CHECK(GetLastError() == 0x2468);
        fnid = 0; canonical = NULL;
        CHECK(!wow_class_words_match_native(classes, wide, FALSE,
            &fnid, &canonical));
        CHECK(fnid == 0 && canonical == NULL);
        CHECK(GetLastError() == 0x2468);
    }
}

static void verify_original_cleanup(wow_class_lookup_context *classes)
{
    wow_cleanup_shared shared = {0};
    wow_cleanup_context context = {0};
    wow_task_order_process process = {0};
    wow_task_order_thread thread = {0};
    wow_cleanup_handle handles[1] = {0};
    wow_class_lookup_entry *entry;
    NEMODULESEG selector = {0};
    wow_standard_class_view other;
    WNDCLASSEXA shadow = {sizeof(shadow)};
    UNICODE_STRING shadow_name;
    DWORD shadow_words[2] = {0x12370056u, 0};
    wow_window_dispatch_target shadow_target = {0x92370056u, unexpected_callback, TRUE};
    wow_class_words_binding *private_class;
    ATOM shadow_atom;
    DWORD unrelated_module = 0, module_attempt;
    unsigned restored = 0;
    process.classes = classes;
    thread.ppi = &process;
    context.thread = &thread;
    context.shared = &shared;
    context.handles.aheList = handles;
    context.allocate = allocate;
    context.release = release;
    context.destroy_class = unexpected_destroy;
    CHECK(!wow_standard_classes_bind_cleanup(NULL));
    CHECK(wow_standard_classes_bind_cleanup(&context));
    CHECK(wow_standard_class_describe("EDIT", &other));
    CHECK(shared.atomSysClass[ICLS_EDIT] == other.atom);
    {
        wow_standard_class_view combo_list;
        CHECK(wow_standard_class_describe("COMBOLBOX", &combo_list));
        CHECK(shared.atomSysClass[ICLS_COMBOLISTBOX] == combo_list.atom);
    }
    shadow.hInstance = GetModuleHandleA(NULL);
    shadow.lpfnWndProc = DefWindowProcA;
    shadow.lpszClassName = "BUTTON";
    RtlInitUnicodeString(&shadow_name, L"BUTTON");
    shadow_atom = wow_class_words_register_target(classes, &shadow, &shadow_name,
        shadow_words, NULL, &shadow_target);
    CHECK(shadow_atom != 0);
    selector.ns_handle = 0x1236;
    for (entry = classes->pclsPublicList; entry; entry = entry->next) {
        wow_class_words_binding *binding = wow_class_words_acquire(classes,
            entry->atomClassName, classes->client_module);
        DWORD old;
        BOOL previous_guest;
        CHECK(binding != NULL);
        if (!binding) continue;
        /* A previously installed native CPD must not overwrite the immutable
         * original standard-class target used after module unload. */
        CHECK(wow_class_words_exchange_procedure(binding,
            (DWORD)other.native.lpfnWndProc, FALSE, &old, &previous_guest));
        CHECK(wow_class_words_exchange_procedure(binding, 0x92370056u, TRUE,
            &old, &previous_guest));
        entry->adwWOW[0] = 0x12370056u;
        wow_class_words_release(binding);
    }
    /* This row tests selector/procedure restoration, not module destruction.
     * A fixed module1 accidentally matches a native image relocated to10000h.
     * Select a genuinely unrelated WORD from the actual retained class list. */
    for (module_attempt = 0; module_attempt <= 0xffffu; ++module_attempt) {
        BOOL conflict = FALSE;
        unsigned list;
        /* Start with the actual occupied value, exercising the avoidance
         * even when ASLR does not place this particular image at10000h. */
        unrelated_module = (HIWORD(shadow.hInstance) + module_attempt) & 0xffffu;
        for (list = 0; list < 2; ++list)
            for (entry = list ? classes->pclsPublicList : classes->pclsPrivateList;
                    entry; entry = entry->next)
                if (HIWORD(entry->hModule) == unrelated_module) conflict = TRUE;
        if (!conflict) break;
    }
    CHECK(module_attempt > 0 && module_attempt <= 0xffffu);
    CHECK(_WOWCleanup((HANDLE)(ULONG_PTR)unrelated_module, 0, &selector, 1, &context));
    fprintf(stderr, "WOW_STANDARD_CLEANUP_MODULE image=%p excluded=%lu\n",
        GetModuleHandleA(NULL), unrelated_module);
    private_class = wow_class_words_acquire(classes, shadow_atom, shadow.hInstance);
    CHECK(private_class != NULL);
    if (private_class) {
        wow_window_dispatch_target restored_target;
        wow_standard_class_view button;
        CHECK(wow_class_words_value(private_class)[0] == 0);
        CHECK(wow_standard_class_describe("BUTTON", &button));
        CHECK(wow_class_words_target(private_class, &restored_target));
        /* cleanup.c selects by atom for both private and public classes.
         * A private BUTTON shadow restores the same recovered W procedure,
         * not a second native-control behavior owner. */
        CHECK(restored_target.procedure ==
            (DWORD)wow_control_client_procedure(FNID_BUTTON, TRUE));
        wow_class_words_release(private_class);
    }
    if (shadow_atom)
        CHECK(wow_class_words_unregister(classes, shadow_atom, shadow.hInstance, NULL));
    for (entry = classes->pclsPublicList; entry; entry = entry->next) {
        wow_class_words_binding *binding = wow_class_words_acquire(classes,
            entry->atomClassName, classes->client_module);
        wow_standard_class_view expected;
        wow_window_dispatch_target snapshot;
        DWORD previous;
        BOOL previous_guest;
        CHECK(binding != NULL);
        if (!binding) continue;
        CHECK(entry->adwWOW[0] == 0);
        CHECK(wow_standard_class_describe(MAKEINTATOM(entry->atomClassName), &expected));
        CHECK(wow_class_words_target(binding, &snapshot));
        CHECK(snapshot.procedure == (entry->fnid == FNID_EDIT ?
            (DWORD)DefWindowProcW : entry->fnid == FNID_DIALOG ?
            (DWORD)wow_dialog_window_procW : wow_control_client_procedure(entry->fnid, TRUE) ?
            (DWORD)wow_control_client_procedure(entry->fnid, TRUE) : (DWORD)expected.native.lpfnWndProc));
        if (entry->fnid == FNID_BUTTON) {
            WW words = {0};
            HWND window = CreateWindowExA(0, "BUTTON", "", BS_CHECKBOX,
                0, 0, 16, 16, NULL, NULL, GetModuleHandleA(NULL), NULL);
            CHECK(window != NULL);
            if (window) {
                /* This row checks immutable native-class forwarding on an
                 * implicitly native-created control, not recovered BUTTON
                 * behavior. Recovered cleanup targets are asserted above;
                 * their real creation/retirement is covered by the CCPU
                 * driver's original-control group. Never attach an original
                 * private-state procedure to this native BUTTON state. */
                wow_window_dispatch_target native_target = {
                    (DWORD)expected.native.lpfnWndProc, NULL
                };
                wow_class_words_binding *window_class = wow_class_words_acquire(
                    classes, entry->atomClassName, classes->client_module);
                CHECK(window_class != NULL);
                if (wow_window_words_attach_target(window, &words, window_class, &native_target)) {
                    SendMessageA(window, BM_SETCHECK, BST_CHECKED, 0);
                    CHECK(SendMessageA(window, BM_GETCHECK, 0, 0) == BST_CHECKED);
                    CHECK(wow_class_words_exchange_procedure(binding, (DWORD)DefWindowProcA,
                        FALSE, &previous, &previous_guest));
                    CHECK(wow_class_call_original_native(window, BM_GETCHECK, 0, 0) == BST_CHECKED);
                    CHECK(wow_class_words_exchange_procedure(binding, snapshot.procedure,
                        snapshot.wow_encoded, &previous, &previous_guest));
                } else {
                    CHECK(FALSE);
                    wow_class_words_release(window_class);
                }
                CHECK(DestroyWindow(window));
                CHECK(entry->cWndReferenceCount == 0);
            }
        }
        CHECK(wow_class_words_exchange_procedure(binding, (DWORD)DefWindowProcA,
            FALSE, &previous, &previous_guest));
        CHECK(previous == snapshot.procedure);
        CHECK(wow_class_words_exchange_procedure(binding,
            (DWORD)expected.native.lpfnWndProc, FALSE, &previous, &previous_guest));
        wow_class_words_release(binding);
        ++restored;
    }
    CHECK(restored == 9);
    fprintf(stderr, "WOW_STANDARD_CLEANUP errors=%u restored=%u original_policy=1\n", errors, restored);
    fprintf(stderr, "WOW_STANDARD_PRIVATE_CLEANUP errors=%u\n", errors);
}
int __cdecl main(void)
{
    static const struct { LPCSTR name; WORD fnid; } cases[] = {
        {"bUtToN", FNID_BUTTON}, {"COMBOBOX", FNID_COMBOBOX},
        {"COMBOLBOX", FNID_COMBOLISTBOX}, {"EDIT", FNID_EDIT},
        {"LISTBOX", FNID_LISTBOX}, {"MDICLIENT", FNID_MDICLIENT},
        {"SCROLLBAR", FNID_SCROLLBAR}, {"STATIC", FNID_STATIC},
        {"#32770", FNID_DIALOG}
    };
    wow_standard_class_view named, atom, unchanged;
    wow_class_lookup_context classes = {0};
    wow_class_words_binding *retained = NULL;
    wow_class_lookup_entry *public_head;
    WNDCLASSEXA app = {sizeof(app)};
    UNICODE_STRING app_name;
    DWORD app_words[2] = {0x12345678u, 0};
    ATOM app_atom;
    HHOOK terminal = SetWindowsHookExA(WH_CALLWNDPROCRET,
        wow_window_terminal_observer, NULL, GetCurrentThreadId());
    unsigned i, resolved = 0;
    CHECK(terminal != NULL);
    if (!terminal) return 1;
    memset(&unchanged, 0x5a, sizeof(unchanged));
    named = unchanged;
    CHECK(!wow_standard_class_describe("Not a standard class", &named));
    CHECK(!memcmp(&named, &unchanged, sizeof(named)));
    CHECK(!wow_standard_class_describe(NULL, &named));
    CHECK(!wow_standard_class_describe("BUTTON", NULL));
    for (i = 0; i < ARRAYSIZE(cases); ++i) {
        BOOL ok = wow_standard_class_describe(cases[i].name, &named);
        CHECK(ok);
        if (!ok) continue;
        CHECK(named.fnid == cases[i].fnid && named.atom && named.native.lpfnWndProc);
        CHECK(wow_standard_class_describe(MAKEINTATOM(named.atom), &atom));
        CHECK(atom.atom == named.atom && atom.fnid == named.fnid);
        CHECK(atom.native.lpfnWndProc == named.native.lpfnWndProc);
        ++resolved;
    }
    classes.client_module = GetModuleHandleW(L"user32.dll");
    CHECK(wow_standard_classes_prepare(&classes, unexpected_callback));
    CHECK(classes.pclsPrivateList == NULL && classes.system_classes == NULL);
    public_head = classes.pclsPublicList;
    CHECK(public_head != NULL);
    CHECK(!wow_standard_classes_prepare(&classes, NULL));
    CHECK(classes.pclsPublicList == public_head);
    verify_recovered_targets(&classes);
    for (i = 0; i < ARRAYSIZE(cases); ++i) {
        wow_class_words_binding *binding, *again;
        wow_window_dispatch_target target;
        DWORD *words;
        CHECK(wow_standard_class_describe(cases[i].name, &named));
        binding = wow_class_words_acquire(&classes, named.atom, (HANDLE)0x12345678u);
        CHECK(binding != NULL);
        if (!binding) continue;
        again = wow_class_words_acquire(&classes, named.atom, classes.client_module);
        CHECK(again == binding);
        words = wow_class_words_value(binding);
        CHECK(words && words[0] == 0 && words[1] == 0);
        CHECK(wow_class_words_entry(binding)->fnid == named.fnid);
        CHECK(wow_class_words_entry(binding)->hTaskWow == 0);
        CHECK(wow_class_words_target(binding, &target));
        CHECK(target.procedure == (DWORD)(wow_control_client_procedure(named.fnid, FALSE) ?
            wow_control_client_procedure(named.fnid, FALSE) : named.fnid == FNID_DIALOG ?
            wow_dialog_window_procA : named.native.lpfnWndProc));
        CHECK(HIWORD(wow_class_words_entry(binding)->lpfnWndProc) != 0xffff);
        wow_class_words_release(again);
        if (!i) retained = binding;
        else wow_class_words_release(binding);
    }
    verify_original_cleanup(&classes);
    /* Original private-before-public lookup must still allow an application's
     * own BUTTON registration to shadow the imported native control. */
    app.hInstance = GetModuleHandleA(NULL);
    app.lpfnWndProc = DefWindowProcA;
    app.lpszClassName = "BUTTON";
    RtlInitUnicodeString(&app_name, L"BUTTON");
    app_atom = wow_class_words_register(&classes, &app, &app_name, app_words, NULL);
    CHECK(app_atom != 0);
    if (app_atom) {
        wow_class_words_binding *shadow = wow_class_words_acquire(&classes,
            app_atom, app.hInstance);
        CHECK(shadow && shadow != retained);
        if (shadow) CHECK(wow_class_words_value(shadow)[0] == app_words[0]);
        wow_class_words_release(shadow);
        CHECK(wow_class_words_unregister(&classes, app_atom, app.hInstance, NULL));
    }
    if (retained) {
        /* Storage borrow and native-window association are separate domains. */
        wow_class_words_window_association(retained, TRUE);
        CHECK(!wow_class_words_forget_native(&classes));
        CHECK(GetLastError() == ERROR_CLASS_HAS_WINDOWS);
        CHECK(classes.pclsPublicList == public_head);
        wow_class_words_window_association(retained, FALSE);
        wow_class_words_value(retained)[0] = 0x98765432u;
    }
    app.style = CS_GLOBALCLASS;
    app.lpszClassName = "S2_PUBLIC_CONTROL_STORAGE";
    RtlInitUnicodeString(&app_name, L"S2_PUBLIC_CONTROL_STORAGE");
    app_atom = wow_class_words_register(&classes, &app, &app_name, app_words, NULL);
    CHECK(app_atom != 0);
    CHECK(wow_class_words_forget_native(&classes));
    CHECK(classes.pclsPublicList && classes.pclsPublicList->atomClassName == app_atom &&
        classes.pclsPublicList->next == NULL);
    if (retained) {
        CHECK(wow_class_words_value(retained)[0] == 0x98765432u);
        wow_class_words_release(retained);
    }
    if (app_atom)
        CHECK(wow_class_words_unregister(&classes, app_atom, app.hInstance, NULL));
    CHECK(classes.pclsPublicList == NULL && classes.pclsPrivateList == NULL);
    for (i = 0; i < ARRAYSIZE(cases); ++i)
        CHECK(wow_standard_class_describe(cases[i].name, &named));
    fprintf(stderr, "WOW_STANDARD_CLASS errors=%u resolved=%u original_table=1\n", errors, resolved);
    fprintf(stderr, "WOW_STANDARD_STORAGE errors=%u classes=9 precedence=1 retirement=1\n", errors);
    CHECK(UnhookWindowsHookEx(terminal));
    return errors ? 1 : 0;
}
