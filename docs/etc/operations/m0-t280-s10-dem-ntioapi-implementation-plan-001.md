# M0 T280 S10 — reached `ntioapi` implementation plan

## Decision

The exact `opennt-platform-abi/.../ntioapi.h` file remains a byte-identical
source carrier.  It is not included wholesale by a modern host build.  The
next admitted implementation packet may add **one** reached-subset facade in
`adapter-win32`; it may not split the eleven entries into DEM-service-local
helpers.

## Required source form

1. Retain the original DEM bodies and their call ordering unchanged.
2. Provide only the reached declarations, constants and layouts named by
   `m0-t280-s10-dem-ntioapi-reached-abi-ledger.tsv`, using the original
   spelling, parameter order, calling convention and `NTSTATUS` result form.
3. Bind `OBJECT_ATTRIBUTES`, `IO_STATUS_BLOCK`, `NtOpenFile`, `NtClose` and
   `NtQueryObject` only after architecture-specific declaration/layout checks
   prove the selected modern surface is source-compatible.
4. Provide the directory, volume, object-name and symbolic-link portion as a
   single adapter-win32 facade.  It may call an available same-shaped `ntdll`
   export only after the import and failure rule are evidenced.  Otherwise it
   returns an explicit source-shaped `NTSTATUS` unavailable outcome; it must
   not silently substitute a lossy Win32 result.
5. Keep query records, Unicode strings, `IO_STATUS_BLOCK`s and native handles
   host-local and synchronous.  Every successful opened handle follows the
   original close path.

## Mapping and ownership

`adapter-win32` owns only host-local NT I/O ABI material.  It neither accepts
nor creates guest identities.  `demsrch.c` may copy a completed result into a
guest DTA only through the separate same-shaped `GetVDMAddr`/`Sim32*` route:
`opennt-mvdm-host -> adapter-softpc -> session.guest_memory ->
adapter-bochs`.  The queried host buffer itself never enters that mapping.

## Verification required for the successor

- x86 and x64 syntax/layout fixtures include the original DEM headers through
  the facade and assert every reached record field used by the three callers;
- focused negative tests cover unavailable directory, volume and symbolic-link
  calls and preserve an explicit `NTSTATUS` branch;
- a focused host-only ownership test proves handles and query buffers do not
  enter any session mapping-manager instance;
- source review proves no DEM body, BOP route or guest trace was enabled as a
  side effect.

## Deferred boundary

This plan does not choose a host directory/volume backend, enable a DEM
provider, or implement guest DTA copy-out.  Those are later owner-package
work.  The plan supplies only the source-shaped NT I/O ABI prerequisite they
will consume.
