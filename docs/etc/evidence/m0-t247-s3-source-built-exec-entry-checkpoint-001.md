# M0 T247 S3 — Source-built EXEC entry checkpoint

## Question

Does the staged original guest actually pass NTDOS `$Exec` PDB construction and
the original `50:36` entry handoff, and if it does not reach an ordinary child
return, what is the first remaining owner without turning the trace into a new
leaf-BOP implementation plan?

## Inputs and procedure

The T247 S2 formal graph was rebuilt as
`build/M0-T247-S2/formal-r1` (MSVC x64 `/MT`, CPU5/P-MMX).  Its native target
ran the source-built `NTIO.SYS`, `NTDOS.SYS`, `COMMAND.COM` and local
source-built `SHARE.EXE` request with the opt-in diagnostic only:

```text
ntdos64-native --dos-root build/output/dos --wow16-root build/output/wow16 \
  --mutation-mode direct --instruction-tick-budget 1000000 \
  --observe-ntdos-exec-entry build/output/dos/share.exe
```

The diagnostic is disabled by default. It observes a completed original
`50:36` result; it does not select a provider, mutate a register or memory
byte, or alter execution.

## Observation

```text
ntdos64-native: ntdos-exec-entry observed=1 read-failed=0
  cs=04ea eip=00005a70 pdb=0bf2 physical=0000bf20
  exit=8e08097e ctrl-c=00a710b8 fatal=8e083348
  parent=8df8 environment=0000
ntdos64-native: terminal=3 detail=6 lifecycle=4 presentation=4
```

This is the exact source order from `msproc.asm:$Exec`: `$Dup_PDB` and the
PDB return-vector writes precede `Xfer_To_User`, which emits `50:36` with DX
equal to the child PDB.  The observation therefore proves original guest PDB
construction and child-entry transfer have occurred for this source-built
guest invocation.

The same bounded run later repeats accepted `50:1E`.  `dossvc.h` names that
selector `SVC_DEMWRITE`; the active imported owner is `demhndl.c:demWrite`.
It has `disposition=Resume` in the prior sequence and is already code complete
with local Direct failure/success coverage.  No dispatch decline, missing
provider, or invented failure result is present.  The eventual terminal is
the existing unexpected-loop execution terminal (`detail=6`), so this run
does not prove `SHARE.EXE` has executed or that `$Exit/$Abort` restored the
parent.

## Owner decision

The first unclosed transition is **post-`50:36` guest execution continuity**,
not `50:1E` implementation.  It belongs to the selector-blind bx-core/
bx-mantle real-mode execution/interrupt/return contract, with the NTDOS guest
remaining owner of `$Exit/$Abort/reset_environment`.  `bx-vdm` must not
repair it by changing `demWrite`, synthesizing a PSP return, or introducing a
trace-specific BOP handler.

The new tracker row `BOP-DEPENDENCY-115` records that cross-owner dependency.
T247 has nevertheless completed the source-defined entry proof; package S4
must now reconcile regression, current tracker state and the explicit
machine-continuity transfer.
