# M0 T288 S5 keyboard IVT and lifecycle evidence

## Recovered source contract

The exact firmware mirror keeps `softpc.new/base/keymouse/keybd_io.c`. Its
`UpdateKbdInt15(word Seg, word Off)` body is a small INT 15h callout-cache
contract: read IVT words `15h*4` and `15h*4+2`; if either differs from the
previously stored handler, clear both stored words and return; otherwise retain
the caller's supplied `Off` then `Seg` as the new cached vector.

The full translation unit is not a safe host runtime input because it also owns
the historical keyboard controller, timer, PIC, IRQ, BIOS function table and
`host_simulate` re-entry shell. The source-shaped extraction in
`adapter-mvdm-host-out/softpc/mvdm_int15.c` preserves only the reachable
two-word IVT contract. It reads through existing same-shaped `sas_loadw` and
has no direct Bochs dependency.

`mvdm_int15_capture_vector` corresponds to the original `keyboard_init`
capture of the initialized IVT vector. `mvdm_int15_reset_vector` supports
session/machine teardown. `mvdm_int15_matches_current_vector` is a narrow
composition predicate for the original later `INT15` decision; it neither
dispatches an interrupt nor owns keyboard policy.

## Explicit lifecycle boundary

The original subsequent `INT15` path requires BIOS function table dispatch,
AH/CS/IP state, `host_simulate`, keyboard hardware ownership, timer behavior
and PIC/IRQ handling. It cannot be claimed by a callout-cache implementation.
The complete source/provenance transfer is recorded in
`m0-t288-s5-keyboard-ivt-lifecycle-ledger.tsv`; no generic keyboard or IRQ
shell, second CPU executor, raw pointer or new mapping manager is added.

## Reproducible verification

For x86 and x64, run outside the sandbox:

```powershell
powershell.exe -ExecutionPolicy Bypass -File .\tools\build\New-T288S5Int15Ninja.ps1 -Architecture x86 -RepositoryRoot (Get-Location).Path
ninja -C build\M0-T288\S5\x86
build\M0-T288\S5\x86\bin\int15-vector-fixture.exe

powershell.exe -ExecutionPolicy Bypass -File .\tools\build\New-T288S5Int15Ninja.ps1 -Architecture x64 -RepositoryRoot (Get-Location).Path
ninja -C build\M0-T288\S5\x64
build\M0-T288\S5\x64\bin\int15-vector-fixture.exe
```

Both fixtures prove capture, an unchanged-vector update, hook invalidation,
reset to the zero vector and subsequent recovery. They use only an in-memory
typed machine-facade double; no selector, guest execution, controller or IRQ
route is enabled.
