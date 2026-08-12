# T95 S7 Runtime Trace 007: Post-Boot-Drive Result

## Question

Does the source-built r6 runtime actually consume the newly registered
`DEMGETBOOTDRIVE` contract, and what is the next source owner reached?

## Inputs and procedure

- r6 binary: `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r6/ntdos64-s7-runtime-trace.exe`, SHA-256
  `42CC21674491763EE6CDBEA7D6BF128950EC7FF8A7167FE388F5998622F07156`.
- Immutable source-built NTIO/NTDOS/COMMAND inputs and v2 contained profile.
- One 15-second invocation of `Invoke-T95S7RuntimeTraceObservation.ps1` into
  `artifacts/analysis/t95-s7-runtime-trace-007-20260811-001`.

## Observations

`observation.json` records exit 1 without watchdog termination, 11 BOP
observations and two committed RAM transactions. The log shows `50:0D` at
`8DC8:0667` resuming at `8DC8:066B` with one CPU delta and AL=3. It therefore
crosses the prior boot-drive stop. The next unhandled BOP is `50:21` at
`9346:443C`; `dossvc.h` and `demdisp.c` identify it as `SVC_DEMIOCTL` /
`demIOCTL`. `ioctl.asm` callers show it is DOS device-control mediation, not a
CPU, firmware, FPU, PIC, VGA, or generic file-read requirement.

No `54:04` occurs in trace 007; it remains a tested, source-proven latent
contract, not runtime evidence.

## Interpretation and follow-up

The boot-drive service is source-built runtime evidence. `DEMIOCTL` is a new
owner/caller analysis gate and is not admitted by this result. Determine its
specific AL/BL subfunction and caller failure path before considering a narrow
contained response; do not add device support, host paths, or a generic IOCTL
dispatcher.
