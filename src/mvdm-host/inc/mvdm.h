/*++ BUILD Version: 0001
 *
 *  MVDM v1.0
 *
 *  Copyright (c) 1991, Microsoft Corporation
 *
 *  MVDM.H
 *  Constants, macros, and types common to all of MVDM
 *
 *  History:
 *  Created 14-May-1991 by Jeff Parsons (jeffpar)
--*/


/* Common constants, not defined in NT/WIN32
 */
#define K		1024L


/* Common types, not defined in NT/WIN32
 */
#line 22
#ifdef H2INC
#line 23
#error DIVERGENCE MVDM-HOST-DIV-536: H2INC branch is not selected; see mirror register.
#endif
#line 36

// typedef int INT;
// typedef DWORD UINT;
typedef short FAR *LPSHORT;
typedef char SZ[];
typedef char FAR *LPSZ;
typedef PBYTE *PPBYTE;
typedef PWORD *PPWORD;
typedef PDWORD *PPDWORD;
// typedef UINT *PUINT;


// unaligned typedefs

typedef WORD  UNALIGNED *PWORD16;
typedef SHORT UNALIGNED *PSHORT16;
typedef DWORD UNALIGNED *PDWORD16;

#define WORDOF16(i,n)	(((PWORD16)&(i))[n])
#define LOW16(l)		WORDOF16(l,0)
#define HIW16(l)		WORDOF16(l,1)

/* Common macros, not defined in NT/WIN32
 */
#define BYTEOF(i,n)	(((PBYTE)&(i))[n])
#define LOB(i)		BYTEOF(i,0)
#define HIB(i)		BYTEOF(i,1)
#define WORDOF(i,n)	(((PWORD)&(i))[n])
#define LOW(l)		WORDOF(l,0)
#define HIW(l)		WORDOF(l,1)
#define INTOF(i,n)	(((PINT)&(i))[n])
#define UINTOF(i,n)	(((PUINT)&(i))[n])
#line 68
#ifndef MAKEWORD
#line 69
#error Excluded unsupported missing MAKEWORD SDK profile
#endif
#line 71
#define MAKEDWORD(l0,h0,l1,h1)	((DWORD)MAKEWORD(l0,h0)|((DWORD)MAKEWORD(l1,h1)<<16))
#define GETBYTE(p)	*((PBYTE)p)++
/* DIVERGENCE(MVDM-SUPPORT-DIV-006): recognize MSVC's x86 target macro. */
#ifdef i386
#line 74
#error Excluded unsupported historical i386 SDK profile
#else
#if defined(_M_IX86)
#line 74
#define FETCHWORD(s)	((WORD)(s))
#define FETCHDWORD(s)	((DWORD)(s))
#define STOREWORD(d,s)	(WORD)d=(WORD)s
#define STOREDWORD(d,s) (DWORD)d=(DWORD)s
#else
#line 79
#error Unsupported non-x86 MVDM host access profile
#endif
#endif
#line 84
#define FETCHSHORT(s)	((SHORT)(FETCHWORD(s)))
#define FETCHLONG(s)	((LONG)(FETCHDWORD(s)))
#define STORESHORT(d,s) STOREWORD(d,s)
#define STORELONG(d,s)	STOREDWORD(d,s)

#define NUMEL(a)	((sizeof a)/(sizeof a[0]))
#define OFFSETOF(t,f)	((INT)&(((t *)0)->f))

