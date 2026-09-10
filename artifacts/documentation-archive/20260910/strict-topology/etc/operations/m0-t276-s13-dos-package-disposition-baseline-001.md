# M0 T276 S13: `dos` package disposition baseline

## Scope and evidence

The selected original `dos` root contains exactly 40 paths. Twenty-seven are
literal historical `SOURCES` translation units: sixteen DEM and eleven
COMMAND. The remaining paths are seven declarations, four original build
descriptions, the original `dirs` metadata, and the historical test batch.
The generated per-file ledger records one owner and one non-enabled
composition disposition for every path. The companion interface ledger records
the eight actual external boundary families reached by the retained source.

This is a package-boundary result. It neither re-enables an earlier BOP route
nor treats a trace hit as evidence that a single service may be restored.

## Original package shape

- DEM is one original library, not a collection of independent BOP patches.
  Its service entry/dispatcher and shared state sit with namespace, handle,
  FCB, search, directory, system-state, error, locking, label and raw-media
  providers. The raw DASD/IOCTL pair is explicitly distinct because it reaches
  BIOS-style device/media semantics.
- COMMAND is one original library, not a host `cmd.exe` wrapper. Its entry and
  dispatch, shared state, environment, configuration/bootstrap, console/input,
  PIF, redirection and child lifecycle/return paths remain one source-owned
  control plane.
- The original `sources` and makefiles establish membership only. They add no
  modern compile or link edge. `dos/test/test.bat` remains historical test
  evidence, not an automatically enabled product test.

## Boundary result

- `adapter-bop` is the mechanical BOP ABI boundary. DEM/COMMAND selector and
  service meaning remain in the mirrored original dispatcher sources.
- `adapter-softpc` is the only retained historical SoftPC/SAS/guest-state
  facade. It is the path from the original sources to `adapter-bochs`; no DEM
  or COMMAND source obtains a Bochs object directly.
- `adapter-win32` owns same-shaped public-modern bindings for the historical
  Win32/NTDLL operations. Source algorithms and failure order must remain in
  the original owner package.
- `session` owns per-instance lifecycle, mutable session state and opaque
  identity mappings. It is not a replacement DOS or COMMAND implementation.
- `adapter-redir` and `adapter-vdd` remain specialist boundaries for reached
  redirector/pipe and VDD paths. Neither may grow into a generic host layer.
- `opennt-mvdm-support` remains a prerequisite for OEM/NLS calls; its package
  must be independently dispositioned before binding.
- Raw DASD/IOCTL is machine-media gated: Bochs remains the only machine/device
  owner. Future work may bind a specifically proved public host-media endpoint
  through adapters, but must not restore the historical SoftPC controller.

## Dependency order for later owner packages

1. Preserve the current non-enabled mirrors and their eight boundary contracts.
2. Recover shared support/session/adapter contracts before enabling a package.
3. Recover DEM as a complete provider package: shared state and dispatch,
   namespace/handle/FCB/search/state/error groups, then explicitly profile the
   raw DASD/IOCTL branch.
4. Recover COMMAND as a complete provider package on that base: shared state,
   environment/bootstrap/PIF/input, then lifecycle/redirection/return with the
   required session and Redirector contracts.
5. Only after a package-level local closure may native trace be used as an
   integration observation. It does not decide the next implementation unit.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Audit-T276S13DosDisposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64

powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Audit-T276S13DosInterfaces.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64

powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Verify-T276S13DosDisposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64

powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Verify-T276S13DosInterfaces.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64

powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Verify-T276S13DosCompletion.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64
```

Expected result: 40 package paths, 27 literal source units and eight named
non-enabled external boundary families.
