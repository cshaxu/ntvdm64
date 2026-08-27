# M0 T280 S18 — `demsrch.c` host-local x64 binding evidence

## Applied recovery

- `FINDFILE_DEVICE` now expresses the original all-bits-one host HANDLE
  sentinel at native width. It is private host state, not a surrogate and not
  a guest value.
- The original pathname/device-name and directory-buffer algorithms retain
  their byte-offset, alignment, query and failure order, but perform their
  host-local arithmetic with byte pointers or `uintptr_t` rather than a
  truncating `ULONG` pointer cast.
- The reached `vdm.h` and `NtVdmControl` implementation moved unchanged in
  function shape from `adapter-win32` to its architectural owner,
  `adapter-vdm-monitor`. Its only enabled service remains session-bound
  `VdmQueryDir`; the rest deterministically returns `STATUS_NOT_IMPLEMENTED`.

The original `GetVDMAddr` call sites did not change. They require their own
bounded `adapter-softpc` guest-memory lease and remain excluded here. No
host-local handle, buffer, path pointer or `VDMQUERYDIRINFO` field enters a
mapping-manager instance.

## Formal verification

With MSVC BuildTools, `/MT /W4` and disposable Ninja roots:

```text
x64/x86: New-T280S12DemObjectObservationNinja.ps1; ninja demsrch
x64/x86: New-T280VdmControlNinja.ps1; ninja test
x64/x86: New-T280S11NtioapiFacadeNinja.ps1; ninja; fixture.exe
```

Both original-source syntax runs completed without C4311 or C4312. The
remaining identical x86/x64 diagnostics are the S17-classified original
unused/shadowed and CRT-hygiene warnings. Both VdmQueryDir fixtures print
`PASS`, and both NT I/O facade fixtures compile, link and exit zero.

## Limit

This is source and adapter binding closure only. It does not link or enable a
DEM directory provider, expose a guest DTA/FCB pointer, choose a filesystem
backend, or run a BOP/guest trace.
