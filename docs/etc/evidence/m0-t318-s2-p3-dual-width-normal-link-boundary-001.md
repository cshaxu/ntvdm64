# M0 T318 S2 P3 — Dual-width formal composition and normal-link boundary

## Scope

This record verifies the selected original `CPU_40_STYLE`/CCPU40 source graph
under both supported host widths.  It is source/build evidence only: neither
row is evidence that a guest reached `NTIO`, `demLoadDos`, NTDOS `EXEC`, or a
parent return.

## Formal build execution

`New-T310OriginalSoftpcNinja.ps1` now emits a disposable
`run-ninja-parallel.cmd` runner for each formal build root.  The runner
initializes the matching MSVC environment once and invokes Ninja with eight
translation-unit jobs.  Rules retain `deps = msvc` and `/showIncludes`; no
warning is disabled and `/MP` is deliberately absent because Ninja owns
parallel scheduling.

- The x86 graph at `build/M0-T318/S2/cpu40-formal-r2` completed the selected
  archive candidate after the current source changes.
- The x64 graph at `build/M0-T318/S2/cpu40-formal-r2-x64` completed all 410
  selected compile/archive actions.
- The same source tree therefore compiles into all selected original package
  archives at both host widths.  This does not select CPU30 or introduce a
  second execution engine.

## Recovered host-width bindings

Three historical RTL spellings referenced by selected original host source
are now supplied once by `adapter-mvdm-host-out/win32`:

- `RtlExtendedLargeIntegerDivide` and `RtlExtendedIntegerMultiply` for the
  original heartbeat arithmetic in `nt_timer.c`;
- `RtlFillMemoryUlong` for original `copy_fnc.c`.

Their original names, parameters and fixed-width return forms are retained.
They operate only on host-local arithmetic or caller-owned byte ranges and do
not expose a guest address as a host pointer.  The adapter registration is
`ADAPTER-WIN32-038`.

The historical CPU40 `retrieve_descr_fields` spelling is likewise now bound
once by `adapter-mvdm-host-out/softpc`. Its selected caller is original BIOS
`tape_io.c`; the bridge delegates descriptor decoding to original CCPU
`read_descriptor_linear` and performs only the original fixed-width
AR/base/limit stores. This is `ADAPTER-SOFTPC-032`, not a CPU30 descriptor
table, a new decoder, or a host-pointer mapping.

The original `VDM_TIB` declaration is exposed to both host-width rows only at
the monitor adapter's include boundary.  That local declaration gate is not
the retired CPU30/V86 route; session TLS remains the sole runtime storage.

## Normal-product link result

`original-softpc-process.exe` deliberately performs a non-`/FORCE` link with
the real app entry and selected original static libraries.  It does **not**
yet link.  The remaining names are exact cross-owner boundaries, not generic
archive omissions:

- `ActivityCheckAfterTimeSlice`: source-unavailable generated CCPU activity
  callback; the original source does not establish a safe no-op, yield or
  timer replacement.
- `ClearInstanceDataMarking`: historical VxD instance-data lifecycle;
  ordinary DOS does not establish its required mapping/cleanup semantics.
- `DBGDispatch`, `ModuleLoad`, `ModuleFree`, `ModuleSegmentMove`, and
  `DbgPrompt`: the private NT4 VDM debugger event product.  Its original
  `dbg.c` package requires the private debugger/CSR transport and is not
  silently selected.
- `ShowStartGlass` and `NtRaiseHardError`: historical WOW/GUI hard-error
  product-shell forms, outside the declared ordinary non-WOW child profile.

After the descriptor bridge, the x86 normal link has nine residuals and the
x64 normal link has seven. The x86 row additionally surfaces `DbgPrompt` and
`NtRaiseHardError`; the x64 row has the same selected source archive closure
and its own normal-link residual set. No empty provider, `/FORCE` executable,
app-owned loader, or substituted BOP was added to turn these boundaries green.

## Consequence

The dual-width archive gate is closed.  The normal-product link and the
ordinary child observation remain open until the named owner packages have a
source-shaped adapter or an explicitly admitted profile disposition.  This
record transfers no debugger, WOW, VDD, generated CCPU callback, or firmware
semantics into T318 S2.
