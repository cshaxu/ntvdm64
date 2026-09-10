# M0 T321 S2 — Fixed vertical-slice integration matrix

## Fixed inputs

- Generator: `tools/build/New-T310OriginalSoftpcNinja.ps1`.
- Selected machine: original `CPU_40_STYLE`/CCPU40.
- Build roots: `build/M0-T321/S2/x86` and `build/M0-T321/S2/x64`.
- Product target: `original-softpc-process.exe`.
- Runtime staging: `tools/build/Stage-OriginalSoftpcRuntime.mjs` to
  `build/runtime-t321-s2-cpu40`.
- Observation: unchanged `console-startup-observer.exe`, console-owning,
  non-debug, 8000 ms bound.

## Results

Both independent 419-action Ninja graphs completed a normal process link:

- x86: `build/M0-T321/S2/x86/original-softpc-process.exe` exists.
- x64: `build/M0-T321/S2/x64/original-softpc-process.exe` exists.

The fixed x86 observation recorded:

```text
container=console-owning-nondebug
result=exited
exit=0xc0000005
timeout-ms=8000
```

## Interpretation

This is a reproducible integration result, not proof of DOS program execution
or controlled return.  It neither adds an app/adapter bypass nor changes guest
media, observer mode, BOP behavior or device completion.  Together with T321
S1 and T320, it preserves the exact remaining owner transfer: original CPU40
recursive execution and FDC/ICA device-event scheduling.
