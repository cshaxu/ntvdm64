#include "wow_private_user_compat.h"

#include <string.h>

#define WOW_PRIVATE_USER_MAX_TASKS 16u

typedef struct wow_private_user_task_slot {
    DWORD thread_id;
    DWORD wow_task;
    HANDLE resume_event;
    BOOL waiting;
} WOW_PRIVATE_USER_TASK_SLOT;

static INIT_ONCE wow_private_user_scheduler_once = INIT_ONCE_STATIC_INIT;
static CRITICAL_SECTION wow_private_user_scheduler_lock;
static WOW_PRIVATE_USER_TASK_SLOT
    wow_private_user_task_slots[WOW_PRIVATE_USER_MAX_TASKS];

static BOOL CALLBACK wow_private_user_initialize_scheduler(PINIT_ONCE once,
    PVOID parameter, PVOID *context)
{
    UNREFERENCED_PARAMETER(once);
    UNREFERENCED_PARAMETER(parameter);
    UNREFERENCED_PARAMETER(context);
    InitializeCriticalSection(&wow_private_user_scheduler_lock);
    return TRUE;
}

static WOW_PRIVATE_USER_TASK_SLOT *wow_private_user_find_task(DWORD thread_id)
{
    DWORD index;

    for (index = 0u; index < WOW_PRIVATE_USER_MAX_TASKS; ++index) {
        if (wow_private_user_task_slots[index].thread_id == thread_id) {
            return &wow_private_user_task_slots[index];
        }
    }
    return NULL;
}

static BOOL wow_private_user_register_task(DWORD wow_task)
{
    DWORD index;
    DWORD thread_id = GetCurrentThreadId();
    WOW_PRIVATE_USER_TASK_SLOT *slot;

    if (!InitOnceExecuteOnce(&wow_private_user_scheduler_once,
            wow_private_user_initialize_scheduler, NULL, NULL)) {
        return FALSE;
    }
    EnterCriticalSection(&wow_private_user_scheduler_lock);
    slot = wow_private_user_find_task(thread_id);
    if (slot != NULL) {
        slot->wow_task = wow_task;
        LeaveCriticalSection(&wow_private_user_scheduler_lock);
        return TRUE;
    }
    for (index = 0u; index < WOW_PRIVATE_USER_MAX_TASKS; ++index) {
        slot = &wow_private_user_task_slots[index];
        if (slot->thread_id == 0u) {
            slot->resume_event = CreateEvent(NULL, FALSE, FALSE, NULL);
            if (slot->resume_event != NULL) {
                slot->thread_id = thread_id;
                slot->wow_task = wow_task;
                slot->waiting = FALSE;
            }
            LeaveCriticalSection(&wow_private_user_scheduler_lock);
            return slot->resume_event != NULL;
        }
    }
    LeaveCriticalSection(&wow_private_user_scheduler_lock);
    return FALSE;
}

static BOOL wow_private_user_yield_to(DWORD requested_thread_id)
{
    DWORD index;
    DWORD current_thread_id = GetCurrentThreadId();
    HANDLE wait_event = NULL;
    HANDLE signal_event = NULL;
    BOOL target_was_waiting = FALSE;
    DWORD wait_result;
    WOW_PRIVATE_USER_TASK_SLOT *current;
    WOW_PRIVATE_USER_TASK_SLOT *target = NULL;

    if (!InitOnceExecuteOnce(&wow_private_user_scheduler_once,
            wow_private_user_initialize_scheduler, NULL, NULL)) {
        return FALSE;
    }
    EnterCriticalSection(&wow_private_user_scheduler_lock);
    current = wow_private_user_find_task(current_thread_id);
    if (current == NULL) {
        LeaveCriticalSection(&wow_private_user_scheduler_lock);
        SwitchToThread();
        return TRUE;
    }
    if (requested_thread_id != 0u) {
        target = wow_private_user_find_task(requested_thread_id);
        if (target == current) target = NULL;
    }
    if (target == NULL) {
        for (index = 0u; index < WOW_PRIVATE_USER_MAX_TASKS; ++index) {
            WOW_PRIVATE_USER_TASK_SLOT *candidate =
                &wow_private_user_task_slots[index];
            if (candidate->thread_id != 0u && candidate != current) {
                target = candidate;
                break;
            }
        }
    }
    if (target == NULL) {
        LeaveCriticalSection(&wow_private_user_scheduler_lock);
        SwitchToThread();
        return TRUE;
    }

    /* The original USER scheduler lets the current guest task sleep until a
     * different task yields back.  Do not signal a task that is still running:
     * an auto-reset token left behind would let two CCPU guest executions run
     * concurrently on the next yield.  Conversely, when the peer is already
     * asleep, make the wake-and-sleep hand-off atomic.  A SetEvent followed by
     * WaitForSingleObject has a runnable interval in which both original WOW
     * task threads can resume CCPU even though its register state is shared. */
    ResetEvent(current->resume_event);
    current->waiting = TRUE;
    target_was_waiting = target->waiting;
    if (target_was_waiting) signal_event = target->resume_event;
    wait_event = current->resume_event;
    LeaveCriticalSection(&wow_private_user_scheduler_lock);

    if (target_was_waiting) {
        wait_result = SignalObjectAndWait(signal_event, wait_event, INFINITE,
            FALSE);
    } else {
        wait_result = WaitForSingleObject(wait_event, INFINITE);
    }

    EnterCriticalSection(&wow_private_user_scheduler_lock);
    current->waiting = FALSE;
    LeaveCriticalSection(&wow_private_user_scheduler_lock);
    return wait_result == WAIT_OBJECT_0;
}

BOOL wow_private_user_register_hung_app_handlers(void *end_task_callback,
    HANDLE wowexec_event)
{
    /* DIVERGENCE(ADAPTER-WOW-012): `UserRegisterWowHandlers` is an NT4
     * USER-private table contract. Current USER32 exports the old symbol but
     * does not populate the historical callback table. This particular slot
     * only registers USER's hung-app broker -> WOW callback. The standalone
     * CLI has no private USER broker and no producer for that callback, so
     * retain the successful no-notification direction without inventing a
     * broker, task policy, thread, or callback invocation. The original
     * callback and event are deliberately validated as present: this is not a
     * general success stub for malformed callers. */
    if (end_task_callback == NULL || wowexec_event == NULL)
        return FALSE;
    return TRUE;
}

WORD WINAPI wow_private_user_get_def_window_proc_bits(PBYTE bits,
    WORD byte_count)
{
    /* DIVERGENCE(ADAPTER-WOW-013): this one NT4 output slot tells USER16
     * which messages USER32 owns by its private default-procedure table.
     * Public USER32 exposes no such table.  USER16's own CheckDefWindowProc
     * gate returns zero before the retained WOW32 DEFWINDOWPROC thunk unless
     * the appropriate bit is present.  WM_NCCREATE is such a message: its
     * original WOW32 thunk has a dedicated WM32Create marshaller, and Win32
     * DefWindowProc must return TRUE for normal non-client creation.
     * WM_NCCALCSIZE is the immediately following creation-stage default
     * operation: it calculates the projected non-client/client geometry.
     * Without these bits a projected WOW window can either be cancelled
     * before WM_CREATE or retain no valid default non-client calculation.
     *
     * Keep the caller-owned buffer and bounded result contract.  This is the
     * smallest source-evidenced subset of the unavailable private table; more
     * bits are admitted only with a corresponding retained marshaller and a
     * runtime witness.
     */
    if (bits == NULL || byte_count == 0u) return 0u;
    memset(bits, 0, byte_count);
    if (byte_count <= (WM_NCCALCSIZE >> 3)) return 0u;
    bits[WM_NCCREATE >> 3] |= 1u << (WM_NCCREATE & 7u);
    bits[WM_NCCALCSIZE >> 3] |= 1u << (WM_NCCALCSIZE & 7u);
    return WM_NCCALCSIZE;
}

BOOL WINAPI wow_private_user_init_task(UINT expected_windows_version,
    LPCSTR application_name, DWORD wow_task, DWORD hotkey, DWORD task_id,
    DWORD x, DWORD y, DWORD width, DWORD height, WORD show_command)
{
    /* DIVERGENCE(ADAPTER-WOW-014): NT4 USER's pfnInitTask slot admitted a
     * freshly-created WOW task to USER's private task broker and applied its
     * initial show placement.  Current USER32 exports the registration
     * symbol but leaves that slot empty.  The standalone container already
     * owns task creation and the public window projection owns later window
     * registration, so this bridge admits only a structurally valid WOW task
     * without claiming an absent USER-private broker or placement policy. */
    UNREFERENCED_PARAMETER(expected_windows_version);
    UNREFERENCED_PARAMETER(hotkey);
    UNREFERENCED_PARAMETER(task_id);
    UNREFERENCED_PARAMETER(x);
    UNREFERENCED_PARAMETER(y);
    UNREFERENCED_PARAMETER(width);
    UNREFERENCED_PARAMETER(height);
    UNREFERENCED_PARAMETER(show_command);

    return application_name != NULL && application_name[0] != '\0' &&
        wow_task != 0u && wow_private_user_register_task(wow_task);
}

BOOL WINAPI wow_private_user_yield_task(void)
{
    /* DIVERGENCE(ADAPTER-WOW-017): NT4 USER scheduled WOW tasks through its
     * private broker.  The standalone retains the original WOW32 task-thread
     * creation and creator-event handshake, but modern USER32 leaves this
     * output slot null.  A bounded public scheduler yield is the remaining
     * contract at this seam: it gives the already-created task thread an OS
     * scheduling opportunity without introducing a USER task list, selecting
     * a foreground window, or retaining any task state. */
    return wow_private_user_yield_to(0u);
}

VOID WINAPI wow_private_user_directed_yield(DWORD thread_id)
{
    /* DIVERGENCE(ADAPTER-WOW-017): public Win32 has no safe equivalent of
     * USER's private directed WOW scheduler.  Preserve the yield direction
     * without fabricating a target-task broker; the thread identifier remains
     * an opaque USER-private value and must not be converted to a host handle.
     */
    (void)wow_private_user_yield_to(thread_id);
}

BOOL WINAPI wow_private_user_cleanup(HANDLE instance, DWORD wow_task,
    void *module_segments, DWORD module_segment_count)
{
    /* DIVERGENCE(ADAPTER-WOW-018): NT4 USER's private server removed USER
     * objects associated with a departing WOW task/module through this slot.
     * The standalone uses public projected windows, whose lifetime is owned
     * by their public DestroyWindow path and finally by session teardown;
     * no private USER object or server-side module state was created here.
     * Accept the original cleanup notification without inventing a second
     * USER object manager or dereferencing the original server-only segment
     * carrier. */
    UNREFERENCED_PARAMETER(instance);
    UNREFERENCED_PARAMETER(wow_task);
    UNREFERENCED_PARAMETER(module_segments);
    UNREFERENCED_PARAMETER(module_segment_count);
    return TRUE;
}

int WINAPI wow_private_user_get_id_from_directory(PBYTE resource_bits,
    UINT resource_type)
{
    /* DIVERGENCE(ADAPTER-WOW-015): NT4 USER supplied the private
     * pfnWOWGetIdFromDirectory slot for USER16's grouped icon/cursor lookup.
     * The equivalent public USER32 capability remains available as
     * LookupIconIdFromDirectory.  Retain the original resource-directory
     * selection semantics, without projecting or retaining any USER-private
     * state. */
    if (resource_bits == NULL ||
        (resource_type != RT_ICON && resource_type != RT_CURSOR)) {
        return 0;
    }
    return LookupIconIdFromDirectory(resource_bits, resource_type == RT_ICON);
}

static HCURSOR wow_private_user_create_legacy_dib_cursor_icon(
    PBYTE resource_bits, DWORD resource_size, BOOL f_icon)
{
    const BITMAPINFOHEADER *source;
    DWORD colors;
    DWORD header_size;
    DWORD color_stride;
    DWORD mask_stride;
    DWORD color_size;
    DWORD mask_size;
    DWORD required_size;
    BYTE *header_copy;
    BITMAPINFOHEADER *header;
    BYTE *color_source;
    BYTE *mask_source;
    BYTE *color_destination;
    BYTE *xor_bits;
    HDC screen_dc;
    HDC color_dc;
    HDC mono_dc;
    HBITMAP color_bitmap;
    HBITMAP mono_bitmap;
    HBITMAP old_color_bitmap;
    HBITMAP old_mono_bitmap;
    ICONINFO icon_info;
    SHORT hotspot_x = 0;
    SHORT hotspot_y = 0;
    HCURSOR result;

    /* This is the public-GDI reconstruction of NTUSER's
     * WowServerLoadCreateCursorIcon -> ConvertDIBIcon path. USER16
     * supplies a legacy DIB (not a native module resource): its first half
     * is the colour/XOR image and the following monochrome rows are the AND
     * mask. Convert the former through GDI just as the original cursor route
     * requested LR_MONOCHROME only for cursors.  Icons preserve their colour
     * bitmap and pair it with the AND mask through CreateIconIndirect; the
     * cursor case converts its XOR image to monochrome and uses the paired
     * public cursor masks.  Cursor resources retain the two original
     * leading hotspot words, exactly as ConvertDIBIcon consumes them. */
    if (!f_icon) {
        if (resource_bits == NULL || resource_size < 2u * sizeof(WORD)) {
            return NULL;
        }
        hotspot_x = (SHORT)*(const WORD *)resource_bits;
        hotspot_y = (SHORT)*(const WORD *)(resource_bits + sizeof(WORD));
        resource_bits += 2u * sizeof(WORD);
        resource_size -= 2u * sizeof(WORD);
    }
    source = (const BITMAPINFOHEADER *)resource_bits;
    if (resource_bits == NULL ||
        resource_size < sizeof(BITMAPINFOHEADER) ||
        source->biSize != sizeof(BITMAPINFOHEADER) ||
        source->biWidth <= 0 || source->biHeight <= 0 ||
        (source->biHeight & 1) != 0 || source->biPlanes != 1 ||
        source->biBitCount == 0 || source->biBitCount > 8 ||
        source->biCompression != BI_RGB) {
        return NULL;
    }
    colors = source->biClrUsed != 0 ? source->biClrUsed :
        (1u << source->biBitCount);
    if (colors > 256u) {
        return NULL;
    }
    header_size = sizeof(BITMAPINFOHEADER) + colors * sizeof(RGBQUAD);
    color_stride = (((DWORD)source->biWidth * source->biBitCount + 31u) &
        ~31u) / 8u;
    mask_stride = (((DWORD)source->biWidth + 31u) & ~31u) / 8u;
    color_size = color_stride * ((DWORD)source->biHeight / 2u);
    mask_size = mask_stride * ((DWORD)source->biHeight / 2u);
    required_size = header_size + color_size + mask_size;
    if (required_size > resource_size) {
        return NULL;
    }

    header_copy = (BYTE *)LocalAlloc(LPTR, header_size);
    xor_bits = (BYTE *)LocalAlloc(LPTR, mask_size);
    if (header_copy == NULL || xor_bits == NULL) {
        LocalFree(header_copy);
        LocalFree(xor_bits);
        return NULL;
    }
    RtlCopyMemory(header_copy, resource_bits, header_size);
    header = (BITMAPINFOHEADER *)header_copy;
    header->biHeight /= 2;
    color_source = resource_bits + header_size;
    mask_source = color_source + color_size;

    screen_dc = GetDC(NULL);
    color_dc = screen_dc == NULL ? NULL : CreateCompatibleDC(screen_dc);
    mono_dc = screen_dc == NULL ? NULL : CreateCompatibleDC(screen_dc);
    color_bitmap = screen_dc == NULL ? NULL : CreateDIBSection(screen_dc,
        (BITMAPINFO *)header_copy, DIB_RGB_COLORS,
        (void **)&color_destination, NULL, 0);
    mono_bitmap = CreateBitmap(header->biWidth, header->biHeight, 1, 1,
        NULL);
    old_color_bitmap = NULL;
    old_mono_bitmap = NULL;
    result = NULL;
    if (color_dc != NULL && mono_dc != NULL && color_bitmap != NULL &&
        mono_bitmap != NULL) {
        RtlCopyMemory(color_destination, color_source, color_size);
        old_color_bitmap = (HBITMAP)SelectObject(color_dc, color_bitmap);
        old_mono_bitmap = (HBITMAP)SelectObject(mono_dc, mono_bitmap);
        if (old_color_bitmap != NULL && old_mono_bitmap != NULL) {
            if (SetBitmapBits(mono_bitmap, mask_size, mask_source) !=
                mask_size) {
            } else if (f_icon) {
                RtlZeroMemory(&icon_info, sizeof(icon_info));
                icon_info.fIcon = TRUE;
                icon_info.hbmColor = color_bitmap;
                icon_info.hbmMask = mono_bitmap;
                result = (HCURSOR)CreateIconIndirect(&icon_info);
            } else if (BitBlt(mono_dc, 0, 0, header->biWidth,
                    header->biHeight, color_dc, 0, 0, SRCCOPY) &&
                GetBitmapBits(mono_bitmap, mask_size, xor_bits) ==
                    mask_size) {
                result = CreateCursor(NULL, hotspot_x, hotspot_y,
                    header->biWidth, header->biHeight, mask_source, xor_bits);
            }
        }
        if (old_color_bitmap != NULL) {
            SelectObject(color_dc, old_color_bitmap);
        }
        if (old_mono_bitmap != NULL) {
            SelectObject(mono_dc, old_mono_bitmap);
        }
    }
    if (color_bitmap != NULL) {
        DeleteObject(color_bitmap);
    }
    if (mono_bitmap != NULL) {
        DeleteObject(mono_bitmap);
    }
    if (color_dc != NULL) {
        DeleteDC(color_dc);
    }
    if (mono_dc != NULL) {
        DeleteDC(mono_dc);
    }
    if (screen_dc != NULL) {
        ReleaseDC(NULL, screen_dc);
    }
    LocalFree(xor_bits);
    LocalFree(header_copy);
    return result;
}

HCURSOR WINAPI wow_private_user_load_create_cursor_icon(HANDLE module,
    LPTSTR module_name, DWORD expected_windows_version, LPCTSTR name,
    DWORD resource_size, PVOID resource_bits, LPTSTR resource_type,
    BOOL client_side)
{
    UINT type = (UINT)(UINT_PTR)resource_type;

    /* DIVERGENCE(ADAPTER-WOW-016): the original output slot constructed a
     * USER-server cursor/icon from already-expanded Win16 resource bits.
     * CreateIconFromResourceEx is the public USER32 equivalent for that exact
     * payload.  A null-module integer name is the original standard
     * cursor/icon case: it must recover the public shared stock object so
     * WU32LoadCursor can find the alias installed by InitStdCursorIconAlias.
     * The remaining fallback is restricted to an ordinary module resource
     * load; module_name, placement and client/server distinctions belonged to
     * the vanished private server and are intentionally not recreated. */
    UNREFERENCED_PARAMETER(module_name);
    UNREFERENCED_PARAMETER(client_side);
    if (type != RT_ICON && type != RT_CURSOR) {
        return NULL;
    }
    if (resource_bits != NULL && resource_size != 0u) {
        HCURSOR result = wow_private_user_create_legacy_dib_cursor_icon(
            (PBYTE)resource_bits, resource_size, type == RT_ICON);
        if (result == NULL && type == RT_ICON) {
            result = (HCURSOR)CreateIconFromResourceEx((PBYTE)resource_bits,
                resource_size, TRUE, expected_windows_version, 0, 0,
                LR_DEFAULTCOLOR);
        }
        if (result != NULL) {
            return result;
        }
    }
    if (name == NULL) {
        return NULL;
    }
    if (module == NULL) {
        if (HIWORD(name) != 0) {
            return NULL;
        }
        return type == RT_ICON ? (HCURSOR)LoadIcon(NULL, name) :
            LoadCursor(NULL, name);
    }
    return (HCURSOR)LoadImage((HINSTANCE)module, name,
        type == RT_ICON ? IMAGE_ICON : IMAGE_CURSOR, 0, 0, LR_DEFAULTCOLOR);
}

HMENU WINAPI wow_private_user_load_create_menu(HANDLE module, LPTSTR name,
    CONST LPMENUTEMPLATE menu_template, DWORD resource_size,
    BOOL call_client)
{
    /* DIVERGENCE(ADAPTER-WOW-020): USER16 has already found and locked the
     * original Win16 RT_MENU resource before WU32LoadMenu enters this NT4
     * private output slot.  WU32LoadMenu retains the original ConvertMenu16
     * call and passes its resulting public Unicode menu template here.  NT4
     * routed that template through WowServerLoadCreateMenu to
     * CreateMenuFromResource.  The corresponding public USER32 entrypoint is
     * LoadMenuIndirectW: it consumes the same converted template and returns
     * a public HMENU, without treating the packed HMODINST32 value as a native
     * module instance or recreating USER's resource server/cache.
     *
     * The historical module/name/client fields identify server bookkeeping,
     * not the template payload.  They remain deliberately unobserved.  The
     * source route has already rejected an absent resource; retain that
     * failure direction for a missing/empty template. */
    UNREFERENCED_PARAMETER(module);
    UNREFERENCED_PARAMETER(name);
    UNREFERENCED_PARAMETER(call_client);
    if (menu_template == NULL || resource_size == 0u) {
        return NULL;
    }
    return LoadMenuIndirectW(menu_template);
}
