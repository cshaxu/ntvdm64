/*++ BUILD Version: 0001    // Increment this if a change has global effects

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    nt.h

Abstract:

    Top level include file for applications and subsystems written to
    the NT API.  Including this file defines all the public types
    and system calls that an application program can use.

    Initial clients of this include file are the NT Shell, along with
    the Windows, OS2 and POSIX subsystems.


Author:

    Steve Wood   [stevewo]   23-Feb-1989

Revision History:

--*/

#ifndef OPENNT_PRODUCT_NT_H
#define OPENNT_PRODUCT_NT_H
#include <opennt_support_types.h>
#include <devioctl.h>
#line 29

#if defined (_MSC_VER)
#if ( _MSC_VER >= 800 )
#pragma warning(disable:4514)
#ifndef __cplusplus
#pragma warning(disable:4116)       // TYPE_ALIGNMENT generates this - move it
                                    // outside the warning push/pop scope.
#endif
#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning(disable:4001)
#pragma warning(disable:4201)
#pragma warning(disable:4214)
#endif
#if (_MSC_VER > 1020)
#pragma once
#endif
#endif
//
//  Common definitions
//

#define _CTYPE_DISABLE_MACROS

#include <excpt.h>
#include <stdarg.h>
#include <ntdef.h>

#include <ntstatus.h>
#line 60

#if defined(_AMD64_)
#line 63

#elif defined(_X86_)
#line 66

#elif defined(_IA64_)
#error Unsupported excluded original configuration

#else
#error "no target defined"
#endif // _AMD64_
#line 73

//
//  Each NT Component that exports system call APIs to user programs
//  should have its own include file included here.
//

#line 83
#include <ntpsapi.h>
#line 85
#include <ntlpcapi.h>
#include <ntioapi.h>
#include <ntiolog.h>
#line 89
#include <ntexapi.h>
#line 91
#include <ntregapi.h>
#line 94
#include <ntnls.h>
#include <ntpnpapi.h>

#include "mce.h"
#pragma push_macro("NTSYSAPI")
#undef NTSYSAPI
#define NTSYSAPI
#include <ntrtl.h>
#include <nturtl.h>
#pragma pop_macro("NTSYSAPI")
#line 98

#if defined(_AMD64_)
#line 101

#elif defined(_X86_)
#include "nxi386.h"

#elif defined(_IA64_)
#error Unsupported excluded original configuration

#else
#error "no target defined"
#endif // _AMD64_
#line 111

#if defined (_MSC_VER)
#if _MSC_VER >= 1200
#pragma warning(pop)
#else
#pragma warning(default:4001)
#pragma warning(default:4201)
#pragma warning(default:4214)
#endif
#endif

#endif // OPENNT_PRODUCT_NT_H
