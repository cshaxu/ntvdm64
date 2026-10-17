/****************************** Module Header ******************************\
* Module Name: help.c
*
* Copyright (c) 1985-91, Microsoft Corporation
*
* This module contains the various rectangle manipulation APIs.
*
* History:
* 23-May-95 BradG   Created for Kernel mode
\***************************************************************************/
/* DIVERGENCE(OPENNT-HOST-107): retain only GetContextHelpId; omit unrelated
 * FIsParentDude/EnumPwndDlgChildProc. Standalone host layout/binding includes
 * replace the enclosing original RTL compilation environment. */
#include "wow_user_client_view_layout.h"
#include "wow_window_client_state_bindings.h"
#include "userrtl.h"

/***************************************************************************\
* GetContextHelpId()
*   Given a pwnd, this returns the Help Context Id for that window;
* Note: If a window does not have a Context Id of its own, then it inherits
* the ContextId of it's parent, if it is a child window; else, from its owner,
* it is a owned popup.
\***************************************************************************/

DWORD GetContextHelpId(
    PWND pwnd)
{
    DWORD  dwContextId;

    /* DIVERGENCE(OPENNT-HOST-107): only the private property read changes;
     * use the pinned observation, never re-resolve a possibly reused HWND. */
    while (!(dwContextId = wow_window_client_context_help(pwnd))) {
        pwnd = (TestwndChild(pwnd) ?
                REBASEPWND(pwnd, spwndParent) :
                REBASEPWND(pwnd, spwndOwner));
        if (!pwnd || (GETFNID(pwnd) == FNID_DESKTOP))
            break;
    }

    return dwContextId;
}
