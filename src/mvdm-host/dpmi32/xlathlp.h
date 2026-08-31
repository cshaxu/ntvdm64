/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    xlathlp.h

Abstract:

    This module contains helper macros for the api translation

Author:

    Dave Hastings (daveh) 24-Nov-1992

Revision History:

--*/

#define DPMI_EXEC_INT(x)    DpmiSwitchToDosxStack(FALSE);   \
                            DpmiPushRmInt(x);               \
                            host_simulate();                \
                            DpmiSwitchFromDosxStack();

//
// VOID
// DPMI_FLAT_TO_SEGMENTED(
//     PUCHAR buffer,
//     PUSHORT seg,
//     PUSHORT off
// )

#define DPMI_FLAT_TO_SEGMENTED(buffer, seg, off) {  \
    /* DIVERGENCE(MVDM-HOST-DIV-140): retain host-pointer width until the \
       original result is intentionally narrowed into 16:16 guest form. */ \
    *seg = (USHORT)(((ULONG_PTR)buffer-IntelBase) >> 4);        \
    *off = (USHORT)(((ULONG_PTR)buffer-IntelBase) & 0xf);       \
}


#define SELECTOR_TO_INTEL_LINEAR_ADDRESS(sel) \
    (FlatAddress[(sel & ~7) / sizeof(LDT_ENTRY)] - IntelBase)
