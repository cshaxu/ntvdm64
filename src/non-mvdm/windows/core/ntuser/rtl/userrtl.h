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
#ifndef _USERRTL_
#define _USERRTL_

#define REBASEALWAYS(p, elem) ((p)->elem)
#define REBASEPTR(obj, p) (p)
#define REBASE(p, elem) ((p)->elem)
#define REBASEPWND(p, elem) ((p)->elem)

#line 30
#ifdef _USERK_
#line 31
#else
#line 36
#endif
#line 41

#line 42
#if DBG
#line 43
#error Excluded unsupported original DBG user RTL profile
#endif
#line 45

#line 46
#ifdef RIP_COMPONENT
#line 47
#endif
#line 49
#ifdef _USERK_
#line 50
#else
#line 52
#endif
#line 54

#line 68
#ifdef _USERK_
#line 69
#else
#line 78
#ifndef WOW
#line 79
#if 0
#line 80
#error Excluded original disabled user RTL rebase branch
#else
#line 86
#endif
#line 89
#else
#line 90
#endif
#line 107
#endif /* _USERK_ */
#line 108

#line 118
#ifdef FE_SB
#line 119
#error Excluded unsupported original FE_SB user RTL profile
#endif
#line 130

#line 131
#ifdef _USERK_
#line 132
#else
#line 149
#endif
#line 169

#line 177
#ifdef _USERK_
#line 178
#else
#line 217
#endif
#line 243

#endif
#line 245
