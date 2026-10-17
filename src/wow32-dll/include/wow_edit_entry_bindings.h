#ifndef WOW_EDIT_ENTRY_BINDINGS_H
#define WOW_EDIT_ENTRY_BINDINGS_H
#include <windows.h>
#include "wow_window_client_state_bindings.h"

/* ADAPTER-WOW-058: finite EDIT and recovered list/combo system inputs.
 * An invocation owns this snapshot; it is not modern USER's SERVERINFO. */
typedef struct wow_edit_context {
    int wMaxLeftOverlapChars, wMaxRightOverlapChars;
    int cxSysFontChar, cySysFontChar;
    TEXTMETRICW tmSysFont;
    RECT screen_rect; /* Original clinit DISPLAY desktop-resolution input. */
    struct { UINT BitCount; } screen; /* Original drawing display-depth input. */
    UINT dtLBSearch, dtScroll, dtCaretBlink, ucWheelScrollLines;
    ATOM atomSysClass[20]; /* Snapshot of the existing process cleanup class table. */
    /* Dialog-only inputs; ordinary EDIT capture leaves these unused. */
    BOOL fSnapTo;
    HFONT hMsgFont;
    RECT rcWork;
} wow_edit_context;
extern __declspec(thread) wow_edit_context *wow_edit_current;
/* Original clglobal.c owns this process-wide partial-wheel accumulator.
 * Unlike per-call metric inputs, it must survive entry/context restoration. */
extern int gcWheelDelta;
BOOL WINAPI wow_edit_capture_context(wow_edit_context *);
#ifdef WOW_ORIGINAL_EDIT_CLIENT
#define gpsi wow_edit_current
#endif

/* Canonical scoped entries for the recovered original editor. Native HWND
 * registration, class selection and terminal ownership remain with their
 * existing bindings. These entries do not allocate another window registry. */
LRESULT CALLBACK wow_edit_window_procA(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK wow_edit_window_procW(HWND, UINT, WPARAM, LPARAM);
/* Original client control selection, shared by class binding, explicit
 * creation and task-exit patching. NULL means this group does not own it. */
WNDPROC WINAPI wow_control_client_procedure(WORD fnid, BOOL unicode);
/* Ordinary control timers; only the original list caret callback is admitted.
 * NULL callbacks remain ordinary WM_TIMER delivery, not system timers. */
UINT WINAPI wow_control_set_timer(HWND, UINT, DWORD, WNDPROC);
/* Terminal owner calls while HWND identity is still enrolled. Establishes
 * the same task/borrow permission as normal EDIT entry, then invokes the
 * canonical EDIT/ComboBox/ListBox finalizer selected by initialized FNID. */
BOOL WINAPI wow_control_finalize_bound(HWND);
/* Internal scoped operation, declared without exposing private WND layout. */
BOOL WINAPI wow_control_finalize_window(HWND);
#endif
