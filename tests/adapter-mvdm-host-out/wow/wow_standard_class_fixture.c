#include "wow_standard_class_bindings.h"
#include "wow_window_words_binding.h"
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
    wow_window_dispatch_target shadow_target = {0x92370056u, unexpected_callback};
    wow_class_words_binding *private_class;
    ATOM shadow_atom;
    unsigned restored = 0;
    process.classes = classes;
    thread.ppi = &process;
    context.thread = &thread;
    context.shared = &shared;
    context.handles.aheList = handles;
    context.allocate = allocate;
    context.release = release;
    CHECK(!wow_standard_classes_bind_cleanup(NULL));
    CHECK(wow_standard_classes_bind_cleanup(&context));
    CHECK(wow_standard_class_describe("EDIT", &other));
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
        CHECK(binding != NULL);
        if (!binding) continue;
        /* A previously installed native CPD must not overwrite the immutable
         * original standard-class target used after module unload. */
        CHECK(wow_class_words_exchange_procedure(binding,
            (DWORD)other.native.lpfnWndProc, &old));
        CHECK(wow_class_words_exchange_procedure(binding, 0x92370056u, &old));
        entry->adwWOW[0] = 0x12370056u;
        wow_class_words_release(binding);
    }
    CHECK(_WOWCleanup((HANDLE)1, 0, &selector, 1, &context));
    private_class = wow_class_words_acquire(classes, shadow_atom, shadow.hInstance);
    CHECK(private_class != NULL);
    if (private_class) {
        wow_window_dispatch_target restored_target;
        wow_standard_class_view button;
        CHECK(wow_class_words_value(private_class)[0] == 0);
        CHECK(wow_standard_class_describe("BUTTON", &button));
        CHECK(wow_class_words_target(private_class, &restored_target));
        CHECK(restored_target.procedure == (DWORD)button.native.lpfnWndProc);
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
        CHECK(binding != NULL);
        if (!binding) continue;
        CHECK(entry->adwWOW[0] == 0);
        CHECK(wow_standard_class_describe(MAKEINTATOM(entry->atomClassName), &expected));
        CHECK(wow_class_words_target(binding, &snapshot));
        CHECK(snapshot.procedure == (entry->fnid == FNID_EDIT ?
            (DWORD)DefWindowProcW : (DWORD)expected.native.lpfnWndProc));
        if (entry->fnid == FNID_BUTTON) {
            WW words = {0};
            HWND window = CreateWindowExA(0, "BUTTON", "", BS_CHECKBOX,
                0, 0, 16, 16, NULL, NULL, GetModuleHandleA(NULL), NULL);
            CHECK(window != NULL);
            if (window) {
                wow_class_words_binding *window_class = wow_class_words_acquire(
                    classes, entry->atomClassName, classes->client_module);
                CHECK(window_class != NULL);
                if (wow_window_words_attach_target(window, &words, window_class, &snapshot)) {
                    CHECK(SetWindowLongA(window, GWL_WNDPROC, (LONG)wow_window_native_proc) != 0);
                    SendMessageA(window, BM_SETCHECK, BST_CHECKED, 0);
                    CHECK(SendMessageA(window, BM_GETCHECK, 0, 0) == BST_CHECKED);
                    CHECK(wow_class_words_exchange_procedure(binding, (DWORD)DefWindowProcA, &previous));
                    CHECK(wow_class_call_original_native(window, BM_GETCHECK, 0, 0) == BST_CHECKED);
                    CHECK(wow_class_words_exchange_procedure(binding, snapshot.procedure, &previous));
                } else {
                    CHECK(FALSE);
                    wow_class_words_release(window_class);
                }
                CHECK(DestroyWindow(window));
                CHECK(entry->cWndReferenceCount == 0);
            }
        }
        CHECK(wow_class_words_exchange_procedure(binding, (DWORD)DefWindowProcA, &previous));
        CHECK(previous == snapshot.procedure);
        CHECK(wow_class_words_exchange_procedure(binding,
            (DWORD)expected.native.lpfnWndProc, &previous));
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
    unsigned i, resolved = 0;
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
        CHECK(target.procedure == (DWORD)named.native.lpfnWndProc);
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
    return errors ? 1 : 0;
}
