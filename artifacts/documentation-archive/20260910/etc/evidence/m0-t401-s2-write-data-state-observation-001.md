# M0 T401 S2 — WRITE same-run data-state observation

## Question

Does the selected original `WRITE.EXE` reach either of the two source-owned
paths that set its private `fMessageInzFailed` flag before the observed
terminal failure?

## Inputs and identity

- Product: `O:\t398\ntvdm32.exe`, SHA-256
  `f1c3f81d793198d32ed685429e4509547a581fc0b3f87240b97296022f0a66a0`.
  The stage manifest identifies its matching map as
  `build/M0-T398/S2/error-interaction-x86/original-softpc-process.exe.map`.
- Workload: `O:\t398\system32\WRITE.EXE`, SHA-256
  `08ee1659788880fb593815ff9609bfe76743c1182d05e6702764bf3dff68b30b`, equal
  to the selected immutable source media
  `src/mvdm-guest/wow16/write/WRITE.EXE`.
- The selected `write16.map` identifies `MmwCreate` as segment 2 offset
  `0x05b0`.  The immutable executable encodes `C7 06 58 01 01 00` at segment
  2 offset `0x0772`: the source store `fMessageInzFailed = TRUE` at DGROUP
  offset `0x0158`.
- `initwin.c` has exactly two source stores to the flag: the `MmwCreate`
  `Error` path at line 793 and `MdocCreate`'s null-DC branch at line 807.

## Procedure

Only the test-only Console observer was compiled.  It reads the original
CPU40 LDT/SAS state with `ReadProcessMemory`; it neither writes the target nor
injects, traces instructions, creates guest input, changes guest media, or
rebuilds WRITE.  Before each read it identifies the loaded non-code WRITE
DGROUP by the immutable 20-byte data signature at selected NE segment 84,
offset `0x00ec`:

```
00 46 49 4c 45 3a 00 2e 44 52 56 00 25 73 25 73 00 45 78 74
```

The valid run used the same formal WOW launch spelling as the pre-existing
stage evidence, but deliberately suppressed every optional product diagnostic
environment variable:

```
console-startup-observer.exe O:\t398\ntvdm32.exe O:\t398 \
  build\M0-T401\S2\write-data-state-run-003-clean \
  --observe-without-diagnostics --observe-write-capture \
  --observe-cpu40-live-map build\M0-T398\S2\error-interaction-x86\original-softpc-process.exe.map \
  --observe-cpu40-ne-data-state src\mvdm-guest\wow16\write\WRITE.EXE 84 0xEC 0x158 \
  --observation-timeout-ms 30000 -f -o --command system32\WRITE.EXE
```

The preceding reporter-enabled trial did not load WOW before its timeout; it
is retained as a rejected observer-configuration result, not product
evidence.  A bare `WRITE.EXE` spelling was also rejected because app correctly
sent the unresolved command to host `cmd`, before any VDM/WOW load.

## Observation

The clean run loaded `WOW32.DLL`, saw the WOWExec shell, `MSWRITE_MENU`, and
one visible `Application Error` dialog.  Its original Kernel31 text says
`WRITE caused a General Protection Fault`; it is not WRITE's low-memory
dialog.

The observer made 23 candidate data-state samples, found exactly one matching
data descriptor, and read:

| Field | Value |
| --- | --- |
| selector | `0x1514` |
| LDT base | `0x00126800` |
| data base | `0x00045fa0` |
| limit / access | `0x000095ff` / `0xf3` |
| `fMessageInzFailed` at `DS:0x0158` | `0x0000` |

The selector is reported only as same-run descriptor evidence.  It is not
used as a cross-run module identity or an instruction-location claim.

## Interpretation and confidence

High confidence: the observer can bind the actually loaded WRITE DGROUP once,
without a broad guest-memory scan, and read the intended static word.

High confidence: the observed GPF terminal does **not reach the normal
error-exit stores** in either `MmwCreate:Error` or `MdocCreate`'s null-DC
branch. The source initializes the static word to zero and has no reset store;
therefore its observed zero value excludes both of those true-setting paths
for this execution.

This result does not identify the faulting instruction, its module, a CPU or
DPMI defect, or a repair. In particular, it does **not** exclude a fault
*inside* `FInitMemory` or elsewhere inside `MmwCreate` before either routine
can reach its normal error-exit store. It also does not erase the separately
observed low-memory-dialog outcome: the two executions have distinct terminal
dialogs. The next source-first question is the original Kernel31
protected-mode GPF handoff and the closest already-composed non-debug
pre-fault boundary; it is not authority for a WRITE rebuild or a private
replacement.

## Follow-up

Audit the original Kernel31 `GPFault` handoff and existing debugger/VDM event
bridge for a stable fault-frame result contract.  Any implementation beyond
that audit requires a new, source-recovery-qualified S admission.
