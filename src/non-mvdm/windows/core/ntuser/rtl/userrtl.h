/*++ BUILD Version: 0001    // Increment this if a change has global effects

/****************************** Module Header ******************************\
* Module Name: userrtl.h
*
* Copyright (c) 1985-91, Microsoft Corporation
*
* Typedefs, defines, and prototypes that are used by the User
* RTL library.
*
* History:
* 04-27-91 DarrinM      Created from PROTO.H, MACRO.H and STRTABLE.H
\***************************************************************************/

/* Original windows/core/ntuser/rtl/userrtl.h subset, OPENNT-HOST-104.
 * DIVERGENCE: retain only the unchanged same-address _USERK_ REBASE
 * definitions; omit unrelated RTL declarations and cross-address forms.
 * Selection describes pointer representation, not kernel execution. */
#ifndef OPENNT_USERRTL_SAME_ADDRESS_SUBSET_H
#define OPENNT_USERRTL_SAME_ADDRESS_SUBSET_H

#define REBASEALWAYS(p, elem) ((p)->elem)
#define REBASEPTR(obj, p) (p)
#define REBASE(p, elem) ((p)->elem)
#define REBASEPWND(p, elem) ((p)->elem)

#endif
