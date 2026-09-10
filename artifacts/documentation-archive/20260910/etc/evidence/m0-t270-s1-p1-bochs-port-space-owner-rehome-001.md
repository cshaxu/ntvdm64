# M0 T270 S1 P1 — Bochs port-space owner rehome

## Question

Can the finite port-space source currently stranded in `adapter-bochs` be
assigned to the matching private `bochs-core` overlay without changing its
machine-only behavior or exposing an overlay ABI?

## Inputs

- Pinned Bochs 2.6 baseline:
  `O:/repos.external/bochs-2.6-compat/bochs-2.6/iodev/devices.cc`.
- The current component manifest and formal Ninja generator.
- `src/adapter-bochs/minimal_port_space.cc` before the move.
- `src/bochs-core/iodev/{devices.cc,iodev.h}` and the local/central exception
  registers.

## Change and ownership result

- Used `git mv` to move `minimal_port_space.cc` from `adapter-bochs` to
  `bochs-core-overlay/iodev/`.
- The moved source contains the admitted original `bx_devices` lifetime,
  default-handler, registration and dispatch bodies, together with bounded
  empty-port initialization, cleanup and registration-query mechanics. It is
  now compiled only by the `bochs-core` library.
- `adapter-bochs` retains no copy of that Bochs body. Its minimal-machine
  source calls the existing class boundary only.
- Added local `DIVERGENCE:`/README registration for the true-subset device
  source, its private overlay, and previously unmarked component-root include
  redirects in `gui/siminterface.h`.
- Corrected the stale minimal-machine forward declaration in `iodev.h` to the
  actual Bochs-only `adapter_bochs_minimal_machine_c` type and marked all
  retained additions there.

## Verification

- `tests/adapter-bochs/Test-T225S23NativePicBoundary.ps1` passed. It confirms
  the finite native PIC ordering, requires the original port lifecycle
  operations in the new private-overlay source, and rejects semantic leakage
  into the machine assembly.
- A new external formal graph at `build/M0-T270-S1/r001` compiled
  `src_bochs_core_overlay_iodev_minimal_port_space_cc.obj` successfully as a
  `bochs-core` member before any diagnostic.
- Full-graph completion is **not yet a passing result**. The first background
  run reached the Bochs-core archive then recorded an `LNK1104` open failure
  on its disposable output. Later retries were terminated by the execution
  environment mid-build without a compiler diagnostic and left Ninja's
  disposable log in recovery mode. No source change was made to work around
  either infrastructure condition.

## Interpretation and follow-up

The owner correction is source and focused-fixture proven. S1 must still
obtain a clean formal Ninja completion and rerun the focused executable
fixtures before it can close. The remaining core diff/overlay inventory and
external-consumer scan remain in S1 scope.
