# m0-t200 historical record

Consolidated task record. Raw contemporaneous records are retained in the external documentation archive.

## m0-t200-closure-20260814.md

# M0 T200 Closure — global BOP admission audit and route replan

T200 is closed by owner-approved scope correction. Its deliverable is the
evidence required to safely start one shared host-mutation capability
foundation; it does not claim that BOP providers, continuous guest execution,
or the CLI NTVDM product are complete.

## Delivered evidence

- The reproducible S31 source inventory covers all 203 OpenNT BOP identities:
  73 DEM, 1 WOW, 12 XMS, 25 DPMI, 17 COMMAND, 16 debugger modes, 50
  Redirector services and 9 top-level selectors.
- Every identity has exactly one owner package, mutation class, profile
  disposition, target disposition and retain/migrate/replace/delete workaround
  action. The exporter rejects cardinality drift, duplicate identities and
  blank/unclassified admission fields.
- The audit distinguishes direct ordinary user-mode host APIs from historical
  private/inapplicable dependencies (CCPU/SAS, VDM broker, VDMREDIR, WOW and
  x64 LDT paths). An unavailable dependency remains an explicit row; it does
  not block unrelated owner packages.
- Current bx-vdm routes are classified as foundations, explicit unavailable
  fences, or endpoint-local evidence. In particular they are not credited as
  complete DEM, COMMAND, XMS, DPMI, Redirector, Debugger, WOW or machine
  provider recovery.

Evidence: [S31 BOP admission ledger](../../artifacts/documentation-archive/20260910/etc/research/t200-s31-host-integrated-bop-provider-audit-ledger-001.md),
[unified mutation-profile proposal](../../artifacts/documentation-archive/20260910/etc/research/proposal-unified-host-mutation-capability-profiles-001.md),
and `tools/governance/Export-T200S31BopSourceInventory.ps1`.

## Replan

T201 begins before Host-integrated OpenNT BOP capability recovery. It owns the
shared direct/readonly/volatile-overlay/virtual-volume ABI, owner enforcement,
overlay lifecycle and profile regressions. Only after that common foundation
closes may subsequent owner packages recover DEM, COMMAND, Redirector or
other providers.

No T200 result proves that a historical provider is directly buildable, that a
source-derived replacement is correct, that an unavailable profile is
implemented, or that guest trace behavior is repaired.


## m0-t200-s1-closure-20260814.md

# M0 T200 S1 Closure — Native CLI Integrated Baseline

The first authorized native run after T199's BOP closure source-built the
current CPU5 composition and stopped at the controlled COMMAND `54:01`
frontier.  It also reached established DEM and machine-handoff markers.

The observation does not include the stopped CMDINFO input state and therefore
does not support an implementation conclusion.  Follow-up remains one complete
COMMAND package diagnosis, not a one-service patch.  Evidence is in
`artifacts/build/t200-s1-native-cli-baseline-r1/`.


## m0-t200-s10-closure-20260814.md

# M0 T200 S10 Closure — CLI terminal-owner boundary

S10 establishes that the T200 S9 exit code is fixture assertion evidence, not
a CLI or guest exit.  The next owner package is native CLI/engine composition,
whose first bounded task is the fixed-width entry/result ABI map.  Evidence:
`docs/etc/research/t200-s10-cli-terminal-owner-boundary-001.md`.


## m0-t200-s11-closure-20260814.md

# M0 T200 S11 Closure — native engine entry ABI map

S11 maps the retained `bochs.exe` shell and full-product startup hook against
the intended five-component architecture.  It admits a C-only native-engine
request/result contract in `bx-mantle`; no implementation, build, or guest run
occurred.  Evidence:
`docs/etc/research/t200-s11-native-engine-entry-abi-map-001.md`.


## m0-t200-s12-closure-20260814.md

# M0 T200 S12 Closure — native engine contract foundation

The versioned C-only native-engine request/result contract now has a passing MSVC x64 `/MT` two-source regression. It is contract evidence only: no machine or guest ran, and the old product shell remains. Evidence: `docs/etc/research/t200-s12-native-engine-contract-001.md`.


## m0-t200-s13-closure-20260814.md

# M0 T200 S13 Closure — native engine lifecycle skeleton

The native-engine entry now validates the copied contract, uses a direct
`bx-vdm` copied-input installation seam, and unconditionally resets after an
attempt.  Its x64 `/MT` regression uses no machine or guest.  Evidence:
`docs/etc/research/t200-s13-native-engine-lifecycle-skeleton-001.md`.


## m0-t200-s14-closure-20260814.md

# M0 T200 S14 Closure — direct composition installation

The native engine has a source-built x64 `/MT` direct composition closure:
the exact v7 profile/root install and reset successfully twice without an
environment handoff, machine initialization, CPU loop, or guest execution.
Evidence: `docs/etc/research/t200-s14-direct-composition-closure-001.md`.


## m0-t200-s15-closure-20260814.md

# M0 T200 S15 Closure — native engine machine-lifecycle map

S15 maps the product lifecycle and rejects both the full-product Bochs startup
hook and fixture-only finite runner.  It identifies the direct-composition
startup-plan provider and XMS release ordering as the next owner package.
Evidence: `docs/etc/research/t200-s15-native-engine-machine-lifecycle-map-001.md`.


## m0-t200-s16-closure-20260814.md

# M0 T200 S16 closure — direct composition startup-plan provider

S16 is complete. The copied-input bx-vdm composition owns the selected NTIO
image and produces the pointer-free startup plan with an explicit borrowed
payload lifetime. Reset releases the image and rejects later preparation.

`Invoke-T200S16DirectStartupPlanProbe.ps1` passed at
`artifacts/build/t200-s16-direct-startup-plan-r6`, rebuilding the current
source-built composition as x64 `/MT`; the retained S14 probe also passed.
No machine initialization, CPU loop, guest execution, environment startup,
adapter runtime, or finite-run API was used.

Evidence: `docs/etc/research/t200-s16-direct-composition-startup-plan-closure-001.md`.


## m0-t200-s17-closure-20260814.md

# M0 T200 S17 closure — product preentry source/ABI map

S17 closes the source and ownership map for initial NTIO publication. OpenNT
proves the narrow capture/publish/restore/CS:IP sequence, and existing S16
provides its payload and pointer-free plan. The first NTIO prefix also proves
that IVT/BDA/virtual-interrupt inputs are required.

Current bx-mantle does not own ROM POST or a bound firmware snapshot, while
the S16 profile has no initial-state declaration. Thus no product RAM
publication or machine execution is admitted. S18 will design the copied
profile initial-state declaration and its evidence admission boundary.

Evidence: `docs/etc/research/t200-s17-product-preentry-source-abi-map-001.md`.


## m0-t200-s18-closure-20260814.md

# M0 T200 S18 closure — initial-state declaration ABI

S18 is complete. bx-vdm now has a pointer-free, evidence-identified initial
state declaration with an explicit `ABSENT` state. Absent state is valid data
but not publishable; present state requires complete, non-overlapping bounded
writes and a nonzero evidence identity.

The focused x64 `/MT` probe passed at
`artifacts/build/t200-s18-initial-state-abi-r2`. No machine initialization,
RAM publication, CPU loop, or guest execution occurred.

Evidence: `docs/etc/research/t200-s18-initial-state-declaration-abi-001.md`.


## m0-t200-s19-closure-20260814.md

# M0 T200 S19 closure — initial-state evidence/profile binding decision

S19 explicitly rejects binding r18 stock-ROM/POST snapshot evidence as the
reset output of the current minimal mantle. The mantle does not load ROMs or
run POST; its CPU5 build projection is not a firmware-state identity.

The active v7 profile remains `ABSENT` under the S18 ABI. A future profile
revision may select an evidence-identified external initial-state record, but
that remains opaque composition data and is not a firmware implementation.

Evidence: `docs/etc/research/t200-s19-initial-state-evidence-binding-decision-001.md`.


## m0-t200-s2-closure-20260814.md

# M0 T200 S2 Closure — COMMAND CMDINFO frontier diagnosis

The original `54:01` caller, `CMDINFO` ABI, result order, and current
source-derived provider gates were compared as one COMMAND subpackage.  The
existing native record identifies the BOP but not its input state, so it cannot
support a repair decision.  S3 is admitted solely to build a test-only,
fixed-36-byte CMDINFO observation surface; no guest run or behaviour change is
accepted by this closure.  See
`docs/etc/research/t200-s2-command-cmdinfo-native-frontier-diagnosis-001.md`.


## m0-t200-s20-closure-20260814.md

# M0 T200 S20 closure

V8 profile evidence now selects an opaque bx-vdm initial-state record. The
source-built x64 `/MT` composition probe passed; no guest mechanics ran.


## m0-t200-s21-closure-20260814.md

# M0 T200 S21 closure

The evidence-selected bx-vdm initial-state record now has a typed,
selector-blind mantle publication path. An initialized minimal CPU5 machine
applied and read back the copied ranges; a later invalid range was rejected
before any earlier write. No guest instruction ran.

Evidence: [T200 S21 publication closure](../../artifacts/documentation-archive/20260910/etc/research/t200-s21-initial-state-publication-closure-001.md).


## m0-t200-s22-closure-20260814.md

# M0 T200 S22 closure

The native engine now binds its installed direct composition to a
mantle-private minimal-machine preentry stage. It applies external initial
state, captures the preserved range, writes NTIO, restores the range, and
cleans up twice in a source-built x64 `/MT` fixture. No CPU loop or guest
instruction ran.

Evidence: [T200 S22 machine-stage closure](../../artifacts/documentation-archive/20260910/etc/research/t200-s22-native-engine-machine-stage-closure-001.md).


## m0-t200-s23-closure-20260814.md

# M0 T200 S23 closure

The mantle-private active machine can now receive only a checked copied
real-mode CS:IP record. `0070:0000` is observed after arming; invalid EIP is
rejected without mutation. The engine performs the stage and reset twice with
no CPU loop or guest instruction.

Evidence: [T200 S23 real-mode entry arming closure](../../artifacts/documentation-archive/20260910/etc/research/t200-s23-real-mode-entry-arming-closure-001.md).


## m0-t200-s24-closure-20260814.md

# M0 T200 S24 closure

The active mantle stage now runs only under a finite native watchdog and
returns typed budget or controlled-stop results. The first source-built
profile entered this engine lifecycle twice and both runs ended in controlled
stop without raw #UD or unbounded execution.

Evidence: [T200 S24 bounded native execution closure](../../artifacts/documentation-archive/20260910/etc/research/t200-s24-bounded-native-execution-closure-001.md).


## m0-t200-s25-closure-20260814.md

# M0 T200 S25 closure — first controlled terminal observation

Closed: 2026-08-14
Packet: M0 T200 S25, Ordinary Mode

## Result

Added a default-off bx-vdm fixed-width observation record that retains only
the first already-accepted typed STOP event.  It copies no additional guest
memory and performs no selector/service routing.

The fresh source-built bounded engine fixture captured `C4 C4 54 01` at
`95AB:03C1`; the terminal is the existing COMMAND `cmdGetNextCmd` (`54:01`)
exhausted-plan disposition.  A second run after disabling observation produced
the same controlled terminal.

## Evidence

- [S25 observation](../../artifacts/documentation-archive/20260910/etc/research/t200-s25-first-controlled-terminal-observation-001.md)
- `artifacts/build/t200-s25-current-composition-r1/`
- `artifacts/build/t200-s25-terminal-observation-r1/`

## Scope guard

No BOP provider was implemented or changed.  The observation does not claim
COMMAND or any endpoint is closed; follow-up remains package-level native
COMMAND lifecycle reconciliation, not a trace-derived service patch.


## m0-t200-s26-closure-20260814.md

# M0 T200 S26 closure — COMMAND initial launch/lifecycle reconciliation

Closed: 2026-08-14
Packet: M0 T200 S26, Ordinary Mode

## Result

The first engine terminal is reconciled as the declared one-slot COMMAND
plan's exhausted `54:01` result.  No BOP/provider implementation changed.

The full COMMAND composition regression and focused `54:01`/`54:0B`
lifecycle regression both pass under MSVC x64 `/MT`.  The latter revealed and
corrected a fixture-only DX precondition: `cmdReturnExitCode` returns the low
byte of DX, so the asserted zero result now explicitly supplies zero.

One fresh bounded native engine run again reaches the expected controlled
`54:01` terminal.

## Evidence

- [lifecycle reconciliation](../../artifacts/documentation-archive/20260910/etc/research/t200-s26-command-initial-lifecycle-reconciliation-001.md)
- `artifacts/build/t200-s26-command-package-r1/`
- `artifacts/build/t200-s26-command-lifecycle-r2/`
- `artifacts/build/t200-s26-current-composition-r1/`
- `artifacts/build/t200-s26-engine-native-r1/`

## Follow-up

M0 T200 S27 owns the CLI outer-shell to native-engine direct handoff.  It is
not a new BOP task and must not use the retained legacy product shell or
environment-variable composition route.


## m0-t200-s27-closure-20260814.md

# M0 T200 S27 closure — native CLI engine handoff

Closed: 2026-08-14
Packet: M0 T200 S27, Ordinary Mode

## Result

Added the MSVC x64 `/MT` `ntdos64-native` CLI path.  It validates the BYOB
profile/root/target and calls the native engine with a fixed-width copied
request.  It does not launch the retained Bochs shell or use the old
environment-variable handoff.

The current source-built native CLI run passed with controlled terminal `5/1`.

## Evidence

- [native handoff evidence](../../artifacts/documentation-archive/20260910/etc/research/t200-s27-native-cli-engine-handoff-001.md)
- `artifacts/build/t200-s27-native-cli-r3/`


## m0-t200-s28-closure-20260814.md

# M0 T200 S28 closure — native CLI admission regression

Closed: 2026-08-14

The direct native CLI now rejects empty drive-list arguments, preserves both
include and exclude masks for downstream exclusion precedence, and rejects a
foreign target before engine execution.  Its admitted direct run remains
source-built and bounded.  The excluded-drive guest hang is documented as
owner-package integration risk, not hidden by a CLI workaround.


## m0-t200-s29-closure-20260814.md

# M0 T200 S29 closure — excluded-drive owner map

Closed: 2026-08-14

S29 established that a BYOB profile's contained guest boot drive and the CLI
host-drive projection are separate ownership domains. A focused source-built
COMMAND transaction confirms the current mixed representation can publish
guest `C:` together with zero admitted host drives. The earlier native timeout
remains unassigned; it was not rerun. S30 owns the required
guest-volume/host-projection separation repair.


## m0-t200-s3-closure-20260814.md

# M0 T200 S3 Closure — CMDINFO observation fixture

The fixture-only 36-byte CMDINFO observation surface compiled in the full x64
source-built closure without a guest run.  Evidence:
`docs/etc/research/t200-s3-command-cmdinfo-observation-fixture-001.md`.


## m0-t200-s4-closure-20260814.md

# M0 T200 S4 Closure — COMMAND CMDINFO native observation

The sole admitted native run proved valid CMDINFO input and identified the
exhausted single-slot COMMAND lifecycle as the decline cause.  It also found
the current `54:11` versus original `54:0B` return-service mismatch.  See
`docs/etc/research/t200-s4-command-cmdinfo-native-observation-001.md`.


## m0-t200-s5-closure-20260814.md

# M0 T200 S5 Closure — COMMAND return/next lifecycle

The `54:0B` COMMAND return service, session dispatch, re-entry and exhausted
CLI-plan terminal outcomes are repaired and covered by fresh x64 regression.
Evidence: `docs/etc/research/t200-s5-command-return-next-lifecycle-001.md`.


## m0-t200-s6-closure-20260814.md

# M0 T200 S6 Closure — COMMAND lifecycle native observation

The only admitted run did not traverse `54:0B`; it retained a valid exhausted
`54:01` frontier.  Evidence is
`docs/etc/research/t200-s6-command-lifecycle-native-observation-001.md`.


## m0-t200-s7-closure-20260814.md

# M0 T200 S7 Closure — COMMAND no-next-command diagnosis

`cmdmisc.c` proves that absence of a next host command ends in
`TerminateVDM()`.  The contained replacement is a typed stop, implemented in
the following S8 record.


## m0-t200-s8-closure-20260814.md

# M0 T200 S8 Closure — COMMAND exhausted-plan terminal

The routed `54:01` exhausted-plan path is a no-delta typed stop.  Evidence:
`docs/etc/research/t200-s8-command-exhausted-plan-stop-001.md`.


## m0-t200-s9-closure-20260814.md

# M0 T200 S9 Closure — COMMAND native terminal

The native source-built guest reached an accepted typed terminal at exhausted
`54:01`, with no raw #UD.  Evidence is
`docs/etc/research/t200-s9-command-terminal-native-observation-001.md`.

