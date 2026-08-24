/*
 * Direct source mirror: src/opennt/base/mvdm/softpc.new/base/bios/mem_size.c
 *
 * DIVERGENCE(BOP-DIV-023): the historical insignia/host_def/CPU/SAS headers are not a
 * standalone modern CLI composition.  softpc_memory_size_shim.h supplies
 * only their `word`, `MEMORY_VAR`, `sas_loadw`, and `setAX` contract, using
 * the existing selector-blind checked-RAM seam.  The original function body,
 * local data, access order, and application-mutable BDA semantics remain.
 */

#include "../../adapter-softpc/softpc_memory_size_shim.h"

void memory_size()
{
    word memory_size;
    
    /*
     * Return the memory size in AX.  This is read in from the BIOS, as
     * certain applications can write to this area.
     */
    sas_loadw(RUNTIME_SOFTPC_MEMORY_VAR, &memory_size);
    
    setAX(memory_size);
}
