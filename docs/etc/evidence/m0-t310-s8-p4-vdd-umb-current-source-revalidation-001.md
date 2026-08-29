# M0 T310 S8 P4 — VDD/UMB current-source revalidation

## Scope

This record revalidates the 17 historical `C4311`/`C4312` P4 baseline rows
under `SPC-HOST-INPUT-BINDINGS` that name
`mvdm-host/softpc.new/host/src/nt_msscs.c`.  It is limited to x86/x64
pointer-width evidence; it neither enables a VDD product profile nor changes
the original VDD service contract.

## Current source contract

The historical `PVOID` parameters in `VDDInstallMemoryHook`,
`VDDDeInstallMemoryHook`, `VDDAllocMem`, `VDDFreeMem`, `VDDIncludeMem`, and
`VDDExcludeMem` spell guest physical/UMB addresses, not host pointers.
`MVDM-HOST-DIV-063` retains that exported source shape while routing the
numeric carrier through `mvdm_umb_address_decode` before page arithmetic and
`mvdm_umb_address_encode` before original UMB calls.  The resulting `DWORD`
is a bounded guest address; it is not a mapped host identity and therefore
must not consume a session mapping-manager token.

## Revalidation

- Fresh formal Ninja x64 and x86 candidate logs contain no `C4311` or `C4312`
  record for `nt_msscs.c`.
- The call sites preserve the original VDD handle, page-alignment order,
  `ReserveUMB`/`ReleaseUMB`/`VDDCommitUMB`/`VDDDeCommitUMB` direction, and
  original failure returns.
- `m0-t310-s8-p4-whole-tree-warning-x86-x64-classification-worklist.tsv`
  marks baseline records `037010`–`037020`, `037023`–`037024`, and
  `037027`–`037030` as `closed-existing-umb-numeric-address-codec`.

## Conclusion

The earlier records were baseline diagnostics preceding the existing numeric
codec repair.  They are no longer unresolved host-pointer truncations.  Other
host-input and VDD semantics remain governed by their own owner-package rows.
