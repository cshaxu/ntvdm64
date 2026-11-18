/*++ BUILD Version: 0001    // Increment this if a change has global effects

Copyright (c) 1989-1993  Microsoft Corporation

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

//
//  Common definitions
//

#include <devioctl.h>
#line 43
#ifdef _X86_
/* The SDK supplies the x86 declarations. */
#endif // x86

#ifdef _MIPS_
#error Unsupported excluded original configuration
#endif // MIPS

#ifdef _ALPHA_
#error Unsupported excluded original configuration
#endif // _ALPHA_

#ifdef _PPC_
#error Unsupported excluded original configuration
#endif // _PPC_

//
//  Each NT Component that exports system call APIs to user programs
//  should have its own include file included here.
//

#line 68
#include <ntpsapi.h>
#pragma push_macro("NTSYSAPI")
#undef NTSYSAPI
#define NTSYSAPI
#include <ntrtl.h>
#include <nturtl.h>
#pragma pop_macro("NTSYSAPI")
#line 77
#include <ntregapi.h>
#include <ntioapi.h>

#line 83
#endif // OPENNT_PRODUCT_NT_H

