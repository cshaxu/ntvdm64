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

/* OPENNT-HOST-104: user-mode retained objects share one address space.
 * Use original same-address formulas (lines 71-74), not NT4 client/server
 * pointer translation. This does not select kernel execution. */
#line 15
#ifndef _USERRTL_
#define _USERRTL_


#line 30
#ifdef _USERK_
#line 31
#error T476-S1: branch not selected by product build
#line 35
#else
#line 36
#line 40
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
#error T476-S1: branch not selected by product build
#endif
#line 49
#ifdef _USERK_
#line 50
#error T476-S1: branch not selected by product build
#else
#line 52
#line 53
#endif
#line 54

#line 68
#ifdef _USERK_
#line 69
#error T476-S1: branch not selected by product build
#line 76
#else
#line 78
#ifndef WOW
#line 79
#if 0
#line 80
#error Excluded original disabled user RTL rebase branch
#line 85
#else
#line 86
#line 71
#define REBASEALWAYS(p, elem) ((p)->elem)
#define REBASEPTR(obj, p) (p)
#line 88
#endif
#line 89
#else
#line 90
#error T476-S1: branch not selected by product build
#line 92
#endif
#line 73
#define REBASE(p, elem) ((p)->elem)
#define REBASEPWND(p, elem) ((p)->elem)
#line 107
#endif /* _USERK_ */
#line 108

#line 118
#ifdef FE_SB
#line 119
#error Excluded unsupported original FE_SB user RTL profile
#line 129
#endif
#line 130

#line 131
#ifdef _USERK_
#line 132
#error T476-S1: branch not selected by product build
#line 148
#else
#line 149
#line 168
#endif
#line 169

#line 177
#ifdef _USERK_
#line 178
#error T476-S1: branch not selected by product build
#line 216
#else
#line 217
#line 242
#endif
#line 243

#endif
#line 245
