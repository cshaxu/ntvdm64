/*++

Copyright (c) 1991  Microsoft Corporation

Module Name:

    vrdebug.c

Abstract:

    Contains diagnostic/debugging routines for Vdm Redir:

        VrDebugInit
        VrDiagnosticEntryPoint
        VrPrint
        VrDumpRealMode16BitRegisters
        VrDumpDosMemory
        (ConvertFlagsToString)
        (GetFlags)
        (GrabDosData)
        probe_parens
        CategoryToIndex
        GetRoutineDiagnosticInfo
        MergeInfo
        DoEntryDiagnostics
        GetRegisterOrValue
        VrDumpDosMemoryStructure
        VrPauseBreak
        VrErrorBreak

Author:

    Richard L Firth (rfirth) 13-Feb-1992

Notes:

    This module allows us to run NTVDM.EXE and breakpoint or dump info based
    on an environment variable. Set the __VRDEBUG environment variable depending
    on the level and type of diagnostics required from VdmRedir functions. See
    vrdebug.h for more info

    This is a convenient method of dynamically changing diagnostic/debug
    information at run time without having to poke around in memory or start
    up NTVDM under NTSD (especially since NTVDM is run by the loader and not
    by typing NTVDM<return> at the command prompt. Changing diagnostic settings
    requires that NTVDM be shut down, redefine the __VRDEBUG environment
    variable(s) and start a dos app. Alternatively, start the dos app in a new
    command session after setting the new __VRDEBUG environment variable(s)

    Types of things which can be affected by values in environment string:

        - Whether a routine breakpoints at entry (and goes into debugger)
        - What functions or categories to diagnose
        - What to dump at function entry:
            x86 registers
            DOS memory
            DOS stack
            DOS structures

    Note that currently the order of things at function entry are predefined:

        1. Display function's name
        2. Display x86 registers
        3. Dump DOS stack
        4. Dump DOS memory
        5. Dump DOS structure(s)    Currently only 1 structure
        6. Break

    Diagnostic information is set up once at NTVDM initialisation for this
    session, so the environment information must have been already entered.
    There can be 11 __VRDEBUG environment variables: __VRDEBUG and __VRDEBUG0
    through __VRDEBUG9. Each string can contain several function/group
    specifications of the form:

        <function or group name>[(<diagnostic options>)]

    The group names are predefined (add a new one if a new group of functions
    is added to VdmRedir) and are currently:

        MAILSLOT, NAMEPIPE, LANMAN, NETBIOS and DLC

    Group names are case insensitive. Function names are case sensitive and
    should be entered as they appear in the C code. However, if a case sensitive
    search for a function name fails, a secondary case-insensitive search will
    be made.

    Diagnostic options are case insensitive and are performed in the order shown
    above and are the following:

        BREAK[()]
            Break into the debugger (DbgBreakPoint) when the function is
            entered

        DISPLAYNAME[()]
            Display the function name on entry

        DUMPREGS[()]
            Dump the x86 16-bit registers on entry to the function

        DUMPSTACK[(<number>)]
            Dump <number> of DOS stack words (or use DEFAULT_DUMP_STACK_LENGTH)
            on entry to the function. This is just a special case of DUMPMEM,
            with type set to 'W', and segment:offset set to ss:sp.

        DUMPMEM[(<segment>, <offset>, <number>, <type>)]
            Dump <number> and <type> of DOS memory (or use DEFAULT_DUMP_MEMORY_LENGTH
            and DEFAULT_DUMP_MEMORY_TYPE) at <segment>:<offset>

        DUMPSTRUCT[(<seg> <off> <descriptor>)]
            Dump the structure addressed by <seg>:<off> (not necessarily segment
            and offset registers, can be eg. ax:flags) and descriped by <descriptor>

        INFO[()]
            Filter for calls to VrPrint - display INFORMATION, WARNING and ERROR
            messages

        WARN[()]
            Filter for calls to VrPrint - display WARNING and ERROR messages

        ERROR[()]
            Filter for calls to VrPrint - display ERROR messages only

        ERRORBREAK[()]
            Cause a break into debugger whenever BREAK_ON_ERROR/VrErrorBreak
            called

        PAUSEBREAK[()]
            Cause a break into debuggger whenever BREAK_ON_PAUSE/VrPauseBreak
            called

    If an option appears without parentheses then its function is turned OFF
    for this group or function.

    Whitespace between an option and following parentheses is tolerated, but
    should not be present.

    If an option takes parameters then all required parameters must be present
    or the option is ignored.

    Where an option takes parameters, commas are tolerated between parameters
    but are not necessary. Eg

        dumpmem(ds si 32 B)

    and

        DUMPMEM ( ds, si, 32, b )

    and

        DumpMem (ds,si,32,B)

    are treated as the same (, is just whitespace in this u-grammar).

    Where register parameters are required, absolute hex values can be given.
    Similarly, where a number parameter is required, a 16-bit register (any
    register) definition can be substituted.

    Function specifications take precedence over group specifications. So if the
    same option is given for a group and a function that belongs to that group
    then the options specified in the function entry are used. Any options
    specified for the group but not negated by the function are used for the
    function also.

    When parsing the environment, if the syntax is violated, the current
    description is skipped until the next description starts or the end of
    the string is found. Eg:

        mailslot(break() DLC(break()) DLC(dumpstruct(bx BBBBPPBBBB))
                        ^                               ^
                        1                               2

    In this example, only DLC(break()) is parsed correctly, because there is
    a missing right parenthesis at 1 and a missing offset register specification
    at 2 (assumes bx is segment register, because that's the syntax)

Revision History:

--*/

#if DBG
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#ifdef VR_DEBUG_FLAGS
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#ifdef DBGDBG
#error Excluded original conditional branch
#endif
#ifndef DBGDBG
#error Excluded original conditional branch
#endif
#if DBG
#error Excluded original conditional branch
#endif
#ifdef DBGDBG
#error Excluded original conditional branch
#endif
#ifdef DBGDBG
#error Excluded original conditional branch
#endif
#ifdef DBGDBG
#error Excluded original conditional branch
#endif
#ifdef DBGDBG
#error Excluded original conditional branch
#endif
#ifdef DBGDBG
#error Excluded original conditional branch
#endif
#ifdef DBGDBG
#error Excluded original conditional branch
#endif
#ifdef DBGDBG
#error Excluded original conditional branch
#endif
#ifdef DBGDBG
#error Excluded original conditional branch
#endif
#ifdef DBGDBG
#error Excluded original conditional branch
#endif
#ifdef DBGDBG
#error Excluded original conditional branch
#endif
#ifdef DBGDBG
#error Excluded original conditional branch
#endif
#ifdef DBGDBG
#error Excluded original conditional branch
#endif
#ifdef DBGDBG
#error Excluded original conditional branch
#endif
#ifdef DBGDBG
#error Excluded original conditional branch
#endif
#ifdef DBGDBG
#error Excluded original conditional branch
#endif
#ifdef DBGDBG
#error Excluded original conditional branch
#endif
#ifdef DBGDBG
#error Excluded original conditional branch
#endif
#ifdef DBGDBG
#error Excluded original conditional branch
#endif
#ifdef DBGDBG
#error Excluded original conditional branch
#endif
#ifdef DBGDBG
#error Excluded original conditional branch
#endif
#ifdef DBGDBG
#error Excluded original conditional branch
#endif
#ifdef DBGDBG
#error Excluded original conditional branch
#endif
#ifndef DBGDBG
#error Excluded original conditional branch
#endif
#endif // DBG
#line 184
