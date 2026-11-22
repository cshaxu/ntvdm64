/****************************** Module Header ******************************\
* Module Name: getset.c
*
* Copyright (c) 1985-1995, Microsoft Corporation
*
* This module contains window manager information routines
*
* History:
* 22-Oct-1990 MikeHar   Ported functions from Win 3.0 sources.
* 13-Feb-1991 MikeKe    Added Revalidation code (None)
* 08-Feb-1991 IanJa     Unicode/ANSI aware and neutral
\***************************************************************************/

#line 14
#include "precomp.h"
#pragma hdrstop

HWND WINAPI wow_original_window_handle(PWND window)
{
    return window->head.h;
}

VOID WINAPI wow_original_change_compat_state(PWND window, BOOL set)
{
    if (set) SetWF(window, WFWIN40COMPAT);
    else ClrWF(window, WFWIN40COMPAT);
}

#line 245
/***************************************************************************\
* xxxSetWindowLong (API)
*
* Set a window long.  Positive index values set application window longs
* while negative index values set system window longs.  The negative
* indices are published in WINDOWS.H.
*
* History:
* 26-Nov-1990 DarrinM   Wrote.
\***************************************************************************/

#line 256
DWORD xxxSetWindowLong(
    PWND  pwnd,
    int   index,
    DWORD dwData,
    BOOL  bAnsi)
{
    DWORD dwOld;
    DWORD dwCPDType = 0;

    /*
     * Hide the window proc from other processes
     */
#line 268
#ifdef DEBUG
#line 269
#error
#line 272
#endif
#line 273

    /*
     * CheckLock(pwnd);  The only case that leaves the critical section is
     * where xxxSetWindowData is called, which does this.  Saves us some locks.
     *
     *
     * Don't allow setting of words belonging to a system thread if the caller
     * is not a system thread. Same goes for winlogon.
     */
    if (!FCallerOk(pwnd)) {
        RIPERR0(ERROR_INVALID_INDEX, RIP_VERBOSE, "");
        return 0;
    }

    /*
     * If it's a dialog window, only a few indices are permitted.
     */
    if (GETFNID(pwnd) != 0) {
        if (TestWF(pwnd, WFDIALOGWINDOW)) {
            switch (index) {
            case DWL_MSGRESULT:
#ifdef WOW_NATIVE_DIALOG_PROCEDURE_OWNER
                 return wow_dialog_host_set(HW(pwnd), index, dwData, bAnsi);
#else
#line 294
                 dwOld = (DWORD)((PDIALOG)(pwnd))->resultWP;
                 ((PDIALOG)(pwnd))->resultWP = (long)dwData;
                 return dwOld;
#endif

#line 298
            case DWL_USER:
                 dwOld = (DWORD)((PDIALOG)(pwnd))->unused;
                 ((PDIALOG)(pwnd))->unused = (long)dwData;
                 return dwOld;

            default:
                if (index >= 0 && index < DLGWINDOWEXTRA) {
                    RIPERR0(ERROR_PRIVATE_DIALOG_INDEX, RIP_VERBOSE, "");
                    return 0;
                }
            }
        } else {
            if (index >= 0 &&
                    (index < (int)(CBFNID(pwnd->fnid)-sizeof(WND)))) {
                switch (GETFNID(pwnd)) {
                case FNID_BUTTON:
                case FNID_COMBOBOX:
                case FNID_COMBOLISTBOX:
                case FNID_DIALOG:
                case FNID_LISTBOX:
                case FNID_STATIC:
                case FNID_EDIT:
#line 320
#ifdef FE_IME
#line 321
#error
#endif
#line 323
                    /*
                     * Allow the 0 index for controls to be set if it's
                     * still NULL or the window is being destroyed. This
                     * is where controls store their private data.
                     */
                    if (index == 0) {
                        dwOld = *((LPDWORD)(pwnd + 1));
                        if (dwOld == 0 || TestWF(pwnd, WFDESTROYED))
                            goto SetData;
                    }
                    break;

                case FNID_MDICLIENT:
                    /*
                     * Allow the 0 index (which is reserved) to be set/get.
                     * Quattro Pro 1.0 uses this index!
                     *
                     * Allow the 4 index to be set if it's still NULL or
                     * the window is being destroyed. This is where we
                     * store our private data.
                     */
                    if (index == 0) {
                        goto SetData;
                    }
                    if (index == 4) {
                        dwOld = *((LPDWORD)(pwnd + 1));
                        if (dwOld == 0 || TestWF(pwnd, WFDESTROYED))
                            goto SetData;
                    }
                    break;
                }

                RIPERR3(ERROR_INVALID_INDEX,
                        RIP_WARNING,
                        "SetWindowLong: Trying to set private server data pwnd=(%lX) index=(%ld) FNID=(%lX)",
                        pwnd, index, (DWORD)pwnd->fnid);
                return 0;
            }
        }
    }

    if (index < 0) {
        return xxxSetWindowData(pwnd, index, dwData, bAnsi);
    } else {
        if (index + (int)sizeof(DWORD) > pwnd->cbwndExtra) {
            RIPERR0(ERROR_INVALID_INDEX, RIP_VERBOSE, "");
            return 0;
        } else {
            DWORD UNALIGNED *pudw;

SetData:
            pudw = (DWORD UNALIGNED *)((BYTE *)(pwnd + 1) + index);
            dwOld = *pudw;
            *pudw = dwData;
            return dwOld;
        }
    }
}


#line 749
#ifdef DEBUG
#line 750
#error
#endif
#line 752

#line 759
#ifdef DEBUG
#line 760
#error
#line 769
#endif
#line 770

#line 776
#ifdef DEBUG
#line 777
#error
#endif
#line 779

#line 781
#ifdef DEBUG
#line 782
#error
#line 793
#endif // DEBUG
#line 794

#include "compat/include/wow_page_domain.h"
DWORD *WINAPI wow_original_window_words(struct _WW *words)
{
    return words->adwWOW;
}
VOID WINAPI wow_original_restore_window_words(struct _WW *words,
    const mvdm_softpc_wow_window_words_view *view)
{
    C_ASSERT(sizeof(words->adwWOW) == sizeof(view->words));
    CopyMemory(words->adwWOW, view->words, sizeof(view->words));
    words->dwStyle = view->style;
    words->dwExStyle = view->exstyle;
    words->hInstance = (HANDLE)(ULONG_PTR)view->module;
}


/* Original USER layout operations. The caller owns storage and all locks;
 * these operations neither publish objects nor call application/guest code. */
#include "compat/include/wow_window_access_provider.h"

const wow_window_layout *WINAPI wow_original_window_layout(void)
{
    static const wow_window_layout layout = {
        sizeof(WND), sizeof(WW), sizeof(CLS), WND_CNT_WOWDWORDS,
        CW_FLAGS_ANSI, WS_EX_ANSICREATOR, WNDPROC_HANDLE, FNID_DIALOG
    };
    return &layout;
}

VOID WINAPI wow_original_capture_window_words(const struct _WW *words,
    mvdm_softpc_wow_window_words_view *view)
{
    C_ASSERT(sizeof(words->adwWOW) == sizeof(view->words));
    CopyMemory(view->words, words->adwWOW, sizeof(view->words));
    view->style = words->dwStyle;
    view->exstyle = words->dwExStyle;
    view->module = (ULONG)(ULONG_PTR)words->hInstance;
}

SIZE_T WINAPI wow_original_window_state_extent(const struct tagWND *state)
{
    return state->cbwndExtra < 0 ? (SIZE_T)-1 :
        sizeof(WND) + (SIZE_T)state->cbwndExtra;
}

WCHAR *WINAPI wow_original_promote_window_state(struct tagWND *destination,
    struct tagWND *source, struct tagCLS *identity)
{
    WCHAR *previous = NULL;
    source->pcls = identity;
    if (destination) {
        PWND next = destination->spwndNext, parent = destination->spwndParent;
        PWND child = destination->spwndChild, owner = destination->spwndOwner;
        previous = destination->strName.Buffer;
        CopyMemory(destination, source, sizeof(WND) + (SIZE_T)source->cbwndExtra);
        destination->head.pSelf = (PBYTE)destination;
        destination->spwndNext = next; destination->spwndParent = parent;
        destination->spwndChild = child; destination->spwndOwner = owner;
    }
    return previous;
}

VOID WINAPI wow_original_initialize_resource_window(struct tagWND *state,
    struct tagCLS *identity, const mvdm_softpc_wow_window_observation *view,
    HWND desktop)
{
    state->head.h = (HWND)(ULONG_PTR)view->links.window;
    state->head.pSelf = (PBYTE)state;
    CopyMemory(&state->rcWindow, view->window_rect, sizeof(RECT));
    CopyMemory(&state->rcClient, view->client_rect, sizeof(RECT));
    state->style = view->style; state->ExStyle = view->exstyle;
    state->hModule = (HANDLE)(ULONG_PTR)view->module;
    if (TestwndChild(state)) state->spmenu = (PMENU)(ULONG_PTR)view->control_id;
    if (state->head.h == desktop) state->fnid = FNID_DESKTOP;
    identity->atomClassName = view->descriptor.atom;
    state->pcls = identity;
}

WCHAR *WINAPI wow_original_refresh_window_state(struct tagWND *state,
    struct _WW *words, const WINDOWINFO *info, DWORD control_id, DWORD module,
    WCHAR *name, ULONG name_length, ULONG name_capacity)
{
    WCHAR *previous = NULL;
    BOOL child = (info->dwStyle & (WS_CHILD | WS_POPUP)) == WS_CHILD;
    words->dwStyle = info->dwStyle;
    words->dwExStyle = info->dwExStyle;
    words->hInstance = (HANDLE)(ULONG_PTR)module;
    if (state) {
        if (name) {
            previous = state->strName.Buffer;
            state->strName.Buffer = name;
            state->strName.Length = name_length;
            state->strName.MaximumLength = name_capacity;
            state->strName.bAnsi = FALSE;
        }
        if (child || TestwndChild(state)) state->spmenu = (PMENU)(ULONG_PTR)control_id;
        state->rcWindow = info->rcWindow; state->rcClient = info->rcClient;
        state->style = info->dwStyle; state->ExStyle = info->dwExStyle;
        state->hModule = words->hInstance;
    }
    return previous;
}

VOID WINAPI wow_original_publish_window_links(struct tagWND *state,
    struct tagCLS *resource_identity, ATOM atom, struct tagWND *next,
    struct tagWND *parent, struct tagWND *child, struct tagWND *owner)
{
    if (resource_identity) resource_identity->atomClassName = atom;
    state->spwndNext = next; state->spwndParent = parent;
    state->spwndChild = child; state->spwndOwner = owner;
}

WCHAR *WINAPI wow_original_window_name(struct tagWND *state)
{
    return state->strName.Buffer;
}

WORD WINAPI wow_original_window_fnid(struct tagWND *state)
{
    return GETFNID(state);
}

WORD WINAPI wow_original_mark_window_destroyed(struct tagWND *state)
{
    SetWF(state, WFDESTROYED);
    return GETFNID(state);
}

BOOL WINAPI wow_original_creation_class_name(const struct _LARGE_STRING *source,
    UNICODE_STRING *name)
{
    if (!HIWORD((ULONG_PTR)source)) {
        name->Buffer = (PWSTR)source;
        name->Length = name->MaximumLength = 0;
        return TRUE;
    }
    if (source->bAnsi || (source->Length & 1) || source->Length > 0xffffu ||
            source->Length > source->MaximumLength || !source->Buffer) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    name->Buffer = source->Buffer;
    name->Length = name->MaximumLength = (USHORT)source->Length;
    return TRUE;
}

VOID WINAPI wow_original_initialize_window_state(struct tagWND *state,
    const wow_window_state_initialization *input)
{
    const struct _WW *words = input->words;
    HWND window = input->window;
    const WINDOWINFO info = input->info;
    int class_extra_bytes = input->class_extra_bytes;
    DWORD module_version = input->module_version;
    DWORD compatibility_flags = input->compatibility_flags;
    DWORD control_id = input->control_id;
    BOOL ansi_creator = input->ansi_creator;
    WCHAR *name = input->name;
    ULONG name_length = input->name_length, name_capacity = input->name_capacity;
    state->strName.Buffer = name;
    state->strName.Length = name_length;
    state->strName.MaximumLength = name_capacity;
    state->strName.bAnsi = FALSE;
    state->head.h = window;
    state->head.pSelf = (PBYTE)state;
    state->cbwndExtra = class_extra_bytes;
    state->rcWindow = info.rcWindow;
    state->rcClient = info.rcClient;
    CopyMemory(state->adwWOW, words->adwWOW, sizeof(state->adwWOW));
    state->style = words->dwStyle;
    state->ExStyle = words->dwExStyle;
    state->hModule = words->hInstance;
    state->spmenu = (PMENU)(ULONG_PTR)(DWORD)control_id;
    /* ADAPTER-WOW-063: shared creation-state projection from original createw.c
     * (ANSI transfer and lines 354--371 compatibility branches). The full
     * server creator cannot be composed; its caller provides these exact
     * source inputs. Native creation/style policy is not implemented here. */
    if (ansi_creator) SetWF(state, WFANSICREATOR);
    if (compatibility_flags & GACF_RANDOM3XUI) SetWF(state, WFOLDUI);
    state->dwExpWinVer = (DWORD)LOWORD(module_version);
    if (Is310Compat(state->dwExpWinVer)) {
        SetWF(state, WFWIN31COMPAT);
        if (Is400Compat(state->dwExpWinVer))
            SetWF(state, WFWIN40COMPAT);
    } else if (compatibility_flags & GACF_ALWAYSSENDNCPAINT)
        SetWF(state, WFALWAYSSENDNCPAINT);
}
