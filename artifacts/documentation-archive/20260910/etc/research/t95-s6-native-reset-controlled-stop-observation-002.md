# T95 S6 Native Reset-to-Controlled-Stop Observation 002

## Question

Can the source-built r4 native container enter its original Bochs reset and
simulation lifecycle and stop through the original bounded benchmark timer,
without adapter or guest input?

## Inputs and Procedure

The corrected observer created fresh root
`artifacts/analysis/t95-s6-native-reset-stop-observation-002-20260811-001`.
Its root-local CMD wrapper cleared all `NTDOS64_*` adapter opt-ins and started
only the r4 binary with the copied profile/ROM inputs:

```text
ntdos64-native-container.exe -q -f intact-native-runtime-profile.bochsrc -benchmark 1
```

The run completed in 3,058 ms, below its 30-second watchdog.  The wrapper,
stdout, stderr, exit code and JSON record are retained in that root.  No third
process was launched.

## Observations

- The binary hash is the accepted r4 value
  `F8200EF546C6760816C28E76F0DF90CD93860844ECFE82F59AC7A21D7A3F9AFD`.
- `stdout.log` records `Bochs benchmark mode is ON (~1 millions of ticks)`.
- It records `bx_pc_system_c::Reset(HARDWARE) called`, CPU hardware reset,
  and the original `cpu loop quit, shutting down simulator` marker at
  emulated tick `12,497,239`.
- No host watchdog terminated the process.  The wrapper's exit code is `1`;
  the subsequent `bx_nogui_gui_c::exit() not implemented yet` and
  `quit_sim called with exit code 1` are the native Bochs 2.6 nogui exit path,
  not a benchmark timeout.
- The wrapper clears all adapter environment opt-ins and supplies no BYOB,
  guest disk, OpenNT object, CLI argument, BOP, DEM, DOS or WOW input.

## Observer Classification Defect

The generated JSON initially marks both log predicates false because the
observer inspected only non-stream files.  It also treats the project binary
name `ntdos64-native-container.exe` as a forbidden generic `ntdos` term.
Both are collector predicates, not process facts.  The retained stdout and
wrapper are the controlling evidence above.  The observer was corrected
without another execution to include stdout/stderr in marker inspection and
to reject only concrete adapter/guest boundary terms.

## Minimum-Profile Result

The bounded native lifecycle is proven, but the profile is **not an accepted
minimal machine profile**.  Despite `plugin_ctrl` entries, stdout reports
`PCI support: yes, enabled=yes` and initializes PCI/440FX/PIIX3, CMOS, DMA,
PIC, PIT, floppy, VGA/VBE, ACPI, I/O APIC, keyboard and IDE.  These are
original Bochs behaviors, not adapter leakage, but they exceed the declared
deny-by-default minimum boundary.

## Conclusion and Follow-Up

Classify r4 as **source-built, native reset and bounded controlled-stop
proven; runtime-minimum profile rejected**.  Do not add per-device source
patches or treat the successful stop as DOS/NTVDM execution.  The next task is
a read-only audit of the original Bochs configuration directives and their
device ownership, followed by one coherent declarative minimal-profile design.
