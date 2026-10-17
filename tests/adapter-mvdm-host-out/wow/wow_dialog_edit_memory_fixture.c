#include "wow_edit_memory_bindings.h"
#include <stdio.h>
#undef pfn16GlobalAlloc
#undef pfn16GlobalFree

/* Supplemental original-body fixture. It deliberately substitutes callback
 * inputs and cannot prove guest segment allocation or dialog lifetime.
 * Compile original client.c with WOW_ORIGINAL_DIALOG_EDIT and /Gz; do not
 * link the production registration bridge into this isolated fixture. */
static PFNWOWHANDLERSIN callbacks;
static UINT seen_flags, allocate_calls, free_calls;
static DWORD seen_bytes;
static WORD result_handle, freed_handle;
static HWND initialized_window;
static LONG initialized_parameter;
static DWORD initialization_result;
static UINT initialization_calls;

const struct tagAPFNWOWHANDLERSIN *WINAPI wow_user_registered_callbacks(void)
{
    return &callbacks;
}

static WORD WINAPI allocate_segment(UINT flags, DWORD bytes)
{
    ++allocate_calls;
    seen_flags = flags;
    seen_bytes = bytes;
    return result_handle;
}

static VOID WINAPI free_segment(WORD handle)
{
    ++free_calls;
    freed_handle = handle;
}

static DWORD WINAPI initialize_dialog(HWND window, LONG parameter)
{
    ++initialization_calls;
    initialized_window = window;
    initialized_parameter = parameter;
    return initialization_result;
}

int main(void)
{
    int failed = 0;
#define CHECK(label, expression) do { \
    int ok = !!(expression); \
    printf("%s=%d\n", label, ok); \
    if (!ok) ++failed; \
} while (0)
    /* Avoid the original variable-name macros when filling fixture inputs. */
    callbacks.pfn16GlobalAlloc = allocate_segment;
    callbacks.pfn16GlobalFree = free_segment;
    callbacks.pfnInitDlgCb = initialize_dialog;
    initialization_result = 0xfedcba98;
    CHECK("registered_dialog_result", WOWDlgInit((HWND)(ULONG_PTR)0x1234,
        (LONG)0x87654321) == initialization_result);
    CHECK("registered_dialog_arguments", initialization_calls == 1 &&
        initialized_window == (HWND)(ULONG_PTR)0x1234 &&
        initialized_parameter == (LONG)0x87654321);
    initialization_result = 0;
    CHECK("zero_dialog_parameter_and_result", WOWDlgInit(NULL, 0) == 0 &&
        initialization_calls == 2 && initialized_window == NULL &&
        initialized_parameter == 0);
    CHECK("initialization_has_no_allocation_policy", !allocate_calls && !free_calls);
    result_handle = 0x3457;
    CHECK("original_segment_result", (ULONG_PTR)GetEditDS() == 0x3457);
    CHECK("original_allocation_contract", allocate_calls == 1 &&
        seen_flags == (GHND | GMEM_SHARE) && seen_bytes == 256);
    ReleaseEditDS((HANDLE)(ULONG_PTR)0xabcd3457);
    CHECK("original_loword_release", free_calls == 1 && freed_handle == 0x3457);
    result_handle = 0;
    CHECK("allocation_failure_preserved", GetEditDS() == NULL && allocate_calls == 2);
    CHECK("no_implicit_free", free_calls == 1);
    if (!failed) puts("WOW_DIALOG_EDIT_ORIGINAL_MEMORY_OK");
    return failed ? 1 : 0;
}
