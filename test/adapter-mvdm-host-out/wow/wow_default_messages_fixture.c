#include "compat/include/opennt_user_multipass.h"
#include "wow_default_message_bindings.h"
#include <stdio.h>
#include <string.h>

#undef HeapAlloc
#undef HeapFree
#undef RegisterWindowMessageW

static unsigned allocation_count, fail_allocation, live_allocations;
static BOOL fail_registration;

LPVOID WINAPI fixture_message_alloc(HANDLE heap, DWORD flags, SIZE_T bytes)
{
    LPVOID memory;
    if (!(flags & HEAP_ZERO_MEMORY)) RaiseException(STATUS_INVALID_PARAMETER, 0, 0, NULL);
    if (++allocation_count == fail_allocation)
        RaiseException(STATUS_NO_MEMORY, 0, 0, NULL);
    memory = HeapAlloc(heap, flags, bytes);
    if (memory) ++live_allocations;
    return memory;
}

BOOL WINAPI fixture_message_free(HANDLE heap, DWORD flags, LPVOID memory)
{
    BOOL freed = HeapFree(heap, flags, memory);
    if (freed) --live_allocations;
    return freed;
}

UINT WINAPI fixture_message_register(LPCWSTR name)
{
    if (fail_registration) {
        SetLastError(ERROR_ACCESS_DENIED);
        return 0;
    }
    return RegisterWindowMessageW(name);
}

/* Link the production initializer owner and original server.c slice.
 * Message membership requirements below are independent of its input arrays.
 * This is not a window-procedure, native transport or guest test. */
static int failures;
#define CHECK(x) do { if (!(x)) { ++failures; printf("FAIL %d: %s\n", __LINE__, #x); } } while (0)

static BOOL member(const WNDMSG *table, UINT message)
{
    return message <= table->maxMsgs &&
        (table->abMsgs[message / 8] & (1u << (message & 7))) != 0;
}

static WORD WINAPI unused_merge(PBYTE bits, WORD count)
{ (void)bits; (void)count; return 0; }

int main(void)
{
    WNDMSG server, client, edit, list, combo, combo_list, button;
    SHAREDINFO empty = {0};
    unsigned failure;
    static const wow_default_message_provider provider = { InitMessageTables, unused_merge, wow_default_message_reset };
    CHECK(wow_default_messages_bind(&provider));
    for (failure = 1; failure <= 13; ++failure) {
        allocation_count = 0;
        fail_allocation = failure;
        CHECK(!wow_default_messages_ready());
        CHECK(GetLastError() == ERROR_NOT_ENOUGH_MEMORY);
        CHECK(allocation_count == failure && live_allocations == 0);
        CHECK(memcmp(&empty, &wow_default_message_state, sizeof(empty)) == 0);
    }
    fail_allocation = 0;
    allocation_count = 0;
    fail_registration = TRUE;
    CHECK(!wow_default_messages_ready());
    CHECK(GetLastError() == ERROR_ACCESS_DENIED);
    CHECK(allocation_count == 13 && live_allocations == 0);
    CHECK(memcmp(&empty, &wow_default_message_state, sizeof(empty)) == 0);
    fail_registration = FALSE;
    allocation_count = 0;
    SetLastError(456);
    CHECK(wow_default_messages_ready());
    CHECK(GetLastError() == 456);
    CHECK(allocation_count == 13 && live_allocations == 13);
    CHECK(wow_default_shell_message >= 0xc000 &&
        wow_default_shell_message <= 0xffff);
    CHECK(wow_default_shell_message == RegisterWindowMessageW(L"SHELLHOOK"));
    CHECK(member(&wow_default_message_state.awmControl[FNID_DIALOG - FNID_START], WM_INITDIALOG));
    CHECK(member(&wow_default_message_state.awmControl[FNID_SCROLLBAR - FNID_START], WM_LBUTTONDOWN));
    CHECK(member(&wow_default_message_state.awmControl[FNID_MENU - FNID_START], WM_PAINT));
    CHECK(member(&wow_default_message_state.awmControl[FNID_DESKTOP - FNID_START], WM_ERASEBKGND));
    CHECK(member(&wow_default_message_state.awmControl[FNID_STATIC - FNID_START], WM_PAINT));
    server = wow_default_message_state.DefWindowMsgs;
    client = wow_default_message_state.DefWindowSpecMsgs;
    edit = wow_default_message_state.awmControl[FNID_EDIT - FNID_START];
    button = wow_default_message_state.awmControl[FNID_BUTTON - FNID_START];
    CHECK(button.abMsgs != NULL);
    if (!button.abMsgs) return 1;
    CHECK(member(&button, BM_CLICK) && member(&button, BM_SETCHECK));
    CHECK(member(&button, BM_SETIMAGE) && member(&button, WM_CAPTURECHANGED));
    CHECK(member(&button, WM_NCCREATE) && member(&button, WM_NCDESTROY));
    CHECK(member(&button, WM_FINALDESTROY));
    CHECK(!member(&button, LB_ADDSTRING) && !member(&button, EM_REPLACESEL));
    CHECK(!member(&button, 0) && !member(&button, WM_USER));
    list = wow_default_message_state.awmControl[FNID_LISTBOX - FNID_START];
    combo = wow_default_message_state.awmControl[FNID_COMBOBOX - FNID_START];
    combo_list = wow_default_message_state.awmControl[FNID_COMBOLISTBOX - FNID_START];
    CHECK(list.abMsgs && combo.abMsgs && combo_list.abMsgs);
    if (!list.abMsgs || !combo.abMsgs || !combo_list.abMsgs) return 1;
    CHECK(member(&list, LB_ADDSTRING));
    CHECK(member(&list, LBCB_STARTTRACK));
    CHECK(member(&list, LBCB_ENDTRACK));
    CHECK(member(&list, WM_TIMER));
    CHECK(member(&list, WM_FINALDESTROY));
    CHECK(member(&combo_list, LBCB_STARTTRACK));
    CHECK(member(&combo_list, LB_GETTEXT));
    CHECK(combo_list.maxMsgs == list.maxMsgs);
    CHECK(member(&combo, CB_SHOWDROPDOWN));
    CHECK(member(&combo, CBEC_KILLCOMBOFOCUS));
    CHECK(member(&combo, WM_FINALDESTROY));
    CHECK(!member(&combo, LBCB_STARTTRACK));
    CHECK(!member(&list, CB_SHOWDROPDOWN));
    CHECK(!member(&list, 0) && !member(&combo, 0));
    CHECK(CBEC_SETCOMBOFOCUS == 0x0166);
    CHECK(CBEC_KILLCOMBOFOCUS == 0x0167);
    CHECK(server.abMsgs && client.abMsgs && edit.abMsgs);
    if (!server.abMsgs || !client.abMsgs || !edit.abMsgs) return 1;
    CHECK(member(&edit, EM_REPLACESEL));
    CHECK(member(&edit, EM_GETLINE));
    CHECK(member(&edit, WM_NCCREATE));
    CHECK(member(&edit, WM_NCDESTROY));
    CHECK(member(&edit, WM_FINALDESTROY));
    CHECK(member(&edit, WM_SYSTIMER));
    CHECK(member(&edit, WM_IME_COMPOSITION) && member(&edit, EM_GETIMESTATUS));
    CHECK(member(&edit, WM_STYLECHANGED));
    CHECK(member(&list, LB_GETLISTBOXINFO) && member(&list, WM_MBUTTONDOWN));
    CHECK(member(&wow_default_message_state.awmControl[FNID_IME - FNID_START], WM_IME_REQUEST));
    CHECK(!member(&wow_default_message_state.awmControl[FNID_IME - FNID_START], WM_USER));
    CHECK(member(&wow_default_message_state.awmControl[FNID_DIALOG - FNID_START], WM_SYSKEYDOWN));
    CHECK(member(&edit, WM_MOUSEWHEEL));
    CHECK(!member(&edit, WM_NCPAINT)); /* Original #if 0 exclusion. */
    CHECK(!member(&edit, WM_USER));
    CHECK(!member(&edit, (UINT)-1));
    CHECK(!member(&edit, 0));
    CHECK(member(&server, WM_SETTEXT));
    CHECK(!member(&server, WM_GETTEXT));
    CHECK(!member(&server, WM_GETTEXTLENGTH));
    CHECK(member(&client, WM_GETTEXT));
    CHECK(member(&client, WM_GETTEXTLENGTH));
    CHECK(member(&client, WM_CTLCOLOREDIT));
    CHECK(!member(&server, WM_CTLCOLOREDIT));
    CHECK(member(&server, WM_CONTEXTMENU));
    CHECK(!member(&client, WM_CONTEXTMENU));
    CHECK(!member(&server, WM_APP) && !member(&client, WM_APP));
    CHECK(!member(&server, (UINT)-1) && !member(&client, (UINT)-1));
    CHECK(!member(&server, 0) && !member(&client, 0));
    SetLastError(789);
    CHECK(wow_default_messages_ready());
    CHECK(GetLastError() == 789);
    CHECK(allocation_count == 13 && live_allocations == 13);
    CHECK(server.abMsgs == wow_default_message_state.DefWindowMsgs.abMsgs);
    CHECK(client.abMsgs == wow_default_message_state.DefWindowSpecMsgs.abMsgs);
    CHECK(server.maxMsgs == wow_default_message_state.DefWindowMsgs.maxMsgs);
    CHECK(client.maxMsgs == wow_default_message_state.DefWindowSpecMsgs.maxMsgs);
    CHECK(edit.abMsgs == wow_default_message_state.awmControl[FNID_EDIT - FNID_START].abMsgs);
    CHECK(edit.maxMsgs == wow_default_message_state.awmControl[FNID_EDIT - FNID_START].maxMsgs);
    CHECK(list.abMsgs == wow_default_message_state.awmControl[FNID_LISTBOX - FNID_START].abMsgs);
    CHECK(combo.abMsgs == wow_default_message_state.awmControl[FNID_COMBOBOX - FNID_START].abMsgs);
    CHECK(combo_list.abMsgs == wow_default_message_state.awmControl[FNID_COMBOLISTBOX - FNID_START].abMsgs);
    CHECK(button.abMsgs == wow_default_message_state.awmControl[FNID_BUTTON - FNID_START].abMsgs);
    printf("default_messages_failures=%d\n", failures);
    return failures != 0;
}
