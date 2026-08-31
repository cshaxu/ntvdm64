# M0 T318 S2 P76 — floppy recursive-simulation owner transfer

Date: 2026-08-31
Status: runtime attribution and owner transfer; not NTDOS `EXEC` continuity

## Question

The source-valid short-root CPU40 product reaches original DEM and COMMAND
bootstrap services, then exits with `0xC0000005`. Does that terminal result
belong to the declared ordinary child/`EXEC` lifecycle, a BOP provider, or an
earlier original machine contract?

## Inputs

- The unchanged fixed `build/runtime-t318-cpu40` media container used by P73.
- Fresh x86 and x64 `original-softpc-process.exe` compile/link rows,
  incrementally rebuilt from the selected CPU40 formal graphs: one adapter
  object, its archive and final link. Only the x86 EXE is observed.
- The fixed console-owning, non-debug observer. It supplies
  `MVDM_EXCEPTION_REPORT_PATH` only to the child and restores its own
  environment immediately after `CreateProcess`.
- The selected original sources `softpc.new/base/disks/floppy.c`,
  `softpc.new/base/keymouse/keybd_io.c`, and `softpc.new/base/ccpu386/c_main.c`.
- The fresh x86 linker map.

## Procedure

1. Keep the verified short-root media and ordinary-child declaration unchanged.
2. Rebuild only the original unhandled-exception adapter so its existing
   diagnostic message is additionally copied to the observer-owned report
   file, but only if an unhandled exception has already reached the original
   filter. This does not alter CPU, guest, BOP, device, return or failure
   state.
3. Run one bounded eight-second observation in the fixed container.
4. Resolve the reported host return address against the exact product map and
   review the original caller plus its recursive CPU/vector contracts.

## Observation

The fixed observation exited rather than timing out:

```text
result=exited
exit=0xc0000005
MVDM-ORIGINAL-EXCEPTION code=0xc0000005 address=0x00000000
ip=0x00000000 return=0x007ffb78 base=0x007b0000
```

It retained the same original bootstrap markers as P73:

```text
MVDM-BOP-DISPATCH 50:11
MVDM-BOP-DISPATCH 50:3B
MVDM-BOP-DISPATCH 50:0F
MVDM-BOP-DISPATCH 50:1B
... 54:05 ...
```

The loaded-image-relative return offset is `0x4fb78`. The current linker map
places it in `original-softpc-disks:floppy.obj`, inside the original
`wait_int` body (`0044fb10` at preferred image base). That body sets the
original NTVDM `int15_seg:int15_off` and later `wait_int_seg:wait_int_off`,
then enters the original recursive `host_simulate()` CCPU40 path. CCPU40's
`c_cpu_simulate` retains the original nested `setjmp` frame and execution
body. The vector locations are original SoftPC guest-vector state, initially
declared in `keybd_io.c` and subsequently sourced from the NTIO keyboard/ROM
table during original initialization.

## Interpretation

The access violation is now attributable to an attempted execution transfer
through the original SoftPC floppy/FDC wait and recursive CPU/vector contract;
the exception address is zero. The report's return address proves the active
host caller is `wait_int`, but does not by itself distinguish a zero guest
vector, incomplete ROM/keyboard-table initialization, an FDC/PIC interrupt
completion defect, or a deeper CCPU instruction-path defect. It does prove:

- the failure is later than the original `50:11`, `50:3B`, `50:0F`, `50:1B`
  and `54:05` bootstrap observations;
- it is not caused by the declared `/C VER` child declaration, because P73's
  no-child control has the same terminal result;
- it is not evidence to modify DEM, COMMAND, XMS/HIMEM, CONFIG or BOP
  routing; and
- it occurs before the current task can validly claim original NTDOS `EXEC`,
  child exit, parent restore or `54:0B` continuity.

## Disposition

`M0 T318 S2` excludes device expansion. Transfer the blocker as one original
owner package to queue candidate **Minimal vertical-slice integration
closure**:

```text
SoftPC BIOS/reset -> FDC/PIC -> int15/wait guest-vector table
  -> recursive CPU40 host_simulate -> source-shaped return/interrupt result
```

That package must recover or explicitly source-shape the selected FDC/INT15
machine profile as a whole. It must not invent an app loader, patch a BOP
service, bypass the guest device call, or convert the zero transfer into a
synthetic successful wait. After its closure, a new T318 admission may resume
the frozen NTDOS `EXEC`/parent-return objective.

## Confidence and follow-up

High confidence that the active host caller is the original floppy recursive
wait path; medium confidence on the exact nested zero target until the device
owner package performs its own source-shaped vector/interrupt audit. The
exception-only report is retained as a narrowly registered diagnostic. No
second observation or source behavior repair is admitted by this record.
