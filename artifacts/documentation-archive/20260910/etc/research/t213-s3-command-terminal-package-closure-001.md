# T213 S3: COMMAND Terminal Package Closure

Packet: M0 T213 S3, Ordinary Mode  
Date: 2026-08-15  
Status: source-built package/engine closure

## Delivered boundary

The complete existing COMMAND lifecycle provider (`54:00/03/0B`) now owns a
versioned copied terminal fact.  An exact accepted `54:00 CMDEXITVDM` creates
`TOP_LEVEL_EXIT` with an explicit absent DOS exit code and returns its
pre-existing typed stop.  `54:03` retains its resume contract and `54:0B`
retains its final-return record/re-entry contract.

The boot composition exports only a boolean opaque ordinary-terminal fact;
the composition-runtime API exposes the same opaque fact.  Engine code reads
it only after machine-stage reset, then performs its existing composition and
controller cleanup, and only then maps a controlled stop to the existing
`ORDINARY_GUEST_COMPLETION` result.  A controlled stop without that fact,
including DEM `50:3D`, remains `CONTROLLED_GUEST_TERMINAL`.

No Bochs/mantle source identifies BOP, COMMAND, DEM, or a guest address.

## Verification

1. The full existing COMMAND package source closure compiled, linked and ran
   under MSVC x64 `/MT` at
   `artifacts/build/t213-s3-command-lifecycle-r1/`: its JSON records
   `linkExitCode=0`, `runExitCode=0`, `passed=true`.  It covers exact `54:00`
   record creation plus `54:03/0B` regression.
2. The engine fixture at
   `artifacts/build/t213-s3-engine-terminal-r2/` passed under MSVC x64 `/MT`.
   It proves cancellation/budget preservation, controlled stop without an
   opaque record, and ordinary completion only with an opaque record after
   the engine's stage reset.
3. The actual composition-runtime source compiled independently under MSVC
   x64 `/MT` at
   `artifacts/build/t213-s3-composition-runtime-compile-r2/`.  The earlier
   r1 failure lacked an include directory and is retained negative build
   evidence; r2 used the complete CLI/bx-vdm header closure.

## Limitations

The current boot profile still reaches no `54:00`; this package does not
claim a guest normal run.  DEM `50:3D` remains controlled.  S4 may perform
one passive native integration observation; it may not add another provider
because a trace does not select implementation work.
