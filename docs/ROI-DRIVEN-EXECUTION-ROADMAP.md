# ROI-Driven Execution Roadmap

Status: proposed execution order, 2026-08-07. This is a research planning
document for `ntdos64`; it authorizes neither changes to `ntvdm64` nor a
default, distributable, or host-integrated runtime.

## Outcome And Decision Rule

The first executable outcome is deliberately narrow:

```text
user-selected NT4 EN-US BYOB files
  -> repaired OpenNT SoftPC/CCPU software monitor
  -> original BIOS/BOP/DEM/NTDOS responsibility chain
  -> NTIO -> NTDOS -> COMMAND.COM /C <owned command>
```

Success requires a bounded command exit status, a redacted trace identifying
the applicable historical oracle, and no installation, injection, registry
change, system-file write, ambient drive discovery, or guest artifact
retention. It does not claim a restored historical CCPU profile or a general
NTVDM replacement.

Every proposed task is admitted only if it removes a blocker on this outcome
or converts an unverified startup assumption into an auditable contract. A
module that merely reduces unresolved-symbol count has no execution ROI unless
the bounded boot trace can reach it.

## Guest Artifact Closure Baseline

The requested research output includes the complete set of NT-modified DOS
system files and programs that the fixed source tree declares, not only the
four files needed for the first command-smoke sequence. That campaign is now
closed as an artifact-governance baseline: 33 direct DOS payload/install roles
plus five controlled WOW16 drivers are reconciled as 38 controlled artifacts.
The 75-item MVDM census remains a broader descriptor classification, not 75
unfinished DOS payloads.

The historical host-chain work is the active research path. `ntvdm64`, Bochs
and PCjs remain evidence-only comparison sources: they are neither substitutes
for the preserved historical chain nor prerequisites for its repair.

## Gate 0: NT4 Guest Artifact Campaign (Completed Baseline)

**ROI: completed.**

The authoritative work order is
`OPENNT-FULL-ARTIFACT-CLOSURE-WORK-ORDER.md`. Before an execution attempt can
claim a complete NT4 research payload, every declared output and resolved
install-rule output must be classified and have one of these evidence states:
`built`, `blocked` with an exact source/tool/layout reason, `external input`,
or `sample/development-only`.

1. **G0.1 - Bootstrap payload batch.** Close source, EN-US resources,
   historical tool/format contract, and staged output evidence for `NTIO.SYS`,
   `NTDOS.SYS`, `COMMAND.COM`, HIMEM, ANSI, KEYBOARD, COUNTRY, KB16, required
   messages/NLS data, and their real install companions.
2. **G0.2 - DOS program and service batch.** Close DEBUG, EDLIN, APPEND,
   SHARE, SETVER, FASTOPEN, REDIR, NLSFUNC, EXE2BIN, MEM, LOADFIX, GRAPHICS,
   DOSX, QBasic/EDIT and each indirect command-directory output. A program is
   not "closed" merely because objects assemble or a structural image exists.
3. **G0.3 - Peripheral and pairing batch.** Resolve guest versus host versus
   WOW16 ownership for redirector, mouse, keyboard, display, comms, printing,
   sound, V86 utilities, and their data/resource companions. Record source
   ownership even for deliberately deferred runtime features.
4. **G0.4 - Full inventory reconciliation.** Re-run the fixed-source
   descriptor inventory; resolve the 75 declared outputs and every indirect
   install output into the artifact ledger. Emit only a local non-distributed
   manifest of hashes, sizes, source provenance, toolchain commands, and
   blocker state.

**Result:** no artifact is silently omitted, and no guessed replacement is
called a historical output. The detailed evidence is
`docs/NT4-GUEST-ARTIFACT-CLOSURE.md`. New runtime work consumes only a
user-supplied BYOB set corresponding to those recorded roles.

## Current Starting Point

**Status correction, 2026-08-09.** Gate 0 is closed as a guest-artifact
baseline. The active execution slice is now S5: the original
`BIOS[50h] -> MS_bop_0 -> DemDispatch` path and selected original DEM callers
have default-disabled caller-first evidence; x64 S4 and i386 original SAS also
share one named-section backing through the original `host_sas_init` /
`host_sas_term` lifecycle, followed by one S4 real-mode `NOP` fetch from that
backing. This is not a runnable BOP/DEM continuation. The immediate gate is to
recover the historical CCPU composition/session-owner evidence required before
a versioned, constrained invocation transaction can even be admitted. Such a
future transaction must validate a copied real-mode state and atomically receive
a historical result; it must not introduce a new BOP/DEM dispatcher, switch CPU
mode, own SAS/CCPU lifecycle, or make a DOS service succeed.

Evidence: `docs/research/ccpu-bios-closure/S5-S4-SHARED-APERTURE-EXECUTION.md`.

Already established:

- fixed historical source inputs and a default-disabled x86 clang-cl island;
- CCPU bounded SM-0/SM-1 execution, original BIOS table reachability, and
  original BOP/DEM object closure evidence;
- initialized original `nt_aorc.c` register forwarding plus one original
  `demLockOper` successful caller path, with no guest or SVC dispatch;
- original owner classification for the A20 forwarding, timer, sound, event,
  session/bootstrap, media, OEM path, and selected EMS seams;
- an NT4-style prepared-entry state card and BYOB safety exclusions;
- frozen self-authored BOP/DEM handlers and IVT/BDA scaffolding, usable only
  as trace instrumentation.

Not established: a linkable original chain, original DEM execution, a contained
DOS namespace, an original NTIO-to-NTDOS transition, or a runnable command
processor. A fixed NT4 EN-US BYOB **metadata admission profile** is established
and regression-tested; it is deliberately not a guest loader or runtime.

## Phase 1: Evidence Baseline And Cut Definition

**ROI: very high. Estimated scope: short.**

1. **R0.1 - Lock the first private profile.** Define the NT4 EN-US BYOB
   manifest: required role names, allowed optional roles, version/hash
   recording, pairwise consistency checks, and rejection diagnostics. It
   stores metadata only, never guest bytes or machine-local paths.
2. **R0.2 - Declare the bootstrap cut.** Consolidate the existing NTIO state
   card into one executable-cut checklist: low RAM, IVT/BDA policy, NTIO and
   NTDOS placement, boot-register sources, required initial services, and the
   exact first stop that requires a real machine/provider contract.
3. **R0.3 - Freeze a trace corpus.** Preserve only redacted event/state
   records for the fixed profile. The old self-authored probe remains a
   negative baseline and may not acquire another service handler.

**Gate:** a reviewer can reject an incompatible BYOB set before any guest
memory write, and can identify every assumed startup byte/state without
reading a protected image from repository output.

## Phase 2: Original Historical-Chain Connectivity

**ROI: very high. Estimated scope: medium and technically risky.**

The forced CCPU startup-prefix probe is bounded by `InitNtCpuInfo`, but the
original build definitions establish that it is not the NT4 x86 profile: x86
uses the V86 `MONITOR` path while `CPU_40_STYLE` belongs to non-x86 profiles.
No Win32 shim, no-op, or cross-architecture declaration can turn that probe
into the historical x86 route.

The bounded CCPU/BIOS/BOP/DEM route is the executable reconstruction path.
It runs as a software monitor on x64; it does not attempt to revive the absent
kernel V86 route. Missing CCPU interfaces are repaired only under their
historical caller contracts, rather than replaced with an external backend.

1. **R1.1 - Link the original CCPU-to-BIOS edge.** Keep the whole original
   `BIOS[]` table and prove original `bop(n)` reaches it. No reduced selector
   table and no replacement BOP decoder.
2. **R1.2 - Link original selector `50h` to `MS_bop_0`.** Retain the original
   `nt_bop.c` service-byte fetch/IP advancement. Verify it calls original
   `DemDispatch`, but do not implement a service switch outside DEM.
3. **R1.3 - Make the DEM register ABI bridge runnable.** Validate the exact
   `get/set* -> c_get/c_set*` mappings against initialized CCPU state. This
   bridge has no independent register storage or mode logic.
4. **R1.4 - Classify only live CCPU event/IRQ dependencies.** Use bounded
   traces to decide whether the first cut needs timer/ICA delivery. Retain
   fail-closed guards for unproven `c_sas_touch`, instruction virtualization,
   mouse, and hardware IRQ paths.

**Gate:** a bounded guest BOP traverses repaired original CCPU, `BIOS[]`, `MS_bop_0`,
and `DemDispatch` with a trace proving original IP behavior. It may stop
before a real DEM service dependency, but no self-authored BOP/DEM routine may
have executed.

## Phase 3: Minimum Contained Host Surface

**ROI: high. Estimated scope: medium.**

Implement only the host seams reached by Phase 1's original callers:

1. **R2.1 - Session lifecycle.** A research-runner outer shell owns direct
   CLI arguments, cancellation, bounded execution and exit collection. It
   does not reuse the historical `-f`/WOW/PIF console product shell.
2. **R2.2 - BYOB resource loader.** Supply NTIO/NTDOS bytes from the selected
   manifest, preflight all ranges, load atomically, and emit only hash/length/
   address diagnostics. Do not use the system directory or create host-root
   placeholder system files.
3. **R2.3 - Contained filesystem and directory facade.** Admit SH-04/05/06/
   11 only as original DEM callers reach them. Begin with one configured
   root, read-only open/read/close and enumeration; defer writes, locks,
   devices and ambient drive discovery.
4. **R2.4 - Standard streams and terminal.** Bind inherited stdin/stdout/
   stderr at the original host boundary. Keep graphics, mouse menus and old
   console-server APIs out of the first cut.
5. **R2.5 - Per-session clock and diagnostics.** Return a session clock and
   structured error/trace records; never set host time, show system dialogs,
   or capture guest image content.

**Gate:** every admitted shim has a ledger row, exact historical caller,
bounded success/failure fixture, and a proof that no host-root or physical
device access occurs.

## Phase 4: NTIO To NTDOS Startup

**ROI: high, but blocked by Phases 1-2. Estimated scope: medium/high.**

1. **R3.1 - Apply the validated prepared entry.** Construct only the profile
   state established in R0: writable low RAM, explicit IVT/BDA policy, boot
   registers, and NTIO image placement.
2. **R3.2 - Restore the original DEM load transition.** The original chain
   handles the NTIO request to load NTDOS. The BYOB loader is only the
   resource provider; it must not replace the DOS transition or mutate service
   semantics.
3. **R3.3 - Add machine providers only on demand.** A20/XMS, timer/PIC,
   keyboard/text display and mounted drive/DPB behavior enter one at a time
   when the trace reaches the original owner. Each requires a source-backed
   contract and regression trace.
4. **R3.4 - Reach the first COMMAND initialization point.** Stop at the
   first unprovided original dependency and update the cut; do not add a
   synthetic success response.

**Gate:** original NTIO transfers to original NTDOS and reaches original
command initialization with an auditable transition trace. No compatibility
claim is made before this gate.

## Phase 5: Bounded Command Smoke Test

**ROI: highest product proof, but only after prior gates. Estimated scope:
medium/high.**

1. **R4.1 - Provide the minimum DOS namespace.** One explicitly mounted root,
   a current directory, 8.3 behavior required by the selected command, and
   ordinary read-only program/file access. Add writable files only when the
   owned smoke command demonstrably needs them.
2. **R4.2 - Start COMMAND.COM with an owned command.** Use the original
   command/DEM path, capture an exit status, and terminate through the
   research runner's lifecycle boundary.
3. **R4.3 - Negative and containment tests.** Invalid profile, missing file,
   path escape, physical-drive request, Ctrl-C/cancellation, and guest fault
   all produce bounded, redacted failure results.

**Gate:** `COMMAND.COM /C <owned command>` returns a documented status; the
trace names original chain participants; no host-global side effect occurs.

## SoftPC Reconstruction Gate

The unavailable kernel V86 route is excluded, but it does not block the
historical software-monitor route. The required next execution action is to
repair the evidence-bounded CCPU/SoftPC missing interfaces, then compose the
original CCPU, SAS, BIOS, BOP, DEM and host owners. This repository does not
wait for, link to, or adopt `ntvdm64/core`.

The reconstruction must remain function-by-function: original caller evidence,
minimal domain, fail-closed unknown inputs, and bounded fixtures. It may not
become a new DOS SVC dispatcher or a generic emulation framework. The binding
rules are in `docs/SOFTPC-RECONSTRUCTION-GOVERNANCE.md`.

`NTIO-CORE-PREPARED-ENTRY-GATE.md` records the narrower connection between the
now-implemented BYOB admission gate and the default-disabled core-machine
probe. It does not relax the original-chain requirement: it defines only the
transactional pre-entry conditions and the first trace stop.

## 归档的 Architecture Track：Derive Mantle/Core Requirements

本节保留为完成 SoftPC 实证之后才能进行的下游信息整理，不是当前路线图、
实现队列或 SoftPC 的任何 gate。它不得要求本仓库构建或接入 core、mantle、
Bochs、PCjs 或其它执行后端。

**ROI: high. Design is driven by the command/host-contract evidence and is
updated after every later gate; source migration waits for mature evidence.**

1. Reduce observed requirements into neutral contracts: checked guest memory,
   prepared entry, immutable ROM mapping, reset boundaries, event disposition,
   capability injection, and diagnostics.
2. Mark each contract as `core`, `mantle`, product, or external wrapper based
   on a proven second consumer, not the NT4 name or BOP selector.
3. Feed only neutral, evidence-backed deltas to `ntvdm64`'s governed core
   migration plan after a second consumer or equivalent product need is shown.
   The external wrapper remains outside its source/build/release graph.

**Gate:** every proposed shared API can be stated without NT4 file names,
BOP selectors, SVC numbers, DOS-private structures, or wrapper CLI policy.

## Explicitly Deferred Work

These items have low ROI before Phase 4 and must not expand the first cut:

- V86 monitor, `NtVdmControl`, HAXM, injection or private console APIs;
- WOW, Win16, redirector/network, ISV loading and debugger product behavior;
- serial, parallel, printer, mouse menu, graphics and full-screen UI;
- full EMS, DPMI, protected-mode, x87 and cycle-accurate timing;
- physical drive access, ambient host-drive enumeration, and host-global
  configuration writes;
- `ntvdm64` device extraction or DLL product packaging prior to evidence-based
  mantle/core requirements.

## Queue After Gate 0

The active queue is now the command/host-contract path. Its execution order
is:

1. Audit the original command object/import boundary and retain `CmdDispatch`
   ownership before any command-service implementation.
2. Repair the evidence-bounded SoftPC/CCPU missing interfaces, beginning with
   `c_effective_addr`, `c_sas_touch`, and `c_VirtualiseInstruction`.
3. Then resume the following historical host-chain queue for the first
   executable cut:

   1. **R3.A - Classify the direct-CCPU full-closure diagnostic.** The corrected
   complete-table link diagnostic has 117 unresolved host/ABI dependencies
   in the 2026-08-08 snapshot.
   Classify each by original owner and by whether it is reached by the
   `io_init -> reset -> scs_init -> NTIO` activation prefix. Do not admit an
   implementation merely because it lowers this count. Produce a minimal live
   initialization closure and an explicitly deferred feature closure.
   2. **R3.B - Close the CCPU missing-interface decision gates.** For
   `c_effective_addr`, `c_sas_touch`, and `c_VirtualiseInstruction`, first
   preserve the source-derived caller/argument/failure evidence in controlled
   fixtures. Where no additional original generated input can be recovered,
   any locally implemented completion needs a narrow contract, fail-closed
   behavior outside its evidenced input domain, and separate admission into
   the live initialization closure. It must not implement BOP, DEM, BIOS, or
   mode-switch policy.
   3. **R3.C - Reach original initialization owners.** Build a default-disabled
   trace target from the actual `main.c` prefix: `gfi_init`, virtual-driver
   initialization, `config -> sas_init -> rom_init/copyROM`, `cpu_init`, then
   the original `io_init`/`reset`/`scs_init` subsequence. Add a modern shim
   only when that live caller first reaches a missing host dependency; record
   the caller, input/output contract, containment rule, and bounded
   success/failure test in the shim ledger. Do not enter
   `InitialiseDosEmulation` before the prefix has evidence.
   4. **R3.D - Perform a real NTIO prefix trace.** After R3.C, validate the BYOB
   set before mutation, stage the NTIO image atomically at `0070:0000`, restore
   the historical virtual-interrupt word, enter original CCPU, and stop at the
   first unprovided original dependency. The trace is evidence only; it may
   not supply synthetic device, BOP, SVC, or DOS results.
   5. **R3.E - Restore the original NTDOS load transition.** Only when the live
   NTIO trace reaches `BOP 50h/SVC 11h`, connect the already-defined per-session
   BYOB resource-root seam at the original DEM boundary and prove the original
   `MS_bop_0 -> DemDispatch -> demLoadDos` route. No direct `demLoadDos` call
   or replacement SVC dispatch is admissible.
   6. **R3.F - Add demanded providers one at a time.** Use the next stopped
   original caller to admit A20/XMS, timer/ICA, keyboard/text display, or
   mounted-drive/DPB support. DPMI, protected-mode dispatch, EMS, mouse,
   graphics, redirector, WOW, and old console policy remain off until a later
   bounded cut reaches them.
   7. **R4 - COMMAND smoke test.** Define the minimum contained namespace, start
   `COMMAND.COM /C <owned command>` through the original NTDOS path, collect
   its exit status, then verify containment and cancellation failures.

Mantle/core derivation starts after R3.D has a first real stop trace. It is a
separate downstream evidence reduction task and never a prerequisite for the
SoftPC reconstruction queue.

R0.1's design contract is recorded in
`docs/NT4-ENUS-BYOB-PROFILE-CONTRACT.md`. Its implementation remains a future
separately admitted research-runner task; the frozen trace manifest is not an
implementation substitute.

R0.2's bootstrap admission checklist is recorded in
`docs/NT4-ENUS-BOOTSTRAP-CUT-CHECKLIST.md`. It turns unproven low-memory,
register and device assumptions into explicit trace stops rather than defaults.
