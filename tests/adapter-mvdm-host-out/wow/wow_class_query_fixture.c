#include "wow_class_client_bindings.h"
#include "wow_user_private_access.h"
#include "wow_user_runtime.h"
#include "wow_window_words_binding.h"
#include <stdio.h>
#include <string.h>

static unsigned errors, queries;
extern DWORD wow_class_fixture_words[2], wow_class_fixture_task;
extern unsigned wow_class_fixture_publications;
extern BOOL wow_class_fixture_publication_fail;
#define CHECK(x) do { if (!(x)) { ++errors; fprintf(stderr, "FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)
static DWORD WINAPI version(HANDLE module) { (void)module; return 0x030a; }
static DWORD WINAPI unexpected(HWND w, UINT m, UINT p, LONG l, DWORD f, PVOID v)
{ (void)w;(void)m;(void)p;(void)l;(void)f;(void)v; ++errors; return 0; }
static LRESULT CALLBACK native(HWND w, UINT m, WPARAM p, LPARAM l)
{ return DefWindowProcA(w,m,p,l); }

int __cdecl main(void)
{
    wow_class_lookup_context classes = {0};
    wow_task_order_thread thread = {0};
    wow_task_order_entry task = {0};
    wow_class_client_context context = {&classes,0x030a,version,unexpected,&thread,NULL};
    wow_user_runtime runtime = WOW_USER_RUNTIME_INITIALIZER;
    wow_user_runtime_thread runtime_thread = {0};
    WNDCLASSA original = {0}, output, unchanged, native_view;
    WNDCLASSA external = {0};
    wow_class_words_binding *binding;
    const DWORD tagged = 0x92340056;
    DWORD words[2] = {0x12340056,0x789a}, old;
    BOOL previous_guest;
    HINSTANCE module = (HINSTANCE)0x23451234;
    ATOM atom;
    classes.client_module = GetModuleHandleW(L"user32.dll");
    task.hTaskWow = 0x4567;
    thread.ptdb = &task;
    thread.TIF_flags = WOW_TASK_TIF_16BIT;
    CHECK(wow_user_runtime_bind(&runtime_thread, &runtime, &thread, &context));
    original.hInstance = module;
    original.lpfnWndProc = (WNDPROC)tagged;
    original.lpszClassName = "S2_QUERY";
    original.lpszMenuName = "S2_CLIENT_MENU";
    original.cbClsExtra = 12;
    original.cbWndExtra = 18;
    atom = wow_class_client_register_guest(&context, &original, words);
    CHECK(atom != 0);
    CHECK(wow_class_fixture_publications == 1);
    CHECK(wow_class_fixture_words[0] == words[0] && wow_class_fixture_words[1] == words[1]);
    CHECK(wow_class_fixture_task == task.hTaskWow);
    CHECK(GetClassInfoA(module, original.lpszClassName, &native_view));
    CHECK(native_view.lpfnWndProc == wow_window_class_creation_proc);
    CHECK(native_view.lpszMenuName == NULL);
    CHECK(wow_user_get_class_infoA(module, original.lpszClassName, &output));
    ++queries;
    CHECK(output.lpfnWndProc == original.lpfnWndProc);
    CHECK(output.lpszClassName == original.lpszClassName);
    CHECK(output.lpszMenuName && !strcmp(output.lpszMenuName, original.lpszMenuName));
    CHECK(output.cbClsExtra == original.cbClsExtra && output.cbWndExtra == original.cbWndExtra);
    CHECK(wow_user_get_class_infoA((HINSTANCE)0x23455678, MAKEINTATOM(atom), &output));
    ++queries;
    CHECK(output.lpfnWndProc == (WNDPROC)tagged && output.lpszClassName == MAKEINTATOM(atom));
    binding = wow_class_words_acquire(&classes, atom, module);
    CHECK(binding != NULL);
    if (binding) {
        CHECK(wow_class_words_exchange_procedure(binding, (DWORD)native, FALSE,
            &old, &previous_guest));
        CHECK(old == tagged && previous_guest);
        CHECK(wow_user_get_class_infoA(module, "s2_query", &output));
        ++queries;
        CHECK(output.lpfnWndProc == native);
        CHECK(wow_class_words_exchange_procedure(binding, tagged, TRUE,
            &old, &previous_guest));
        CHECK(old == (DWORD)native && !previous_guest);
        wow_class_words_release(binding);
    }
    memset(&unchanged, 0x5a, sizeof(unchanged));
    output = unchanged;
    CHECK(!wow_user_get_class_infoA(module, "S2_QUERY_MISSING", &output));
    CHECK(!memcmp(&output, &unchanged, sizeof(output)));
    CHECK(!wow_user_get_class_infoA(NULL, original.lpszClassName, &output));
    CHECK(!memcmp(&output, &unchanged, sizeof(output)));
    binding = wow_class_words_acquire(&classes, atom, module);
    CHECK(binding != NULL);
    if (binding) {
        wow_class_words_entry(binding)->flags |= CSF_WOWDEFERDESTROY;
        CHECK(GetClassInfoA(module, original.lpszClassName, &native_view));
        CHECK(native_view.lpfnWndProc == wow_window_class_creation_proc);
        CHECK(!wow_user_get_class_infoA(module, original.lpszClassName, &output));
        CHECK(GetLastError() == ERROR_CLASS_DOES_NOT_EXIST);
        CHECK(!memcmp(&output, &unchanged, sizeof(output)));
        wow_class_words_entry(binding)->flags &= ~CSF_WOWDEFERDESTROY;
        wow_class_words_release(binding);
    }
    CHECK(wow_user_get_class_infoA(NULL, "BUTTON", &output));
    ++queries;
    CHECK(GetClassInfoA(NULL, "BUTTON", &native_view));
    CHECK(output.lpfnWndProc == native_view.lpfnWndProc && output.style == native_view.style);
    CHECK(wow_class_client_unregister(&context, original.lpszClassName, module));
    CHECK(classes.pclsPrivateList == NULL);
    /* Projection failure occurs after the original field copy, but must
     * withdraw the native registration before the original client frees its
     * menu names. A subsequent registration must not find a stale class. */
    wow_class_fixture_publication_fail = TRUE;
    CHECK(!wow_class_client_register_guest(&context, &original, words));
    CHECK(classes.pclsPrivateList == NULL);
    CHECK(!GetClassInfoA(module, original.lpszClassName, &output));
    CHECK(wow_class_fixture_publications == 2);
    wow_class_fixture_publication_fail = FALSE;
    /* Original _RegisterClassEx, not mere existence of ptdb, decides whether
     * the class belongs to a WOW task. Do not retain the old duplicate rule. */
    thread.TIF_flags = 0;
    CHECK(wow_class_client_register_guest(&context, &original, words));
    CHECK(wow_class_fixture_task == 0 && wow_class_fixture_publications == 3);
    CHECK(wow_class_client_unregister(&context, original.lpszClassName, module));
    CHECK(classes.pclsPrivateList == NULL);
    external.hInstance = GetModuleHandleA(NULL);
    external.lpfnWndProc = native;
    external.lpszClassName = "S2_NATIVE_QUERY";
    CHECK(RegisterClassA(&external));
    CHECK(wow_user_get_class_infoA(external.hInstance, external.lpszClassName, &output));
    ++queries;
    CHECK(output.lpfnWndProc == native);
    CHECK(UnregisterClassA(external.lpszClassName, external.hInstance));
    /* No task retirement callback is installed in this query-only fixture. */
    CHECK(wow_user_runtime_unbind(&runtime_thread));
    CHECK(wow_user_get_class_infoA(NULL, "BUTTON", &output));
    ++queries;
    CHECK(output.lpfnWndProc == native_view.lpfnWndProc);
    fprintf(stderr, "WOW_CLASS_QUERY errors=%u queries=%u logical=1 native=1\n", errors, queries);
    fprintf(stderr, "WOW_CLASS_COMMIT errors=%u publications=%u rollback=1 original_task=1\n", errors, wow_class_fixture_publications);
    return errors ? 1 : 0;
}
