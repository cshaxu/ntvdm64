#include <windows.h>
#include <stdio.h>
#include <stddef.h>
#include "wow_window_words_binding.h"
/* Original winuserp.h prerequisite absent from the public Windows SDK. */
typedef VOID (APIENTRY *PFNW32ET)(VOID);
#pragma warning(push)
#pragma warning(disable:4201) /* Original ABI deliberately uses anonymous unions. */
#include "opennt-abi/source/public/internal/windows/inc/wowuserp.h"
#pragma warning(pop)
typedef char ww_size_is_32[(sizeof(WW) == 32) ? 1 : -1];
typedef char ww_prefix_is_12[(sizeof(((WW *)0)->adwWOW) == 12) ? 1 : -1];
typedef char ww_style_offset_is_24[(offsetof(WW, dwStyle) == 24) ? 1 : -1];

/* Public-boundary feasibility only: no production WOW table or guest state.
 * Each invisible owned window gets copied metadata before its first message,
 * without consuming GWLP_USERDATA or replacing CREATESTRUCT.lpCreateParams. */
typedef struct FRAME {
    struct FRAME *parent;
    struct FRAME *owner, *record;
    HWND window;
    DWORD identity;
    BOOL reject, nested, veto, fail_alloc, fail_property;
    BOOL claimed;
    unsigned messages;
    WW words;
    const DWORD *creation_words;
    HANDLE wow_instance;
} FRAME;
static FRAME *pending;
static HHOOK hook, veto_hook;
static HINSTANCE instance;
static unsigned live, created, destroyed, failures, minmax;
static unsigned rolled_back, vetoed, allocations, frees;
static unsigned unbound_destroy;
static const char class_name[] = "S40WowCreateOrderFixture";
static const char property[] = "S40.WowCreateOrder";
static DWORD *guarded_words;
static unsigned bound_callbacks;
static wow_class_lookup_context class_context;
static ATOM class_atom;
static DWORD shared_class_words[2] = {0x12340056, 0x87650000};
static LPVOID application_parameter = (LPVOID)0x12345678;

static HWND create_owned(FRAME *frame)
{
    HWND window;
    frame->parent = pending;
    frame->creation_words = guarded_words;
    frame->wow_instance = instance;
    frame->claimed = FALSE;
    pending = frame;
    window = CreateWindowExA(0, class_name, "", WS_OVERLAPPEDWINDOW,
        0, 0, 64, 64, NULL, NULL, instance, application_parameter);
    /* All fixture frames outlive their windows. A failed create that did not
     * deliver destruction leaves its record owned by this attempt. */
    if (!window && frame->record) {
        wow_window_words_detach(frame->window);
        if (IsWindow(frame->window)) RemovePropA(frame->window, property);
        HeapFree(GetProcessHeap(), 0, frame->record);
        frame->record = NULL;
        --live;
        ++frees;
        ++rolled_back;
    }
    pending = frame->parent;
    return window;
}

static LRESULT CALLBACK creation_hook(int code, WPARAM wparam, LPARAM lparam)
{
    BOOL attached = FALSE;
    LRESULT result;
    if (code == HCBT_CREATEWND && pending) {
        CBT_CREATEWNDA *create = (CBT_CREATEWNDA *)lparam;
        if (!pending->claimed && create->lpcs->hInstance == instance &&
                create->lpcs->lpCreateParams == application_parameter &&
                (ATOM)GetClassLongA((HWND)wparam, GCW_ATOM) == class_atom) {
            wow_class_words_binding *class_borrow;
            pending->claimed = TRUE;
            FRAME *copy = pending->fail_alloc ? NULL :
                (FRAME *)HeapAlloc(GetProcessHeap(), 0, sizeof(*copy));
            if (!copy) return 1;
            ++allocations;
            *copy = *pending;
            /* Original createw.c copies only WND_CNT_WOWDWORDS from lpWOW.
             * The guard page after this input catches an accidental WW copy. */
            CopyMemory(copy->words.adwWOW, pending->creation_words,
                sizeof(copy->words.adwWOW));
            copy->words.dwExStyle = create->lpcs->dwExStyle;
            copy->words.dwStyle = create->lpcs->style;
            copy->words.hInstance = create->lpcs->hInstance;
            if (pending->fail_property || !SetPropA((HWND)wparam, property, copy)) {
                HeapFree(GetProcessHeap(), 0, copy);
                ++frees;
                return 1;
            }
            class_borrow = wow_class_words_acquire(&class_context, class_atom,
                create->lpcs->hInstance);
            if (!class_borrow || !wow_window_words_attach_class((HWND)wparam,
                    &copy->words, class_borrow)) {
                wow_class_words_release(class_borrow);
                RemovePropA((HWND)wparam, property);
                HeapFree(GetProcessHeap(), 0, copy);
                ++frees;
                return 1;
            }
            ++live;
            ++created;
            copy->owner = pending;
            pending->record = copy;
            pending->window = (HWND)wparam;
            attached = TRUE;
        }
    }
    result = CallNextHookEx(hook, code, wparam, lparam);
#ifdef WOW_CREATE_EARLY_VETO_ROLLBACK
    if (attached && result) {
        wow_window_words_detach((HWND)wparam);
        FRAME *copy = (FRAME *)RemovePropA((HWND)wparam, property);
        if (!copy) ++failures;
        else {
            copy->owner->record = NULL;
            HeapFree(GetProcessHeap(), 0, copy);
            ++frees;
            --live;
            ++rolled_back;
        }
    }
#else
    (void)attached;
#endif
    return result;
}

static LRESULT CALLBACK reject_hook(int code, WPARAM wp, LPARAM lp)
{
    if (code == HCBT_CREATEWND && pending && pending->veto) {
        CBT_CREATEWNDA *create = (CBT_CREATEWNDA *)lp;
        if (create->lpcs->lpCreateParams == application_parameter &&
                (ATOM)GetClassLongA((HWND)wp, GCW_ATOM) == class_atom) {
            ++vetoed;
            return 1;
        }
    }
    return CallNextHookEx(veto_hook, code, wp, lp);
}

static LRESULT CALLBACK procedure(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    FRAME *frame = (FRAME *)GetPropA(window, property);
    if (!frame) {
        if (message == WM_NCDESTROY) ++unbound_destroy;
        else {
            printf("missing_state_message=%04x\n", message);
            ++failures;
        }
        return DefWindowProcA(window, message, wp, lp);
    }
    ++frame->messages;
    if (memcmp(frame->words.adwWOW, guarded_words, sizeof(frame->words.adwWOW)) ||
        frame->words.hInstance != frame->wow_instance ||
        frame->words.hInstance != instance) ++failures;
    if (message == WM_GETMINMAXINFO) ++minmax;
    if (message == WM_NCCREATE) {
        CREATESTRUCTA *create = (CREATESTRUCTA *)lp;
        if (create->lpCreateParams != application_parameter) ++failures;
        /* GWLP_USERDATA still belongs to the window/application. */
        if (GetWindowLongPtrA(window, GWLP_USERDATA) != 0) ++failures;
        SetWindowLongPtrA(window, GWLP_USERDATA, frame->identity);
        if (frame->nested) {
            FRAME child = {0};
            HWND nested;
            child.identity = 0x4321;
            nested = create_owned(&child);
            if (!nested) ++failures;
            else DestroyWindow(nested);
            if (GetPropA(window, property) != frame ||
                (DWORD)GetWindowLongPtrA(window, GWLP_USERDATA) != frame->identity)
                ++failures;
        }
        if (frame->reject) return FALSE;
    }
    if (message == WM_NCDESTROY) {
        LRESULT result = DefWindowProcA(window, message, wp, lp);
        wow_window_words_detach(window);
        RemovePropA(window, property);
        frame->owner->record = NULL;
        HeapFree(GetProcessHeap(), 0, frame);
        ++frees;
        --live;
        ++destroyed;
        return result;
    }
    return DefWindowProcA(window, message, wp, lp);
}

static DWORD WINAPI wow_callback(HWND window, UINT message, UINT wp, LONG lp,
    DWORD proc, PVOID words)
{
    FRAME *frame = (FRAME *)GetPropA(window, property);
    wow_window_words_binding *borrow;
    DWORD *class_words;
    if (!frame || !words || proc != 0x81234567) {
        ++failures;
        return 0;
    }
    borrow = wow_window_words_acquire(window);
    class_words = wow_window_class_words_value(borrow);
    if (!class_words || class_words[0] != shared_class_words[0] ||
            class_words[1] != shared_class_words[1]) ++failures;
    wow_window_words_release(borrow);
    if (memcmp(words, &frame->words, offsetof(WW, dwExStyle))) ++failures;
    if (((WW *)words)->dwStyle != (DWORD)GetWindowLongA(window, GWL_STYLE) ||
        ((WW *)words)->dwExStyle != (DWORD)GetWindowLongA(window, GWL_EXSTYLE) ||
        ((WW *)words)->hInstance != (HANDLE)GetWindowLongA(window, GWL_HINSTANCE))
        ++failures;
    ++bound_callbacks;
    return procedure(window, message, wp, lp);
}

static LRESULT CALLBACK native_gateway(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    LONG result = 0;
    /* A hook rejection before attach may receive an unbound destruction. */
    if (!GetPropA(window, property)) return procedure(window, message, wp, lp);
    if (!wow_window_dispatch_bound(window, message, wp, lp, 0x81234567, wow_callback, &result))
        ++failures;
    return result;
}

int __cdecl main(void)
{
    WNDCLASSEXA cls = {sizeof(cls)};
    UNICODE_STRING captured_name;
    FRAME normal = {0}, rejected = {0}, denied = {0}, unavailable = {0};
    HWND window;
    BYTE *guard;
    DWORD old_protection;
    instance = GetModuleHandleA(NULL);
    guard = (BYTE *)VirtualAlloc(NULL, 8192, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!guard || !VirtualProtect(guard + 4096, 4096, PAGE_NOACCESS, &old_protection)) return 4;
    guarded_words = (DWORD *)(guard + 4096 - 12);
    guarded_words[0] = 0x00010001;
    guarded_words[1] = 0x12340056;
    guarded_words[2] = 0x43210078;
    cls.hInstance = instance;
    cls.lpfnWndProc = native_gateway;
    cls.lpszClassName = class_name;
    RtlInitUnicodeString(&captured_name, L"S40WowCreateOrderFixture");
    class_atom = wow_class_words_register(&class_context, &cls,
        &captured_name, shared_class_words, NULL);
    if (!class_atom) return 1;
    veto_hook = SetWindowsHookExA(WH_CBT, reject_hook, NULL, GetCurrentThreadId());
    if (!veto_hook) { wow_class_words_unregister(&class_context, class_atom, instance, NULL); return 2; }
    hook = SetWindowsHookExA(WH_CBT, creation_hook, NULL, GetCurrentThreadId());
    if (!hook) {
        UnhookWindowsHookEx(veto_hook);
        wow_class_words_unregister(&class_context, class_atom, instance, NULL);
        return 2;
    }
    normal.identity = 0x1234;
    normal.nested = TRUE;
    window = create_owned(&normal);
    if (!window) ++failures;
    else DestroyWindow(window);
    rejected.identity = 0x5678;
    rejected.reject = TRUE;
    window = create_owned(&rejected);
    if (window) { ++failures; DestroyWindow(window); }
    denied.identity = 0x7788;
    denied.veto = TRUE;
    window = create_owned(&denied);
    if (window) { ++failures; DestroyWindow(window); }
    /* Reverse hook order: rejection before the binding must allocate nothing. */
    if (!UnhookWindowsHookEx(veto_hook)) ++failures;
    veto_hook = SetWindowsHookExA(WH_CBT, reject_hook, NULL, GetCurrentThreadId());
    if (!veto_hook) ++failures;
    window = create_owned(&denied);
    if (window) { ++failures; DestroyWindow(window); }
    unavailable.fail_alloc = TRUE;
    window = create_owned(&unavailable);
    if (window) { ++failures; DestroyWindow(window); }
    unavailable.fail_alloc = FALSE;
    unavailable.fail_property = TRUE;
    window = create_owned(&unavailable);
    if (window) { ++failures; DestroyWindow(window); }
    if (!UnhookWindowsHookEx(hook)) ++failures;
    if (!UnhookWindowsHookEx(veto_hook)) ++failures;
    if (!wow_class_words_unregister(&class_context, class_atom, instance, NULL)) ++failures;
    if (class_context.pclsPrivateList || class_context.pclsPublicList) ++failures;
    printf("created=%u destroyed=%u live=%u minmax=%u failures=%u\n",
        created, destroyed, live, minmax, failures);
    printf("vetoed=%u rollback=%u allocations=%u frees=%u unbound_destroy=%u\n",
        vetoed, rolled_back, allocations, frees, unbound_destroy);
    if (failures || live || pending || created != 4 || destroyed != 4 || !minmax ||
        vetoed != 2 || rolled_back != 0 || allocations != frees || allocations != 5)
        return 3;
    puts("WOW_PUBLIC_PRECREATE_NESTED_AND_FAILURE_ORDER_OK");
    puts("WOW_PUBLIC_PRECREATE_VETO_AND_RESOURCE_FAILURE_OK");
    if (!VirtualFree(guard, 0, MEM_RELEASE)) return 5;
    puts("WOW_ORIGINAL_WW_PREFIX_GUARD_OK input=12 view=32");
    puts("WOW_PRECREATE_SHARED_CLASS_AND_NATIVE_INSTANCE_OK");
    puts("WOW_PRECREATE_OPAQUE_APPLICATION_PARAMETER_PRESERVED_OK");
    if (!bound_callbacks) return 6;
    printf("WOW_PRECREATE_BOUND_ORIGINAL_DISPATCH_OK callbacks=%u\n", bound_callbacks);
    return 0;
}
