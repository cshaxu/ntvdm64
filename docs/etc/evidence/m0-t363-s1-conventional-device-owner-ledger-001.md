# M0 T363 S1 — SoftPC conventional-device owner ledger

## Scope and method

This ledger resolves the complete immediate ownership cohort for the only
new source-owned transfer in the frozen T362 `/C EXIT` observation:

```
CPU40 c_IOVirtualised
  -> SoftPC IOS inb
  -> original BIOS printer_io
  -> original printer-port table/status path
```

It is a static source and selected-formal-graph review.  It does not reinterpret
the timeout as a printer failure and it does not rerun the frozen T362
container.  The corrected same-run observation remains the authoritative
runtime fact: [T362 S3 correction](m0-t362-s3-frozen-command-observation-correction-002.md).

## Exact source chain

- `softpc.new/base/ccpu386/ccpusas4.c::c_IOVirtualised` owns virtual-8086
  I/O virtualization.  With `getVM()` it invokes the original ROM helper
  `biosDoInst(BIOS_VIRTUALISE_SEGMENT, offset, value, 0, port)`; it is not a
  BOP dispatcher and needs no app or adapter replacement.
- `softpc.new/base/support/ios.c::inb` owns the machine I/O dispatch table.
  Its normal path selects `Ios_inb_function[Ios_in_adapter_table[port]]`.
  The CPU40 selected graph includes this exact translation unit.
- `softpc.new/base/bios/bios.c` associates BIOS entry index `0x17` with
  `printer_io`.  Thus the reached frame is the original INT 17 conventional
  printer service, not a selector-17 BOP service.
- `softpc.new/base/comms/printer_.c::printer_io` selects LPT adapter
  `DX % NUM_PARALLEL_PORTS`, loads the original BIOS Data Area LPT port and
  timeout, then performs the documented `AH=0/1/2` ordinary service through
  `inb`/`outb`.  T362 does not identify the subfunction, so this ledger does
  not claim which of those cases executed.
- The same function's NTVDM `SI=0` flush and `SI=1/2` open/close branches
  return before `setIF()` and before an `inb`.  They therefore cannot explain
  the reached stack and are not an S2 leaf target.

## Initialization and data ownership

- `softpc.new/base/bios/reset.c` calls original `printer_init(adapter)` and
  `printer_post(adapter)` for each LPT adapter.  `printer_init` defines the
  original IOS callbacks and maps LPT1--LPT3 ports; `printer_post` owns the
  original BDA port-address and timeout initialization.
- `printer.c::printer_inb` owns the ordinary device status state machine.
  Its normal `STATE_READY` status read invokes `notbusy_check` and returns the
  original local status bits.  A direct Windows port status read is restricted
  to the `STATE_DONGLE` branch, which T362 did not reach.
- The selected CPU40 graph defines `NTVDM` but not `MONITOR`.  Consequently
  `nt_lpt.c` uses the original in-process `host_lpt[]` status storage, and the
  `MONITOR`-only `printer_setup_table -> MonitorInitializePrinterInfo` route
  is compiled out.  `keybd_io.c::kb_setup_vectors` likewise selects its
  original non-MONITOR branch.  The historical monitor printer calls remain
  source evidence, not unresolved runtime imports in this product profile.
- `IDLE_comlpt()` is the original `HostIdleNoActivity()` notification macro.
  It is a common inactivity signal, not the reached printer data backend.
  ICA/quick-event/timer paths govern later output/interrupt transitions and
  remain part of the same device package; no current evidence makes them an
  independent early terminal.

## Binding disposition

| Cohort edge | Final S1 owner/disposition |
| --- | --- |
| CPU virtual I/O and BIOS entry | Original CPU40/SoftPC code, selected and linked; retain. |
| IOS adapter/function tables | Original SoftPC code, initialized by `reset -> printer_init`; retain. |
| BDA port and timeout | Original `printer_post` and SAS backing; retain. |
| Ordinary LPT status/data state | Original `printer.c` plus original `nt_lpt.c` host state; retain. |
| Printer monitor table/direct-I/O calls | `MONITOR`-conditional source evidence; absent from selected CPU40 profile, not a missing binding. |
| Dongle direct port I/O and host print/flush | Original optional host-capability branches; not reached.  They must preserve their original failure direction if a later workload reaches them, but are not enabled or replaced by a virtual-printer shim here. |
| Timer/ICA/quick-event output transitions | Original device-package dependencies; retain in the formal graph and validate with the cohort, rather than inventing a printer-only substitute. |

## S2 recovery decision

There is no demonstrated missing binding between the frozen runtime transfer
and the ordinary LPT status state machine.  S2 is therefore a bounded
**original conventional-device cohort verification**, not a port patch:

1. prove the selected CPU40 graph contains `reset`, `ios`, `printer`,
   `printer_`, and `nt_lpt` with the exact `NTVDM`/non-`MONITOR` profile;
2. add only a source-shaped local check of original BDA/IOS/LPT status
   initialization and its no-device/timeout direction;
3. if that proof fails, repair the earliest original binding as one cohort;
   do not fabricate a port result, add a virtual printer, or change CPU/BOP
   behavior.

This is sufficient to advance S1.  The one permitted T363 workload observation
remains reserved for S4 after the formal S2/S3 closure.
