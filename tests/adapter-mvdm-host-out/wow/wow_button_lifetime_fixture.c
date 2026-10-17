#include "wow_button_lifetime.h"
#include "wow_edit_entry_bindings.h"
#include "wow_user_client_view_layout.h"
#include "wow_window_client_state_bindings.h"
#define WOW_ORIGINAL_CONTROL_EXTENT_LAYOUT
#include "opennt-host/windows/core/ntuser/inc/user.h"
#undef WOW_ORIGINAL_CONTROL_EXTENT_LAYOUT
#include <stdio.h>

/* Lifetime adapter only: original message algorithm and allocator behavior
 * are separate group acceptance, not replaced by these boundary doubles. */
static BUTNWND first, second;
static BUTN first_data, second_data;
static LOOKASIDE pool;
static PWND current_state;
static unsigned failures, frees;
static BOOL exceptional;
__declspec(thread) wow_edit_context *wow_edit_current;
static HDC resource_dc;
static unsigned released_dcs, released_bitmaps;
static HDC last_released_dc;
#define BUTTON_EXCEPTION 0xe0477117u
#define CHECK(x) do { if (!(x)) { ++failures; printf("FAIL line=%d\n", __LINE__); } } while (0)
BOOL WINAPI fixture_button_delete_dc(HDC dc)
{
    BOOL result = DeleteDC(dc);
    CHECK(result);
    if (result) { ++released_dcs; last_released_dc = dc; }
    return result;
}
BOOL WINAPI fixture_button_delete_bitmap(HGDIOBJ bitmap)
{
    BOOL result = DeleteObject(bitmap);
    CHECK(result);
    if (result) ++released_bitmaps;
    return result;
}
PWND WINAPI wow_window_client_validate_hwnd(HWND window)
{
    CHECK(current_state && current_state->head.h == window);
    return current_state;
}
void WINAPI FreeLookasideEntry(PLOOKASIDE owner, PVOID value)
{
    CHECK(owner == &pool && (value == &first_data || value == &second_data));
    ++frees;
    ((PBUTN)value)->buttonState = 0xdead;
    SetLastError(0xbad);
}
__declspec(noreturn) VOID WINAPI wow_user_native_call_failed(const char *stage, DWORD error)
{
    (void)stage; (void)error;
    ++failures;
    RaiseException(BUTTON_EXCEPTION, EXCEPTION_NONCONTINUABLE, 0, NULL);
    ExitProcess(99);
}
static LRESULT CALLBACK destroy_state(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    (void)window; (void)message; (void)wp; (void)lp;
    PBUTNWND state = (PBUTNWND)current_state;
    /* Match original NC body: clear back-reference, request release, mark. */
    state->pbutn->spwnd = NULL;
    wow_button_free_state(&pool, state->pbutn);
    state->wnd.fnid |= FNID_CLEANEDUP_BIT;
    SetLastError(0x47117);
    return 37;
}
static LRESULT CALLBACK outer(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    unsigned before = frees;
    (void)message; (void)wp; (void)lp;
    CHECK(wow_button_invoke(window, WM_NCDESTROY, 0, 0, destroy_state) == 37);
    CHECK(frees == before && first_data.buttonState == 11);
    CHECK(first_data.spwnd == NULL && (first.wnd.fnid & FNID_CLEANEDUP_BIT));
    /* Same HWND, different retained generation: must not attach to outer. */
    current_state = &second.wnd;
    CHECK(wow_button_invoke(window, WM_NCDESTROY, 0, 0, destroy_state) == 37);
    CHECK(frees == before + 1 && second_data.buttonState == 0xdead);
    CHECK(first_data.buttonState == 11);
    current_state = &first.wnd;
    SetLastError(0x47117);
    if (exceptional) RaiseException(BUTTON_EXCEPTION, 0, 0, NULL);
    return 41;
}
static void setup(void)
{
    ZeroMemory(&first, sizeof(first)); ZeroMemory(&second, sizeof(second));
    first.wnd.head.h = second.wnd.head.h = (HWND)(ULONG_PTR)1;
    first.wnd.fnid = second.wnd.fnid = FNID_BUTTON;
    first.pbutn = &first_data; second.pbutn = &second_data;
    first_data.spwnd = &first.wnd; second_data.spwnd = &second.wnd;
    first_data.buttonState = 11; second_data.buttonState = 22;
    current_state = &first.wnd;
}
static void check_original_blit(wow_button_resources *value)
{
    HDC screen = GetDC(NULL), target = CreateCompatibleDC(screen);
    HBITMAP bitmap = CreateCompatibleBitmap(screen, 32, 32);
    HGDIOBJ previous = NULL;
    UINT index;
    CHECK(screen && target && bitmap);
    if (screen) ReleaseDC(NULL, screen);
    if (target && bitmap) previous = SelectObject(target, bitmap);
    if (previous && previous != HGDI_ERROR) {
        for (index = OBI_CHECK; index < OBI_COUNT; ++index) {
            const OEMBITMAPINFO *image = &value->bm[index];
            int x, y;
            CHECK(wow_button_BitBltSysBmp(target, 2, 3, index));
            for (y = 0; y < image->cy; ++y)
                for (x = 0; x < image->cx; ++x)
                    CHECK(GetPixel(target, x + 2, y + 3) ==
                        GetPixel(value->dc, image->x + x, image->y + y));
        }
        SelectObject(target, previous);
    } else CHECK(FALSE);
    if (bitmap) DeleteObject(bitmap);
    if (target) DeleteDC(target);
}
static LRESULT CALLBACK resource_inner(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    wow_button_resources *value = wow_button_current_resources();
    (void)window; (void)message; (void)wp;
    CHECK(value != (wow_button_resources *)lp && value->BitCount == 32);
    CHECK(!value->dc && !value->bitmap);
    CHECK(wow_button_resources_ready(value));
    check_original_blit(value);
    return 17;
}
static LRESULT CALLBACK resource_outer(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    wow_button_resources *value = wow_button_current_resources();
    (void)message; (void)wp; (void)lp;
    CHECK(value->BitCount == 32 && !value->dc && !value->bitmap);
    CHECK(wow_button_resources_ready(value));
    resource_dc = value->dc;
    check_original_blit(value);
    CHECK(wow_button_invoke(window, WM_PAINT, 0, (LPARAM)value, resource_inner) == 17);
    CHECK(wow_button_current_resources() == value && value->dc == resource_dc);
    check_original_blit(value);
    SetLastError(0x47117);
    if (exceptional) RaiseException(BUTTON_EXCEPTION, 0, 0, NULL);
    return 19;
}
int __cdecl main(void)
{
    unsigned before;
    DWORD returned_error, remaining_type;
    HDC native_control;
    wow_button_scope dialog_scope;
    PBUTN held_button;
    BOOL caught = FALSE;
    wow_edit_context context = {0};
    setup();
    CHECK(wow_button_invoke(first.wnd.head.h, BM_CLICK, 0, 0, outer) == 41);
    CHECK(frees == 2 && first_data.buttonState == 0xdead && GetLastError() == 0x47117);
    setup(); exceptional = TRUE; before = frees;
    __try { (void)wow_button_invoke(first.wnd.head.h, BM_CLICK, 0, 0, outer); }
    __except (GetExceptionCode() == BUTTON_EXCEPTION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
    CHECK(caught && frees == before + 2 && GetLastError() == 0x47117);
    /* No enclosing invocation: physical release is immediate and original. */
    before = frees;
    wow_button_free_state(&pool, &first_data);
    CHECK(frees == before + 1);
    /* Original dialog keyboard/mnemonic code retains PBUTN around focus,
     * without an outer Button WNDPROC. The same owner must cover that use. */
    setup(); before = frees;
    wow_button_scope_enter(&dialog_scope, &first.wnd);
    held_button = first.pbutn;
    __try {
        CHECK(wow_button_invoke(first.wnd.head.h, WM_NCDESTROY, 0, 0, destroy_state) == 37);
        CHECK(frees == before && held_button->spwnd == NULL);
        CHECK(held_button->buttonState == 11);
    } __finally { wow_button_scope_leave(&dialog_scope); }
    CHECK(frees == before + 1 && GetLastError() == 0x47117);
    /* The dialog has no outer Button invocation to perform this unwind. */
    setup(); before = frees; caught = FALSE;
    __try {
        wow_button_scope_enter(&dialog_scope, &first.wnd);
        held_button = first.pbutn;
        __try {
            CHECK(wow_button_invoke(first.wnd.head.h, WM_NCDESTROY, 0, 0, destroy_state) == 37);
            CHECK(frees == before && held_button->buttonState == 11);
            RaiseException(BUTTON_EXCEPTION, 0, 0, NULL);
        } __finally { wow_button_scope_leave(&dialog_scope); }
    } __except (GetExceptionCode() == BUTTON_EXCEPTION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        caught = TRUE;
    }
    CHECK(caught && frees == before + 1 && GetLastError() == 0x47117);
    /* A subsequent independent call must not see the dead stack frame. */
    setup(); before = frees;
    CHECK(wow_button_invoke(first.wnd.head.h, WM_NCDESTROY, 0, 0, destroy_state) == 37);
    CHECK(frees == before + 1);
    /* Cleanup above succeeds without a drawing context. Ordinary drawing
     * uses lazy, independently nested resources, including exception unwind. */
    setup(); exceptional = FALSE;
    context.screen.BitCount = 32;
    wow_edit_current = &context;
    native_control = CreateCompatibleDC(NULL);
    CHECK(native_control != NULL);
    CHECK(DeleteDC(native_control));
    printf("native_delete_control remaining_type=%lu\n", GetObjectType(native_control));
    CHECK(wow_button_invoke(first.wnd.head.h, WM_PAINT, 0, 0, resource_outer) == 19);
    returned_error = GetLastError();
    remaining_type = GetObjectType(resource_dc);
    printf("resource_normal error=%lu dc_type=%lu\n", returned_error, remaining_type);
    CHECK(returned_error == 0x47117);
    CHECK(released_dcs == 2 && released_bitmaps == 2 && last_released_dc == resource_dc);
    exceptional = TRUE; caught = FALSE;
    __try { (void)wow_button_invoke(first.wnd.head.h, WM_PAINT, 0, 0, resource_outer); }
    __except (GetExceptionCode() == BUTTON_EXCEPTION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
    returned_error = GetLastError();
    remaining_type = GetObjectType(resource_dc);
    printf("resource_exception caught=%u error=%lu dc_type=%lu\n", caught, returned_error, remaining_type);
    CHECK(caught && returned_error == 0x47117);
    CHECK(released_dcs == 4 && released_bitmaps == 4 && last_released_dc == resource_dc);
    wow_edit_current = NULL;
    printf("button_lifetime_failures=%u\n", failures);
    return failures ? 1 : 0;
}
