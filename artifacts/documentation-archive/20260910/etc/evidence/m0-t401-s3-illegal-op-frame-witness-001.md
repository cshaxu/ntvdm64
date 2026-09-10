# M0 T401 S3 — Illegal-op frame witness

## Scope

This is a diagnostic-only x86 build of the existing source graph. It does not build or alter `WRITE.EXE`, Kernel31, WOW32, firmware, or the normal product. The diagnostic macro is attached solely to `mvdm_softpc_machine_observation.c`; it writes one fixed-path record without consulting an inherited report-path environment variable. The original `illegal_op_int` path still owns the panel and its one-byte continuation.

The short `O:\\t401s3` stage is material: a stage rooted below the build tree is 68 characters long and is rejected before guest startup by the original 63-character NTDOS COMMAND-path limit. The short stage is not a product install location or an acceptance dependency.

## Reproduction

The source-built diagnostic `original-softpc-process.exe`, matching `VDMREDIR.DLL`, and the existing selected `WOW32.DLL` were staged at `O:\\t401s3`. The existing console-owning, non-debug observer launched `O:\\t401s3\\ntvdm32.exe -f -o --command system32\\WRITE.EXE`. No inherited MVDM diagnostic environment variable was set. The observer confirmed the visible `MvdmErrorInteraction` illegal-instruction panel and the diagnostic build wrote its one source-decoded record before the test process was terminated after capture.

## Result

```text
MVDM-CPU-ILLEGAL-OP frame=05D8:20D8 linear=00007E58 bytes=63 69 65 stack=05D8:000005FC resumed-ip=20D9 below=8A01:D801
```

The panel independently reports the same first five bytes: `CS:05d8 IP:20d8 OP:63 69 65 73 5c`.

`05D8:20D8` maps arithmetically to the captured real-mode linear address `00007E58`. The first bytes are ASCII data, not an executable opcode stream. A read-only CCPU selector sampling run observed the same selector earlier at `05D8:1F48`, with code beginning `F3 A4 5F E8 42 00 07 1F C3`; after the panel was active the selector had already been reclaimed, so an after-the-fact LDT descriptor read is not valid evidence. The captured frame words below the interrupt frame, `8A01:D801`, do not constitute an ordinary far-return coordinate.

The diagnostic stage had not loaded WOW32 at this stop. This is therefore a pre-WOW, real-mode startup fault frontier. It does not identify a WRITE source failure, a Kernel31/WOW32 callback failure, or a DPMI32 ABI defect.

## Superseding clean-container comparison

A subsequent same-stage run used the Console-owning observer's explicit
`--observe-without-diagnostics` mode.  It retained no inherited MVDM report
selector and did not activate the illegal-op panel.  Its passive result was:

```text
loaded-wow32=yes
loaded-wow32-path=O:\\t401s3\\WOW32.DLL
top-level-window-00-class=WOWExecClass
top-level-window-00-title=WOWExec
top-level-window-03-class=#32770
top-level-window-03-title=Write
child-window-02-title=Not enough memory for Write to complete this operation.
```

Therefore the earlier `05D8:20D8` record is a diagnostic-mode route, not a
pre-WOW frontier for the unchanged workload.  It must not be used to claim a
COMMAND, DOSX, DPMI32, or WOW32 startup regression.  The applicable frontier
is the source-owned Win16 low-memory dialog reached after WOW32 and WRITE
have loaded; the existing T401 S2 packet owns its state discrimination.
