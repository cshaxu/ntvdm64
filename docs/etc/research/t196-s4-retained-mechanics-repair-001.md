# M0 T196 S4 Retained-Mechanics Repair 001

## Question

Can the first retained mechanical surfaces be made internally coherent and
selector-blind without adding guest-service, BOP, OpenNT, DOS, SoftPC, device,
firmware, or host-capability semantics to Bochs?

## Inputs

- the active T196 S4 brief in `docs/STATUS.md`;
- the T196 S2 classification and T196 S3 removal record;
- the pinned local Bochs 2.6 comparison tree at
  `O:\repos.external\bochs-2.6-compat\bochs-2.6`;
- `src/bochs/cpu/exception.cc`, `src/bochs/iodev/devices.cc`, and
  `src/bochs/iodev/minimal_port_space.cc`; and
- the retained focused boundary scripts under `tests/`.

## Repair

Three focused test contracts still described superseded source topology:

1. The narrow startup-transaction test required an obsolete
   `prepare_from_environment` call in Bochs.  The current contract is a
   default-off #UD seam that dispatches the typed transaction and takes its
   pending write; preparation remains adapter work and the bridge must not be
   imported by `main.cc`.
2. The default-off #UD test terminated its guarded include scan at the first
   nested preprocessor `#endif`.  It now delimits the full
   `BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT` block and verifies that every
   adapter-runtime/gather header is inside an explicit default-off
   `BX_NTVDM_ENABLE_*` guard.
3. The minimal-mechanics test still sought the `BX-IO-025` block in
   `iodev/devices.cc`, although the initializer was already in
   `iodev/minimal_port_space.cc`.

That third correction exposed an actual incomplete extraction:
`cleanup_empty_port_space` still resided in full `devices.cc`.  S4 moved its
unchanged body into `minimal_port_space.cc`, beside the initializer, default
read/write handlers and default-handler registration helpers.  It is a
Bochs-local source-object repair; it changes neither an ABI, caller,
lifecycle order nor default port behavior.  The `BX-IO-029` register entry and
its source-boundary test now require all six empty-port lifecycle definitions
to be isolated and absent from `devices.cc`.

## Procedure And Observations

Ran all retained mechanical boundary checks, each against the repository root:

```text
Test-BochsAdapterStateTraceBoundary.ps1             passed
Test-BochsDeferredStartupPlanBoundary.ps1           passed
Test-BochsEmptyPortSpaceObjectBoundary.ps1          passed
Test-BochsExecutionPlanBoundary.ps1                 passed
Test-BochsFmtLlLexicalBoundary.ps1                  passed
Test-BochsMachineCompositionBoundary.ps1            passed
Test-BochsOrdinaryRamBoundary.ps1                   passed
Test-BochsStartupTransactionBoundary.ps1            passed
Test-BochsUdDefaultOffBoundary.ps1                  passed
Test-BochsUdInterceptBoundary.ps1                   passed
bx-ntvdm-minimal-machine-boundary.ps1               passed
bx-ntvdm-minimal-mechanics-boundary.ps1             passed
bx-ntvdm-minimal-sim-boundary.ps1                   passed
```

The retired diagnostic scan over `src/bochs`, `tests`, and `tools` for the
eight S3 diagnostic families returned no match.  A zero-context comparison
against the pinned upstream tree reported 25 changed paths and 42 physical
hunks.  The S3 baseline was 43 physical hunks; this is a topology change from
the S4 lifecycle extraction, not a row-for-hunk accounting rule.

No build, link, or guest runtime claim is made here.  Those are S6 work after
S5 reconciles every remaining hunk and register relationship.

## Interpretation And Follow-up

The repaired tests now constrain the current topology rather than preserving
obsolete seam locations.  The port-space lifecycle is one isolated
Bochs-internal object and contains no adapter or OpenNT vocabulary.  This
reduces a concrete unwanted dependency on the full device aggregation object.

S4 remains active.  Its next review must examine the remaining retained hunk
groups for equivalent incomplete extraction or non-minimal build/mechanics
surface before S5 can perform final reconciliation.
