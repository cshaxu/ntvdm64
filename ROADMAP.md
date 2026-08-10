# Pre-Governance Detailed Roadmap Record

> **Superseded as the current roadmap authority on 2026-08-09.** Use
> [docs/design/ROADMAP.md](docs/design/ROADMAP.md) and the active
> [docs/STATUS.md](docs/STATUS.md). This detailed record remains retained
> planning evidence until migrated in a bounded batch.

Status: living research roadmap. This directory is outside `ntvdm64` and is
not a source, build, test, release, or Git workspace. It contains no Microsoft
binary, binary-derived material, or local BYOB path in source, default build,
or release content. A hash-manifested research artifact may retain a fixed
reference binary solely for comparison; it is never a runtime dependency or
redistributable output.

## Purpose

This roadmap coordinates one active outcome: repair and compose the OpenNT NT4
SoftPC/CCPU historical path into an independent, non-invasive local research
`ntvdm.exe`. Evidence may later be handed to another project's architecture
discussion, but that is not an `ntdos64` track, task queue, build input, or
acceptance condition. `ntvdm64`, Bochs, PCjs, and similar projects are not
runtime, build, test, or release dependencies, and no external capability is a
gate for the SoftPC route.

## Current Product Shape

The only active implementation target in this repository is a local,
non-release research `ntvdm.exe` which reconstructs the OpenNT NT4 historical
SoftPC/CCPU host path and binds a fixed BYOB NT4 EN-US profile. It is not a
wrapper around `ntvdm64/core`, a mantle implementation, or a substitute DOS
runtime.

Potential `core.dll`, `mantle.dll`, NXVM, and independently implemented VDM
products belong to their owning projects. This repository can later hand off
source-derived observations, but none is a build, link, runtime, or acceptance
dependency of the current SoftPC work. The governing implementation rule is
in `docs/SOFTPC-RECONSTRUCTION-GOVERNANCE.md`: reconstruct an evidenced OpenNT
function boundary or a narrowly justified modern-host seam; do not substitute
an external execution backend.

## Tracks

### Track A: Evidence And NT4 EN-US Contract

Goal: replace broad historical claims with an auditable fixed-profile contract.

- [x] Establish OpenNT as the normative historical source reference.
- [x] Establish NTVDMx64 as a non-normative modern-transition observation.
- [x] Record BOP and SVC topology, including the `BOP_DOS` SVC surface.
- [x] Record NTIO/NTDOS build separation and the historical host composition.
- [x] Define the exact NT4 EN-US BYOB profile and permitted local manifest
  metadata. The design contract and the outer runner's metadata validation /
  engine-handoff policy are complete; guest bootstrap and engine execution
  remain gated on the governed-backend evidence.
- [x] Produce the NTIO bootstrap state card: loader placement, entry CPU state,
  guest memory ownership, IVT/BDA/ROM prerequisites, and all remaining
  unknowns. The R0.2 checklist records the execution admission gate; it does
  not claim a runnable bootstrap.
- [x] Produce the first-profile BOP/SVC ledger for the bootstrap, DEM and
  COMMAND paths, with inputs, guest-memory access, state effects and error
  behavior. Optional/asynchronous families remain explicitly deferred.
- [x] Define the source-derived minimum boot/runtime device matrix; device
  promotion remains trace-gated after historical-chain admission.
- [x] Define and test a redacted passive trace method for one user-supplied,
  version-locked profile. It remains fixture-only until historical-chain
  admission.

Primary evidence: `artifacts/reports/legacy-root-milestones/M11-OpenNT-NTDOS-source-evidence.md`
and `artifacts/reports/legacy-root-milestones/M11-NT4-ENUS-wrapper-compatibility-contract.md`.

### Track A2: Historical Source/Behavior Oracle

Goal: preserve and inspect the original CCPU/SoftPC -> BIOS BOP -> NT BOP ->
DEM -> NTDOS responsibility chain in isolated, non-redistributable source and
object evidence before deriving a neutral adapter contract.

- [x] Freeze self-authored CCPU adapters, BOP/DEM callbacks, and scaffold
  IVT/BDA initialization as bounded startup-trace instrumentation only.
- [x] Compile the original CCPU, BIOS BOP table, `nt_bop`, and DEM source
  groups in default-disabled targets. Full historical-chain link closure is
  still incomplete.
- [x] Classify the initial old-host dependency closure in
  `docs/R1-ORIGINAL-CHAIN-CLOSURE-LEDGER.md`; each future shim remains gated
  on a live original caller.
- [x] Reconnect the original dispatcher and DEM without a replacement SVC
  switch in a bounded trace: `CCPU -> BIOS[50h] -> MS_bop_0 -> DemDispatch`
  reaches original `demQueryDate` and returns through BOP `FE`. This is not
  full historical link closure; inactive complete-table entries remain a
  separately recorded diagnostic.
- [x] Complete the source BOP/SVC ledger needed to specify the first future
  adapter cut. Deferred optional families remain out of the first profile.
- [ ] Validate a bounded `NTIO -> NTDOS -> COMMAND.COM /C` private-image smoke
  test after the repaired historical software-monitor path is available; it
  must not attempt to revive the x86 kernel V86 route.

The immediate activation contract is
`docs/R3-ORIGINAL-BOOTSTRAP-ACTIVATION-LEDGER.md`. It distinguishes original
`io_init`/`reset`/`scs_init` ownership from prohibited historical product-shell
file, VDD, WOW, and priority policy before an NTIO prefix trace is attempted.

The detailed source graph and shim rules are in
`docs/OPENNT-HISTORICAL-HOST-STACK-BUILD-GRAPH.md`. This research deliberately
excludes the historical kernel-V86 route. It instead repairs the historical
CCPU/SoftPC software-monitor path under its own source boundary. V86-only
code is comparison evidence where useful, never an execution prerequisite;
external core or mantle work is likewise not a gate.
`docs/HISTORICAL-RUNTIME-RESTORATION-DECISION.md` is the binding rule that
prevents trace instrumentation or individual BOP handling from becoming a
replacement runtime.
`docs/MODERN-HOST-SHIM-DEPENDENCY-LEDGER.md` is the admission ledger for each
modern host replacement and its required historical-chain evidence.
`docs/R3-DIRECT-CCPU-INITIALIZATION-CLOSURE-LEDGER.md` records the current
full-table diagnostic classification and limits R3 work to dependencies that
can actually be reached before the first NTIO prefix trace.
`docs/R1-CCPU-PROFILE-SEPARATION.md` records the source-proven separation
between the historical x86 V86 `MONITOR` profile and the `CPU_40_STYLE` CCPU
software-interpreter profile; the two must never be mixed to advance a trace.
`docs/R3-ORIGINAL-STARTUP-PREFIX-LEDGER.md` records the prerequisite process
prefix that historically initializes SAS/ROM/CPU before
`InitialiseDosEmulation`; it prevents an R3 trace from treating `reset` as a
standalone machine builder.
`docs/R3-DIRECT-CLI-COMMAND-TRANSPORT-CONTRACT.md` defines the narrow modern
host-context replacement needed to preserve original `config()` command/PIF
data flow without adopting the historical CSR/BaseSrv command transport.
`docs/R3-DIRECT-CLI-SESSION-STATE-CONTRACT.md` records the fixed DOS-profile
session inputs and explicitly excludes the historical event/UI and PIF policy
paths from the first configuration trace.
`docs/R3-CONFIG-PREFIX-TRACE-LEDGER.md` records the executed original
`config()` prefix through its first `sas_term` stop and through the original
SAS/ROM initialization to `InitNtCpuInfo`, including local containment
assertions; it is not an NTIO or runtime admission.

### Archived Cross-Project Observations

Earlier core/mantle migration proposals, external-machine probes, and hardware
ownership notes are retained as evidence only. They are classified in
`docs/governance/document-map.md`; they are not tasks in this roadmap. A later
handoff may summarize facts observed after SoftPC reaches a real historical
boundary, but no `ntdos64` task may be scheduled to implement, require, or
validate an external core, mantle, or emulator.

### Track G: Local Code Governance Reconciliation

Goal: make every locally authored shim, overlay, compiler compatibility input
and trace fixture auditable before it can affect the historical SoftPC path.

- [x] **M92-LOCAL-CODE-GOVERNANCE-RECONCILIATION**: reconcile physical source,
   `SOURCE-MANIFEST.json` and CMake; classify every local file; migrate or
   explicitly mark legacy adapters; remove the external `ntvdm64` CMake
   integration probe and archive-only WHPX/mantle targets from the active
   configuration graph; and prove trace/archived code cannot enter a normal
   runtime target. This is a governance prerequisite for promoting any new shim
   or overlay, not a replacement execution track.

Authority: `docs/LOCAL-CODE-GOVERNANCE-LEDGER.md`.

### Deferred: External NT4 Wrapper Design

This is not an active track. It may be opened only after Track A2 has produced
a real bounded historical-chain execution result through the repaired software
monitor. Until then, these are retained questions, not tasks, gates, or a
reason to introduce core, mantle, or another execution backend.

Goal when admitted: specify, but do not yet implement, the locally contained
wrapper.

- [x] Define BYOB discovery, version matching, rejection diagnostics and data
  retention rules. The runner implements only local metadata admission and
  handoff to an explicit engine; it is not a guest-executing wrapper.
- [ ] Define NTIO/NTDOS image placement and fixed-profile bootstrap sequence.
- [ ] Define selector/service registration tiers: required boot, required
  runtime, optional feature and explicitly unavailable.
- [ ] Define DOS namespace containment: drive mapping, paths, devices, 8.3,
  locking, errors, pipes and host-root authorization.
- [ ] Define terminal, keyboard, display, timer and media bindings at the
  original historical host boundaries, preserving their observed data flow.
- [ ] Define trace redaction, failure reporting and no-redistribution checks.

The wrapper does not depend on `core/product` debug-console or product UX. It
may bind generic streams/input/display when the guest requires them.

## Sequencing And Gates

1. Close the first live historical SoftPC/host dependency reached by a bounded
   trace, using the original owner where it exists.
2. For an absent implementation, recover only the caller-defined semantic
   minimum under `src/opennt/overlay/`, with a focused positive and negative
   fixture.
3. Repeat through original CCPU, SAS, BIOS and dispatcher ownership until a
   software-monitor execution slice reaches the next recorded boundary.
4. Admit a modern host shim only when an already-reached historical caller
   demonstrates a retired host dependency; preserve the original function
   boundary and data flow.
5. Attempt the fixed-profile NTIO/NTDOS smoke test only after the above chain
   can execute to its bootstrap boundary. Later cross-project handoff notes may
   summarize that evidence but cannot delay this sequence.
6. Only after a successful bounded historical-chain result may the deferred
   wrapper questions be promoted into a separate planning pass.

## Non-Blocking Work

The following are valuable but do not block boundary reconciliation or the
first migration slice: complete digital CGA, composite video, advanced AUX,
full protected mode, x87, cycle timing, Linux runtime verification, and
additional machine profiles. They should use the corrected boundary once it is
available rather than force an all-at-once pre-migration completion.

## Review Cadence

Update this roadmap when a research claim becomes direct source evidence, an
OpenNT SoftPC boundary is recovered or rejected, or the target BYOB profile
changes. This file never authorizes source changes in another repository.

## Linked Reports

- `artifacts/reports/legacy-root-milestones/M11-NTDOS-machine-platform-boundary-research.md`
- `artifacts/reports/legacy-root-milestones/M11-VM-to-core-NTDOS-compatibility-foundation.md`
- `artifacts/reports/legacy-root-milestones/M11-NTVDMx64-direct-source-evidence.md`
- `artifacts/reports/legacy-root-milestones/M11-OpenNT-NTDOS-source-evidence.md`
- `artifacts/reports/legacy-root-milestones/M11-NT4-ENUS-wrapper-compatibility-contract.md`
- `docs/ROI-DRIVEN-EXECUTION-ROADMAP.md` - gated execution order for the
  historical-chain research path; it supersedes ad hoc source-closure work
  when choosing the next task.
- `docs/NT4-ENUS-BYOB-PROFILE-CONTRACT.md` - R0.1 local metadata and rejection
  contract; it is not a guest-file manifest or a runtime implementation.
- `docs/NT4-ENUS-BOOTSTRAP-CUT-CHECKLIST.md` - R0.2 source-fact and
  trace-stop checklist for the first BYOB bootstrap cut.
