# M0 T355 S14 P2 — source-shaped user-mode FastRead contract recovery

## Source contract

Original NTDOS `handle.asm::$READ` calls `SVC_DEMFASTREAD` (`50:42`) only
after `FastOrSlow` selects the normal-file fast route. Its carry branch is
not an error terminal: it calls original `SVC_DEMREAD` (`50:16`). The missing
NT4 kernel handler `base/ntos/vdm/x86/rdwr.c::NTFastDOSIO` completes a normal
file read when possible and otherwise returns carry for that unchanged DOS
fallback.

## User-mode composition

`demdisp.c` now selects `demFastRead`, a mirror-relative overlay rather than
the general `demRead` provider. It preserves the original registers and:

- accepts only valid mapped regular disk files, excluding the three process
  standard handles plus pipe/device classes;
- honors the original `ZF` seek condition;
- acquires exactly one session-owned guest-memory write lease for the
  requested `DS:DX` linear span, invokes public synchronous `ReadFile`, then
  commits and releases it;
- returns `AX=bytes`, `CF=0` for successful reads, including zero-byte EOF;
- returns only `CF=1` for every decline, mapping refusal, seek failure or read
  failure, without calling `demClientError` or publishing a host error.

Thus the kernel-only failure disposition is recreated at the BOP boundary,
while the original user DEM provider remains the exclusive slow/error owner.
No guest or host native pointer persists beyond the synchronous mapping lease.

## Scope

This fixes the P1 contract error before a new runtime observation is admitted.
The existing S14 run still stopped before `50:42` at the independently-owned
`Sas.Sas_overwrite_memory` null vector; it is neither positive nor negative
evidence for this provider.
