/****************************** Module Header ******************************\
* Module Name: getset.c
*
* Copyright (c) 1985 - 1999, Microsoft Corporation
*
* This module contains window manager information routines
*
* History:
* 22-Oct-1990 MikeHar   Ported functions from Win 3.0 sources.
* 13-Feb-1991 MikeKe    Added Revalidation code (None)
* 08-Feb-1991 IanJa     Unicode/ANSI aware and neutral
\***************************************************************************/

#include "precomp.h"
#pragma hdrstop

#line 248

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

ULONG_PTR xxxSetWindowLongPtr(
    PWND  pwnd,
    int   index,
    ULONG_PTR dwData,
    BOOL  bAnsi)
{
    ULONG_PTR dwOld;
    DWORD dwCPDType = 0;

    /*
     * Hide the window proc from other processes
     */
#if DBG
#error
#line 276
#endif
#line 277

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
            case DWLP_MSGRESULT:
#ifdef WOW_NATIVE_DIALOG_PROCEDURE_OWNER
                 return wow_dialog_host_set(HW(pwnd), index, dwData, bAnsi);
#else
#line 298
                 dwOld = (ULONG_PTR)((PDIALOG)(pwnd))->resultWP;
                 ((PDIALOG)(pwnd))->resultWP = (LONG_PTR)dwData;
                 return dwOld;
#endif
#line 301

            case DWLP_USER:
                 dwOld = (ULONG_PTR)((PDIALOG)(pwnd))->unused;
                 ((PDIALOG)(pwnd))->unused = (LONG_PTR)dwData;
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
#ifdef FE_IME
#error
#line 326
#endif
#line 327
                    /*
                     * Allow the 0 index for controls to be set if it's
                     * still NULL or the window is being destroyed. This
                     * is where controls store their private data.
                     */
                    if (index == 0) {
                        dwOld = *((PULONG_PTR)(pwnd + 1));
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
#ifndef _WIN64
                    if (index == 0) {
                        goto SetData;
                    }
#endif
                    if (index == GWLP_MDIDATA) {
                        dwOld = *((PULONG_PTR)(pwnd + 1));
                        if (dwOld == 0 || TestWF(pwnd, WFDESTROYED))
                            goto SetData;
                    }
                    break;
                }

                RIPERR3(ERROR_INVALID_INDEX,
                        RIP_WARNING,
                        "SetWindowLongPtr: Trying to set private server data pwnd=(%#p) index=(%ld) FNID=(%lX)",
                        pwnd, index, (DWORD)pwnd->fnid);
                return 0;
            }
        }
    }

    if (index < 0) {
        return xxxSetWindowData(pwnd, index, dwData, bAnsi);
    } else {
        if ((UINT)index + sizeof(ULONG_PTR) > (UINT)pwnd->cbwndExtra) {
            RIPERR3(ERROR_INVALID_INDEX,
                    RIP_WARNING,
                    "SetWindowLongPtr: Index %d too big for cbWndExtra %d on pwnd %#p",
                    index, pwnd->cbwndExtra, pwnd);
            return 0;
        } else {
            ULONG_PTR UNALIGNED *pudw;

SetData:
            pudw = (ULONG_PTR UNALIGNED *)((BYTE *)(pwnd + 1) + index);
            dwOld = *pudw;
            *pudw = dwData;
            return dwOld;
        }
    }
}

#line 391
#ifdef _WIN64
#error
#line 503
#endif
#line 504

#line 851

/***************************************************************************\
* FindPCPD
*
* Searches the list of CallProcData's associated with window to see if
* one already exists representing this transition.  CPD can be re-used
* and aren't deleted until a window or thread dies
*
*
* 04-Feb-1993 JohnC     Created.
\***************************************************************************/

PCALLPROCDATA FindPCPD(
    PCALLPROCDATA pCPD,
    ULONG_PTR      dwClientPrevious,
    WORD          wCPDType)
{
    while (pCPD) {
        if ((pCPD->pfnClientPrevious == dwClientPrevious) &&
                (pCPD->wType == wCPDType))
            return pCPD;
        pCPD = pCPD->spcpdNext;
    }

    return NULL;
}

#line 895
#if DBG
#error
#line 897
#endif
#line 898

#line 905
#if DBG
#error
#line 915
#endif
#line 916

#line 922
#if DBG
#error
#line 924
#endif
#line 925

#line 927
#if DBG
#error
#line 939
#endif // DBG
#line 940

#line 1011

ULONG DBGGetWindowLong(PWND pwnd, int index)
{
    UserAssert(index >= 0);
    UserAssert((UINT)index + sizeof(DWORD) <= (UINT)pwnd->cbwndExtra);
    return __GetWindowLong(pwnd, index);
}

#line 1018

ULONG_PTR DBGGetWindowLongPtr(PWND pwnd, int index)
{
    UserAssert(index >= 0);
    UserAssert((UINT)index + sizeof(ULONG_PTR) <= (UINT)pwnd->cbwndExtra);
    return __GetWindowLongPtr(pwnd, index);
}

HWND WINAPI wow_original_window_handle(PWND window)
{
    return window->head.h;
}

VOID WINAPI wow_original_change_compat_state(PWND window, BOOL set)
{
    if (set) SetWF(window, WFWIN40COMPAT);
    else ClrWF(window, WFWIN40COMPAT);
}

#include "compat/include/wow_page_domain.h"
VOID WINAPI wow_original_restore_window_words(struct _WW *words,
    const mvdm_softpc_wow_window_words_view *view)
{
    words->state = view->state;
    words->state2 = view->state2;
    words->style = view->style;
    words->ExStyle = view->exstyle;
    words->hModule = (HANDLE)(ULONG_PTR)view->module;
    words->hMod16 = view->module16;
    words->fnid = view->fnid;
}


/* Original USER layout operations. The caller owns storage and all locks;
 * these operations neither publish objects nor call application/guest code. */
#include "compat/include/wow_window_access_provider.h"

const wow_window_layout *WINAPI wow_original_window_layout(void)
{
    static const wow_window_layout layout = {
        sizeof(WND), sizeof(WW), sizeof(CLS),
        CW_FLAGS_ANSI, WS_EX_ANSICREATOR, WNDPROC_HANDLE, FNID_DIALOG
    };
    return &layout;
}

VOID WINAPI wow_original_capture_window_words(const struct _WW *words,
    mvdm_softpc_wow_window_words_view *view)
{
    view->state = words->state;
    view->state2 = words->state2;
    view->style = words->style;
    view->exstyle = words->ExStyle;
    view->module = (ULONG)(ULONG_PTR)words->hModule;
    view->module16 = words->hMod16;
    view->fnid = words->fnid;
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
        PWND next = destination->spwndNext, prev = destination->spwndPrev;
        PWND parent = destination->spwndParent;
        PWND child = destination->spwndChild, owner = destination->spwndOwner;
        previous = destination->strName.Buffer;
        CopyMemory(destination, source, sizeof(WND) + (SIZE_T)source->cbwndExtra);
        destination->head.pSelf = (PBYTE)destination;
        destination->spwndNext = next; destination->spwndPrev = prev;
        destination->spwndParent = parent;
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
    words->style = info->dwStyle;
    words->ExStyle = info->dwExStyle;
    words->hModule = (HANDLE)(ULONG_PTR)module;
    if (state) {
        words->state = state->state;
        words->state2 = state->state2;
        state->hMod16 = words->hMod16;
        words->fnid = state->fnid;
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
        state->hModule = words->hModule;
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
    state->state = words->state;
    state->state2 = words->state2;
    state->style = words->style;
    state->ExStyle = words->ExStyle;
    state->hModule = words->hModule;
    state->hMod16 = words->hMod16;
    state->fnid = words->fnid;
    state->spmenu = (PMENU)(ULONG_PTR)(DWORD)control_id;
    /* ADAPTER-WOW-063: shared creation-state projection from original createw.c
     * (ANSI transfer and lines 354--371 compatibility branches). The full
     * server creator cannot be composed; its caller provides these exact
     * source inputs. Native creation/style policy is not implemented here. */
    if (ansi_creator) SetWF(state, WFANSICREATOR);
    if (compatibility_flags & GACF_RANDOM3XUI) SetWF(state, WFOLDUI);
    if (Is310Compat(module_version)) {
        SetWF(state, WFWIN31COMPAT);
        if (Is400Compat(module_version)) {
            SetWF(state, WFWIN40COMPAT);
            if (Is500Compat(module_version))
                SetWF(state, WFWIN50COMPAT);
        }
    } else if (compatibility_flags & GACF_ALWAYSSENDNCPAINT)
        SetWF(state, WFALWAYSSENDNCPAINT);
}
