# T106 S1 two-phase BOP action-bridge audit 001

## Result

The common action bridge is connected; it is not the cause of T105.  With
`BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT=1`, `exception.cc` installs the generic
adapter interceptor.  Its v4 path obtains a typed gather action, takes the
matching pending session record, verifies ordinary RAM ranges, copies them,
completes the provider action, commits any typed write, applies copied CPU
deltas and resumes.  Its v3 scalar-read fallback follows the same ownership
pattern.  T105 itself records a v4 `gather copied ranges=1 bytes=4` followed
by an accepted resume.

Current v4 producers are DEM path search, CMD get-next, CMD COMSPEC bootstrap,
mouse mapping and profile snapshots; v3 producers include DEM open and DTA.
Immediate and multi-write results use the same interceptor after provider
selection.  The disabled legacy CPU-result-bridge macro is therefore not a
missing dependency for current adapter v4 execution.

The repeated fault frame begins `C4 C4 02`.  In original `nt_bop.c`,
`MS_bop_2` is the explicit VdmRedir-load-failed fallback: it advances the BOP
continuation and returns `CF=1`, `AX=ERROR_INVALID_FUNCTION`.  This is an
original host-service failure disposition, not a machine, COMMAND, gather or
INT6 service.  T107 must add it through the common ingress/provider/result
route, with no VdmRedir loading or host redirector capability.

