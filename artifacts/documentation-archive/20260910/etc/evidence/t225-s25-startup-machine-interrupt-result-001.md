# T225 S25 P2 — Shared Startup-Machine Interrupt Result

## Scope

This focused result implements only the source-mapped common mechanical
protocol for the startup-machine identities `02` and `06`. It is not a
whole-family regression, a BOP-package closure, or a native trace result.

## Delivered route

- `src/opennt/local/compat/host/opennt_int06_provider_v1.*` now exposes a
  direct entry to the original `unexpected_int` common suffix. It does not
  read or modify a saved real-mode frame.
- `src/bx-vdm/bx_ntvdm_startup_machine_interrupt_v1.*` selects `02` or `06`,
  copies each fixed-width provider operation to selector-blind mantle RAM or
  port actions, preflights all writes, and returns checked `RIP + 3` resume.
- `06` retains the original saved-IP/saved-CS reads and CS-before-IP carry
  ordering. `02` begins at the common PIC/BDA suffix.
- The active boot-namespace composition uses this executor before its
  retained package branches. Native composition uses the same executor as
  its fallback. No legacy callback/TLS route was enabled.

## Verification

Fresh disposable root: `build/M0-T225-S25/startup-machine-r1`.

```text
ninja -C build/M0-T225-S25/startup-machine-r1 -j 8 \
  bin/t225-s23-opennt-int06-provider-fixture.exe
ninja -C build/M0-T225-S25/startup-machine-r1 -j 8 \
  bin/t225-s25-startup-machine-interrupt-fixture.exe
build/M0-T225-S25/startup-machine-r1/bin/t225-s23-opennt-int06-provider-fixture.exe
powershell.exe -ExecutionPolicy Bypass -File \
  tests/opennt/Test-T225S23OpenNtInt06ProviderBoundary.ps1
build/M0-T225-S25/startup-machine-r1/bin/t225-s25-startup-machine-interrupt-fixture.exe
```

The provider fixture passed its original software and hardware branches plus
the new direct-common-suffix case. The source boundary test reported
`source invariants verified`. The minimal-machine fixture exited zero after
proving: direct real-mode `C4 C4 02` writes BDA `046B=FF` and resumes at
`RIP+3`; real-mode `C4 C4 06` increments the saved `0070:0479` IP frame,
writes the same BDA byte, and resumes at `RIP+3`; protected-mode `02`
declines.

## Deferred proof

The retained `12`, `15:88`, `17`, `5F`, `66`, and `C8` routes still require
the single whole-family positive/negative fixture. Direct/Readonly native
observation remains prohibited until that regression passes.