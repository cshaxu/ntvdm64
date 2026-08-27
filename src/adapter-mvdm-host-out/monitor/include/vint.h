/*
 * Reached source-shaped subset of the historical VINT header.  The original
 * fixed-low-address pNtVDMState macro is intentionally not reproduced: the
 * same spelling is supplied by vdm.h as session-worker storage.
 */
#ifndef ADAPTER_MVDM_MONITOR_VINT_H
#define ADAPTER_MVDM_MONITOR_VINT_H

#include "vdm.h"

#define INTERRUPT_PENDING_BIT 0x0003
#define VIRTUAL_INTERRUPT_BIT 0x0200
#define MIPS_BIT_MASK 0x0400
#define EXEC_BIT_MASK 0x0800
#define RM_BIT_MASK 0x1000
#define RI_BIT_MASK 0x2000

#endif
