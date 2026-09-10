# M0 T318 S2 P13 — Formal Ninja parallel runner configuration

## Question

Can the formal CPU40 product graph use available host parallelism without
changing its original-source selection, compiler flags, or link topology?

## Change

`tools/build/New-T310OriginalSoftpcNinja.ps1` now emits the formal
`run-ninja-parallel.cmd` runner with:

- one `VsDevCmd.bat` initialization before Ninja starts;
- a default `MVDM_BUILD_JOBS` value equal to `min(12, processor count)`; and
- an environment override: callers may set `MVDM_BUILD_JOBS` before invoking
  the runner.

The runner still invokes the generated `build.ninja` directly.  It does not
enable MSVC `/MP`, alter a translation unit, suppress diagnostics, or change
the selected CPU40 source/build flags.

## Verification

Fresh x86 and x64 `cpu40-v7vga-r4` graphs were generated and each runner
accepted a Ninja dry run for `original-softpc-process.exe`.  The generated
runners both contain `-j %MVDM_BUILD_JOBS%` and default that variable to 12.

## Boundary

This records build scheduling configuration, not a cold-build benchmark.
Actual parallelism remains host-adjustable, and package targets should remain
the normal development target; the full executable is reserved for explicit
integration/closure runs.
