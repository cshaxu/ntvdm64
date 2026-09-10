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
- `refs/bochs/cpu/exception.cc`, `refs/bochs/iodev/devices.cc`, and
  `refs/bochs/iodev/minimal_port_space.cc`; and
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

A further build-surface review found an unregistered Windows-only `bcrypt`
addition in `refs/bochs/Makefile.in`. It is not a Bochs requirement: the
current CLI/BYOB identity and image code owns BCrypt, and admitted external
composition link recipes name `bcrypt.lib` explicitly. S4 removed the
unconditional Bochs template injection and added a focused build-isolation
test that rejects a future BCrypt dependency in the Bochs template.

The execution-plan boundary test was also strengthened during the retained
mechanics review. It now proves that both execution-plan and deferred-startup
forms are explicit opt-ins, that the adapter-runtime include is nested in the
combined opt-in guard, and that the post-hardware/pre-loop call remains inside
the execution-plan guard. This adds no Bochs behavior; it prevents a future
default dependency from being mistaken for the established single consumer.

The same review removed two residual Bochs-side diagnostics (BX-TRACE-060 and
BX-TRACE-061): the `NTDOS64_ADAPTER_TRACE` environment branch and the copied
adapter lifecycle snapshot. The generic BOP observer now runs at the start of
adapter runtime v4 dispatch and writes through an adapter-owned reporter.
Bochs no longer names the observer or reads the adapter trace environment
variable; its #UD seam supplies only copied mechanical inputs and consumes
typed results.

## Build Limitation

After the observer move, a fresh isolated adapter-runtime test build was
attempted under `artifacts/build/t196-s4-adapter-check`. The current shell has
no `cmake`, `gcc`, or `clang` command, so configuration did not start and no
build directory or executable was produced. The pre-existing
`artifacts/build/adapter-tests` directory has no CMake cache and contains only
older binaries; it was not used as evidence for the changed source. Source
boundary checks passed, but current adapter compile/link verification remains
required at S6 on a declared toolchain island.

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
Test-BochsBuildIsolationBoundary.ps1                passed
bx-ntvdm-minimal-machine-boundary.ps1               passed
bx-ntvdm-minimal-mechanics-boundary.ps1             passed
bx-ntvdm-minimal-sim-boundary.ps1                   passed
```

The retired diagnostic scan over `refs/bochs`, `tests`, and `tools` for the
eight S3 diagnostic families returned no match. After the port-space repair, a
zero-context comparison against the pinned upstream tree reported 25 changed
paths and 42 physical hunks. After the unrelated BCrypt-link removal it
reported 24 changed paths and 41 physical hunks. The S3 baseline was 43
physical hunks; these are topology changes from S4 repairs, not a
row-for-hunk accounting rule.

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

## S4 Closure And Handover

S4 is closed on source/static evidence. The final focused run passed the
adapter-owned BOP observer boundary plus 13 retained Bochs mechanical boundary
checks. The pinned upstream comparison is 24 paths and 41 physical hunks.

The hunk groups have the following S4 disposition:

| Group | S4 disposition | S5/S6 follow-up |
| --- | --- | --- |
| Minimal SIM/machine, no-SIM RAM and empty port space | Retained as tested selector-blind Bochs-local mechanics; port lifecycle extraction completed. | S5 reconciles all affected register rows; S6 builds the admitted closure. |
| Generic #UD, copied CPU/window, RAM result and controlled stop | Retained only as typed mechanics. BOP observer and adapter diagnostics were removed from Bochs. | S5 proves every remaining exception hunk maps to a live mechanical row; S6 compiles it. |
| Execution/deferred startup plan | Retained as a single default-off post-hardware/pre-loop consumer. | S5 verifies generated build isolation; S6 performs the fresh declared build. |
| `FMT_LL` lexical edits | Retained as fourteen source-exact compatibility edits; the lexical test passes. | S6 must supply the current declared MSVC proof. |
| Build deltas | Unowned Bochs BCrypt injection removed. | S5 reconciles templates/generated files; S6 proves actual link inputs. |

No S4 result proves a build, link, continuous guest execution, or BOP runtime
closure. Those claims remain prohibited until the applicable later S evidence
exists.
