# DOS/WOW16 Source-Closure Task Brief

Status: proposed bounded build-recovery task, 2026-08-09. This brief does not
change the active SoftPC runtime sequence in `ROADMAP.md`; it creates an
independent, evidence-first closure line for guest and WOW16 artifacts.

## Outcome

Establish a reproducible, source-derived build record for every DOS guest and
WOW16 artifact selected for the NTDOS64 research profile. The work must use the
closest available historical Microsoft source and tools wherever their original
build inputs are present.

The four explicit `binplace` inputs below are excluded from source-build
acceptance and remain hash-manifested BYOB/reference inputs only:

- `BACKUP.EXE`
- `QBASIC.EXE`
- `EDIT.COM`
- `MSHERC.COM`

This task does not make any artifact runnable, redistribute Microsoft binaries,
or create a host execution backend. Guest source-build closure and x64 runtime
closure have separate acceptance criteria.

## Starting Facts

The controlled direct DOS payload currently records 33 artifacts: 22
source-built, seven data/help companions, and four excluded prebuilt inputs.
Five WOW16 drivers (`MOUSE.DRV`, `VGA.DRV`, `KEYBOARD.DRV`, `COMM.DRV`, and
`SOUND.DRV`) have source-build evidence. This is not the complete WOW16 runtime
or the complete MVDM output graph. Authority: `NT4-GUEST-ARTIFACT-CLOSURE.md`.

The first command profile needs only `NTIO.SYS`, `NTDOS.SYS`, `COMMAND.COM`,
and a profile-owned command. `DOSX.EXE`, `HIMEM.SYS`, utilities, and WOW16
artifacts are later feature-profile inputs, not bootstrap requirements.
Authority: `COMMAND-ARTIFACT-ROLE-RECONCILIATION.md`.

## Scope

### Included

1. Inventory every candidate artifact from the fixed OpenNT source tree and
   relevant install/build descriptors, including all WOW16 driver and runtime
   outputs.
2. Classify every artifact exactly once as:
   `source-built`, `static-data`, `excluded-prebuilt`, `source-blocked`,
   `host/support`, or `sample/development-only`.
3. For every source-built artifact, retain the source revision, exact historical
   tool commands, generated input provenance, linker/post-processing steps,
   output hash and byte count.
4. For every source-blocked artifact, identify the first missing source,
   generated input, library, message/resource input, or build rule. Do not use
   an unrelated compiler, toolchain, or binary as a silent substitute.
5. Reconcile the complete WOW16 output set separately from DOS payload files;
   distinguish drivers from Win16 runtime DLLs, tools, and host-side support.

### Excluded

- Any runtime execution, CCPU/SoftPC, BIOS, DEM, BOP, device, V86, DPMI, or
  modern-host implementation work.
- Source reconstruction of the four named excluded prebuilt inputs.
- A claim that byte-identical output is required or that source-build success
  proves runtime compatibility.
- Adding external execution code or a runtime dependency from `ntvdm64`,
  NTVDMx64, Bochs, PCjs, or another emulator.

## Work Packages

| ID | Work package | Deliverable | Exit condition |
| --- | --- | --- | --- |
| G1 | Freeze inventory | Machine-readable artifact inventory with source/install-rule anchors | Every discovered output has a classification; no unclassified entries |
| G2 | Verify existing evidence | Re-run or validate manifests for current 27 source-built artifacts | Commands, hashes, and tool provenance are reproducible |
| G3 | Expand DOS closure | Source/build status for every remaining DOS payload descriptor | Each is source-built, static-data, excluded-prebuilt, or source-blocked with first blocker |
| W1 | Enumerate WOW16 | Separate all drivers, runtime DLLs, tools, and generated resources | Five current drivers are not mistaken for full WOW16 closure |
| W2 | Rebuild eligible WOW16 artifacts | Per-artifact historical build records | Each eligible output has an original-source/tool route or a documented blocker |
| V1 | Audit results | Closure report and profile capability matrix | Artifact availability is separated from runtime admission |

## Acceptance

The task is complete only when:

1. Every included DOS/WOW16 output has one classification and an evidence link.
2. Every `source-built` record lists exact input revision, command/tool chain,
   output hash, size, and any generated/resource input.
3. The four exclusions retain `SourceBuilt: false` and are never counted as
   rebuilt results.
4. The report explicitly states which artifacts are merely build-closed and
   which have an independently admitted runtime profile.
5. Default CMake/CTest remains free of historical guest binaries and runtime
   execution dependencies.

## Non-Acceptance Conditions

The following do not count as success:

- compiling an object without its final historical link/package step;
- copying a reference binary into an output directory;
- treating a `binplace` rule as a source-build rule;
- treating the five rebuilt WOW16 drivers as complete WOW16 runtime closure;
- using a modern substitute tool without a separately documented compatibility
  decision; or
- using guest build success as evidence that the x64 `ntvdm.exe` runtime works.

## Dependencies and Risks

| Risk | Treatment |
| --- | --- |
| Missing source/build inputs, such as the four exclusions | Record as `excluded-prebuilt` or `source-blocked`; do not invent a source route |
| Missing historical tools or generated resources | Preserve the exact first missing input and isolate a tool-recovery task |
| Large WOW16 graph | Inventory first, then prioritize artifacts selected by a declared profile |
| Confusion with runtime progress | Keep this brief independent from `ROADMAP.md` runtime gates |

## Relationship to Runtime Reconstruction

This work removes uncertainty about guest availability. It does not remove the
separate x64 execution requirement: a non-invasive CLI still needs an admitted
software monitor plus the original-owner/modern-host seams reached by the
profile. Runtime work begins only under `SOFTPC-RECONSTRUCTION-GOVERNANCE.md`
and the current `ROADMAP.md` sequence.

## Evidence Inputs

- `NT4-GUEST-ARTIFACT-CLOSURE.md`
- `OPENNT-FULL-ARTIFACT-CLOSURE-WORK-ORDER.md`
- `COMMAND-ARTIFACT-ROLE-RECONCILIATION.md`
- `artifacts/toolchain-runs/nt4-prebuilt-guest-inputs-v1/prebuilt-inputs-manifest.json`
- `docs/source-policy.md`
- `docs/SOFTPC-RECONSTRUCTION-GOVERNANCE.md`

## Task Evidence Outputs

The following records are the authoritative local outputs of this task. Their
provenance-only scope, regeneration command, and preservation rules are
registered in `DOS-WOW16-SOURCE-CLOSURE-EVIDENCE-REGISTER.md`:

- `docs/dos-wow16-source-closure-inventory.json` (machine-readable ledger);
- `docs/DOS-WOW16-SOURCE-CLOSURE-RESULT.md` (human-readable closure decision).
