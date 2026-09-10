# T107 S1 original VdmRedir-unavailable provider 001

## Contract and implementation

OpenNT `nt_bop.c` `MS_bop_2` records the terminal branch after VdmRedir load
has failed: it does not retry loading, sets CF, places
`ERROR_INVALID_FUNCTION` (one) in AX, and advances the BOP continuation.
The contained CLI profile selects that terminal historical disposition rather
than attempting the intrusive VdmRedir load.

The adapter now classifies only `C4 C4 02` as explicit unavailable and routes
it through the existing ingress and provider registry to
`bx_ntvdm_redir_unavailable_provider_v1`.  That provider accepts only the
copied real-mode #UD record and emits `CF=1`, `AX=1`, `RIP+3`.  It has no
guest-memory access, redirector backend, host path, device or Bochs semantic.

The focused MinGW C11 target passed.  A fresh MSVC/x86 derivative built with
binary SHA-256 `FB748E860670ECA325620D2158238E172044E870DC338874C6737197F2869AF7`.
Its single standard deferred 1M trace reached the native POST/NTIO/NTDOS path
without a regression, but ended at the benchmark boundary before selector
`02` was reached.  This is build and non-regression evidence, not runtime
proof of the result branch.

## Follow-up

T108 is limited to one longer unchanged-binary observation.  It must prove
the `02` result in the actual trace or record that the current workload takes
another boundary first.  It may not modify the provider or add a redirector.

