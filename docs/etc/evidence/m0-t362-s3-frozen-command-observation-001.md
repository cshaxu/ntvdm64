# M0 T362 S3 — Frozen COMMAND observation and device-owner transfer

## Question

Does the selected original CPU40/x86 `COMMAND.COM /C EXIT` workload reach
`SVC_CMDGETNEXTCMD` (`54:01`) and the original exit terminal after the mapped
first-call COMMAND cohort, or does it reach one earlier source owner?

## Inputs

- Product: `build/M0-T362/S2/x86/original-softpc-process.exe`, SHA-256
  `e2a8d705159738b5900ea197b8d7aed64bbac178102e98d2954eabdcc01730f2`.
- Immutable staged package manifest SHA-256
  `43f09b928f459fbca4c26a6cd6a24f08faf23e74c5ea3a7c291c955828de1cfb`;
  stage manifest SHA-256
  `d58a396332401eedc0e482331f5bb1225290d7e398ba5dcf1370ad84b14abdaa`.
- Fixed short root: `O:\ntvdm64` (10 characters), console-owning, non-debug
  observer; timeout 8,000 ms.
- Original source/map inputs: `softpc.new/base/{ccpu386/ccpusas4.c,
  comms/printer_.c,support/ios.c}` and the formal executable map.

## Procedure

One and only one fresh, hash-verified staged invocation ran through the fixed
console-owning non-debug observer.  The workload, media, product, root,
observer and timeout were not varied.  No production source was changed and
no second observation was made.

## Observation

The copied ingress stream was, in order:

```
50:11, 50:3B, 50:0F, 50:1B, 54:05
```

Neither `54:01` nor `54:00` appeared before the controlled observer timeout.
The product itself did not fault; the observer ended it with `0x53504354`.

The frozen primary thread stack maps as follows:

```
c_IOVirtualised (ccpusas4.c)
  -> inb (ios.c)
     -> printer_io (printer_.c)
        -> CCPU40 execution loop
           -> host_start_cpu
```

The stop instruction is inside the original CCPU I/O-virtualisation path; it
is not in `cmdComSpec`, `cmdGetInitEnvironment`, `cmdGetNextCmd`, or any T362
mapping seam.  `printer_io` is the original BIOS parallel-port status path;
its source loops on the selected status port until the original timeout/status
contract resolves.

## Interpretation and confidence

This is a high-confidence owner transfer, not a causality claim about the
newly mapped COMMAND first-call cohort.  The run proves only that the current
workload reaches the original SoftPC conventional-device I/O path before it
can provide the requested COMMAND lifecycle marker.  It does **not** prove a
printer defect, a `54:05` non-return, a COMMAND result, or a CPU semantic
fault.

The exact predecessor owner is the existing queued **SoftPC
conventional-device and DOS workload matrix** package.  That package must
recover/test the complete source-shaped printer/PIC/PIT/keyboard/device
cohort before COMMAND built-in/batch runtime recovery resumes.  T362 therefore
closes by its declared dependency-first transfer; S4 is not admitted.

## Follow-up

No second T362 observation is permitted.  The next numeric package must be
the re-prioritized conventional-device owner, beginning with its complete
source/binding map and an unchanged frozen workload.
