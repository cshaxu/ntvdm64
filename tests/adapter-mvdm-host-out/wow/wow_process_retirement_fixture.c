#include "wow_user_task_lifecycle.h"
#include "wow_user_session_binding.h"
#include "wow_standard_class_bindings.h"
#include <stdio.h>
#include <string.h>

static unsigned errors;
#define CHECK(x) do { if (!(x)) { ++errors; fprintf(stderr, "FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)
static void WINAPI end_task(void) {}

static void verify_native_relationships(void)
{
    HWND parent = CreateWindowExA(0, "STATIC", "parent", WS_POPUP,
        0, 0, 40, 40, NULL, NULL, NULL, NULL);
    HWND popup = NULL, first = NULL, second = NULL;
    wow_user_window_links links, unchanged;
    CHECK(parent != NULL);
    if (!parent) return;
    popup = CreateWindowExA(0, "STATIC", "owned", WS_POPUP,
        0, 0, 20, 20, parent, NULL, NULL, NULL);
    first = CreateWindowExA(0, "STATIC", "first", WS_CHILD,
        0, 0, 10, 10, parent, NULL, NULL, NULL);
    second = CreateWindowExA(0, "STATIC", "second", WS_CHILD,
        0, 0, 10, 10, parent, NULL, NULL, NULL);
    CHECK(popup && first && second);
    if (popup && first && second) {
        wow_user_window_links *graph = NULL;
        ULONG count = 0, i, found = 0;
        HWND roots[2] = {first, first};
        CHECK(wow_user_window_sample_links(popup, &links));
        CHECK(links.parent == GetDesktopWindow() && links.owner == parent);
        CHECK(wow_user_window_sample_links(first, &links));
        CHECK(links.parent == parent && !links.owner);
        CHECK(links.next == GetWindow(first, GW_HWNDNEXT));
        CHECK(wow_user_window_sample_links(parent, &links));
        CHECK(links.child == GetWindow(parent, GW_CHILD));
        CHECK(SetWindowPos(first, HWND_TOP, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE));
        CHECK(wow_user_window_sample_links(parent, &links));
        CHECK(links.child == first);
        CHECK(wow_user_window_sample_links(first, &links));
        CHECK(links.next == second);
        CHECK(SetParent(second, popup) == parent);
        CHECK(wow_user_window_sample_links(second, &links));
        CHECK(links.parent == popup && !links.owner);
        CHECK(wow_user_window_sample_links(GetDesktopWindow(), &links));
        CHECK(!links.parent && !links.owner && !links.next);
        CHECK(wow_user_window_sample_graph(roots, 2, &graph, &count));
        for (i = 0; graph && i < count; ++i) {
            if (graph[i].window == first) {
                CHECK(graph[i].parent == parent && !graph[i].next);
                ++found;
            }
            if (graph[i].window == second) {
                CHECK(graph[i].parent == popup);
                ++found;
            }
            if (graph[i].window == popup) {
                CHECK(graph[i].owner == parent);
                ++found;
            }
            if (graph[i].window == parent || graph[i].window == GetDesktopWindow())
                ++found;
        }
        CHECK(found == 5); /* Duplicate roots must not duplicate nodes. */
        if (graph) HeapFree(GetProcessHeap(), 0, graph);
        graph = NULL;
        count = 123;
        roots[1] = NULL;
        CHECK(!wow_user_window_sample_graph(roots, 2, &graph, &count));
        CHECK(!graph && count == 123); /* Failed closure leaves outputs intact. */
        FillMemory(&links, sizeof(links), 0x5a);
        unchanged = links;
        CHECK(!wow_user_window_sample_links(NULL, &links));
        CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
        CHECK(memcmp(&links, &unchanged, sizeof(links)) == 0);
    }
    if (second) CHECK(DestroyWindow(second));
    if (first) CHECK(DestroyWindow(first));
    if (popup) CHECK(DestroyWindow(popup));
    CHECK(DestroyWindow(parent));
}

static void verify_isolated_relationships(void)
{
    HDESK previous = GetThreadDesktop(GetCurrentThreadId()), desktop;
    WCHAR name[80];
    wsprintfW(name, L"T447Graph-%lu-%lu", GetCurrentProcessId(), GetCurrentThreadId());
    desktop = CreateDesktopW(name, NULL, NULL, 0, GENERIC_ALL, NULL);
    CHECK(desktop != NULL);
    if (!desktop) return;
    if (SetThreadDesktop(desktop)) {
        verify_native_relationships();
        CHECK(SetThreadDesktop(previous));
    } else {
        CHECK(FALSE);
    }
    CHECK(CloseDesktop(desktop));
}

/* Original walias.c's private, x86 table ABI. Change only this fixture's
 * process-local descriptor while initialization is unpublished. This makes
 * native lookup fail after BUTTON was imported, without a production fault
 * switch or any modification of guest memory/native registrations. */
typedef struct {
    LPSTR name;
    ATOM atom;
    WNDPROC procedure;
    INT ordinal;
    DWORD guest_procedure;
} fixture_standard_class;
extern fixture_standard_class stdClasses[];
C_ASSERT(sizeof(fixture_standard_class) == 20);

static void verify_startup_rollback(wow_user_task_lifecycle *lifecycle,
    wow_user_runtime *runtime,
    const wow_user_task_lifecycle_callbacks *callbacks)
{
    LPSTR saved_name = stdClasses[3].name; /* WOWCLASS_COMBOBOX */
    DWORD error = ERROR_SUCCESS;
    BOOL result = TRUE;
    wow_cleanup_shared shared, previous;
    wow_cleanup_context cleanup = {0};
    FillMemory(&shared, sizeof(shared), 0x5a);
    previous = shared;
    cleanup.shared = &shared;
    cleanup.scroll_proc = (PROC)DefWindowProcA;
    __try {
        stdClasses[3].name = "T447_Missing_Standard_Class";
        result = wow_user_task_lifecycle_initialize(lifecycle, runtime, callbacks);
        error = GetLastError();
        CHECK(!wow_standard_classes_bind_cleanup(&cleanup));
        CHECK(memcmp(&shared, &previous, sizeof(shared)) == 0);
        CHECK(cleanup.scroll_proc == (PROC)DefWindowProcA);
    } __finally {
        stdClasses[3].name = saved_name;
    }
    CHECK(!result && error != ERROR_SUCCESS);
    CHECK(!lifecycle->initialized && !lifecycle->objects);
    CHECK(!lifecycle->process.objects);
    CHECK(!lifecycle->classes.pclsPublicList);
    CHECK(!lifecycle->classes.pclsPrivateList && !lifecycle->classes.system_classes);
    CHECK(!runtime->lifecycle && !runtime->retire_thread && !runtime->dispose_process);
}

int __cdecl main(void)
{
    session owner;
    wow_user_runtime runtime = WOW_USER_RUNTIME_INITIALIZER;
    wow_user_session_binding registration = {0};
    wow_user_task_lifecycle lifecycle = {0};
    wow_user_task_lifecycle_callbacks callbacks = {0};
    wow_class_lookup_entry unfinished_class = {0};
    wow_class_lookup_entry *public_classes;
    wow_class_lookup_entry *entry;
    unsigned standard_count = 0;
    HANDLE event = CreateEventW(NULL, FALSE, FALSE, NULL), duplicate;
    DWORD flags;
    WNDCLASSEXA control = {sizeof(control)};
    verify_isolated_relationships();
    CHECK(event != NULL);
    session_initialize(&owner, 44702u);
    CHECK(session_activate(&owner));
    CHECK(session_thread_bind_owned(&owner, SESSION_THREAD_BINDING_SOFTPC_ENTRY));
    CHECK(wow_user_session_attach(&registration, &owner, &runtime));
    verify_startup_rollback(&lifecycle, &runtime, &callbacks);
    /* The same owner/runtime must remain usable after partial startup fails. */
    CHECK(wow_user_task_lifecycle_initialize(&lifecycle, &runtime, &callbacks));
    CHECK(lifecycle.classes.pclsPublicList != NULL);
    /* Exercise the original message-handle policy separately from strict
     * ownership lookup. Sentinel rows carry no native object or allocation. */
    CHECK(wow_user_full_handle(lifecycle.objects, 0) == 0);
    CHECK(wow_user_full_handle(lifecycle.objects, 0xffff) == 0xffff);
    CHECK(wow_user_full_handle(lifecycle.objects, 2) == 2);
    lifecycle.objects->entries[2].bType = TYPE_WINDOW;
    lifecycle.objects->entries[2].wUniq = 0x4567;
    CHECK(wow_user_full_handle(lifecycle.objects, 2) == 0x45670002u);
    CHECK(!wow_user_window_full_handle(lifecycle.objects, 2));
    lifecycle.objects->entries[2].bType = TYPE_MENU;
    CHECK(wow_user_full_handle(lifecycle.objects, 2) == 2);
    ZeroMemory(&lifecycle.objects->entries[2], sizeof(lifecycle.objects->entries[2]));
    CHECK(wow_user_full_handle(lifecycle.objects, 2) == 2);
    public_classes = lifecycle.classes.pclsPublicList;
    for (entry = public_classes; entry; entry = entry->next) {
        CHECK(entry->fnid && !entry->hTaskWow);
        CHECK(wow_class_words_native_registration((wow_class_words_binding *)entry));
        ++standard_count;
    }
    CHECK(standard_count == 9);
    CHECK(!wow_standard_classes_prepare(&lifecycle.classes, NULL));
    CHECK(GetLastError() == ERROR_INVALID_STATE);
    CHECK(runtime.dispose_process != NULL);
    CHECK(wow_user_task_lifecycle_register(&lifecycle, end_task, event));
    duplicate = lifecycle.process.pwpi->pEventWowExec;
    CHECK(duplicate != event && GetHandleInformation(duplicate, &flags));
    CHECK(wow_user_task_lifecycle_init(&lifecycle, 0x0400, "RETIRE.EXE",
        0x1234, 0, 0, 0, 0, 1, 1, SW_HIDE));
    CHECK(!wow_user_task_lifecycle_dispose(&lifecycle));
    CHECK(GetLastError() == ERROR_BUSY && lifecycle.threads != NULL);

    /* A live object must not disappear with the table. No native object is
     * invented here: this sentinel tests the last-owner refusal separately
     * from original task cleanup, already exercised by retirement below. */
    CHECK(wow_user_runtime_unbind(wow_user_runtime_current()));
    CHECK(lifecycle.threads == NULL);
    lifecycle.objects->last_handle = 2;
    lifecycle.objects->entries[2].bType = TYPE_MENU;
    CHECK(!wow_user_session_detach(&registration));
    CHECK(GetLastError() == ERROR_BUSY && registration.registered);
    CHECK(lifecycle.classes.pclsPublicList == public_classes);
    CHECK(lifecycle.objects && GetHandleInformation(duplicate, &flags));
    ZeroMemory(&lifecycle.objects->entries[2], sizeof(lifecycle.objects->entries[2]));
    lifecycle.classes.pclsPrivateList = &unfinished_class;
    CHECK(!wow_user_session_detach(&registration));
    CHECK(GetLastError() == ERROR_BUSY && lifecycle.initialized);
    CHECK(lifecycle.classes.pclsPrivateList == &unfinished_class);
    CHECK(lifecycle.classes.pclsPublicList == public_classes);
    lifecycle.classes.pclsPrivateList = NULL;
    ++public_classes->cWndReferenceCount;
    CHECK(!wow_user_session_detach(&registration));
    CHECK(GetLastError() == ERROR_BUSY && registration.registered);
    CHECK(lifecycle.classes.pclsPublicList == public_classes);
    --public_classes->cWndReferenceCount;

    /* Startup now owns the borrowed control metadata. Last-owner disposal
     * must release it without unregistering USER's native controls. */
    CHECK(wow_user_session_detach(&registration));
    CHECK(!registration.registered && !lifecycle.initialized && !lifecycle.objects);
    CHECK(!runtime.lifecycle && !runtime.retire_thread && !runtime.dispose_process);
    CHECK(!GetHandleInformation(duplicate, &flags));
    CHECK(GetLastError() == ERROR_INVALID_HANDLE);
    CHECK(GetHandleInformation(event, &flags));
    CHECK(GetClassInfoExA(NULL, "BUTTON", &control));
    CHECK(!owner.thread_hook_count && !wow_user_runtime_current());
    CHECK(session_thread_unbind(&owner));
    CHECK(session_dispose(&owner));
    CHECK(CloseHandle(event));
    fprintf(stderr, "WOW_PROCESS_RETIREMENT errors=%u startup-rollback/cleanup-atomic/task/object/class/refusal/retry/event/native checked\n", errors);
    return errors ? 1 : 0;
}
