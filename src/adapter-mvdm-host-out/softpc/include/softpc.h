#ifndef MVDM_REDIRECTOR_SOFTPC_BINDING_H
#define MVDM_REDIRECTOR_SOFTPC_BINDING_H

/* DIVERGENCE(ADAPTER-SOFTPC-REDIR-001): retain the complete original
 * declaration carrier, but replace its x86 process-address coincidence with
 * a bounded synchronous session lease. */
#include "../../../mvdm-host/inc/softpc.h"
#include "mvdm_redirector_pointer_scope.h"

#undef GetVDMAddr
#define GetVDMAddr(usSeg, usOff) \
    mvdm_redirector_pointer_from_real_mode((USHORT)(usSeg), (USHORT)(usOff))
#undef Sim32pGetVDMPointer
#define Sim32pGetVDMPointer(address, mode) \
    mvdm_redirector_pointer_from_source_address((ULONG)(address), (ULONG)(mode))
#undef Sim32GetVDMPointer
#define Sim32GetVDMPointer(address, size, mode) \
    mvdm_redirector_pointer_from_source_address((ULONG)(address), (ULONG)(mode))

#endif
