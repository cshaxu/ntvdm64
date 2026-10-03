# Proposal — Rename the native-text worker and Console frontend

## Status and objective

Owner closed the predecessor and admitted this former queue-head candidate on
2026-10-02. Only [CURRENT](../states/CURRENT.md) owns admission and publication;
the [working plan](../etc/operations/t424-worker-frontend-renaming-plan.md)
owns the ordered stages. The [S1 audit](../etc/evidence/m0-t424-s1-name-referent-audit.md)
pins pre-migration source identities in Git, inventories and hashes.
Names below are migration-normalized; they do not claim runtime acceptance.

Owner's latest bounded-stage revision separates service source refactoring
from common/protocol delivery: S7 delivers common and the two existing
transport families, S8 completes the NTSRV provenance/block ledger and
service-private physical split and S9 implements GUI routing. The subsequent
owner insertion makes S10 the rapid relaunch/frontend lost-wakeup repair;
former S10 frontend naming becomes S11. The next owner approval inserts unified
logical text storage/handoff as S12; the former S12 final audit becomes S13.
The linked working plan is the current
sequence; earlier admission chronology below is retained as history, not
permission to fold S8 work back into S7. T closure remains owner-controlled.

The [S11 delivery ledger](../etc/evidence/m0-t424-s11-frontend-name.md) records
the frontend identity migration and verified eight-file publication. CURRENT
retains the owner's stop after S11; S12 planning is not a second admission.

| Role | S2 intermediate owner/product | Final owner/product |
| --- | --- | --- |
| Resident native Windows worker; text owns hidden Console, GUI retains native windows under S9 | `src/ntw32-exe` / `ntw32.exe` | S6 worker identity: `src/ntvwm-exe` / `ntvwm.exe`. |
| Visible Console/Window frontend, owns display/input/presentation routing | S2 product `ntkvm.exe`; its owner is moved by S11. | S11 frontend identity: `src/ntcon-exe` / `ntcon.exe`. |

The final eight files are run16.exe, ntsrv.exe, ntvdm.exe, ntvwm.exe,
ntcon.exe (frontend only), ntmon.exe, WOW32.DLL and VDMREDIR.DLL.
NTCON and its product-owned symbols must never denote the Win32 worker.
Generic imported kvm-* libraries keep their own source identities.

NTSRV's project-owned native direct record becomes OPENNT_BASE_WIN32RECORD /
Win32Record, with win32records, pending_win32record, next_win32record and
corresponding service_*_win32record helpers. Its source is
src/ntsrv-exe/opennt/source/base_service.c, not an original OpenNT record.
Preserve its fields/layout/order, request, actual target PID/handle, image,
receipt, completion and management projection. Original DOSRECORD/WOWRECORD
and real Console identity structures remain unchanged.

## Ordered migration and collision prevention

1. S1 freezes every old identity by referent: executable/directory, C symbols,
   types/guards, build/object targets, tests, package/launch paths, service/RPC
   references, native record family, diagnostics and all documentation.
   Capture actual eight-file baseline hashes and protocol. Classify original
   Console, library and substring identities rather than blindly substituting.
2. S2 renames only the former native worker and its project-owned record family
   to NTW32/Win32Record, across code, paths, all current and historical documents,
   indexed archives and proposals where they actually denote that worker.
   Preserve recorded outcomes/hashes/dates; Git retains literal earlier records.
   Keep frontend NTKVM. Do not leave aliases or duplicate implementations.
   Run a complete case-insensitive tracked-text/path and untracked-current scan:
   zero old project-worker and record referents. Report every remaining raw hit,
   proving original Console/substrings or explicitly reserved future frontend
   identity; no excluded directory may hide worker references. Preserve original
   OpenNT Console identities and actual API names under the owner's boundary.
   Classify sealed build evidence separately and regenerate current graphs.
   Pass focused tests and full production gates; publish a verified intermediate
   eight-file package with ntw32.exe and ntkvm.exe, removing the obsolete worker
   basename only after recoverable backup. Never publish an untested mixture.
3. S3 starts only after S2's audited zero-old-worker gate and production delivery.
   Rename frontend NTKVM to the reserved NTCON identity, including owner-local
   symbols/directories, executable/build/package/tests and all consumers.
   Reconnect to NTVDM and NTW32 through existing interface/worker-base contracts;
   do not invent another frontend or worker mechanic.
4. S4 independently audits final referents and semantic equivalence. Every
   project NTCON denotes frontend, every native worker uses NTW32; original
   OpenNT Console source names remain originals. Normalize all relevant current
   and historical text and links, retaining exact pre-migration records in Git.
5. Review cross-process identities separately. Advance APP_VERSION once per T
   to 0.0.424 and verify existing application mismatch rejection. Unchanged wire
   layouts keep protocol/RPC 28 and its UUID. An actual copied-wire, endpoint,
   ACL or command-line change requires coherent versioning and renewed review.
   No mixed old/new application set may register as a coherent product.

Moves use git mv. Original OpenNT/MVDM mirrors and guest media retain identity,
algorithm, source layout and behavior. No shared-library rename, extra process,
helper, scheduler, observed graph or launch syntax change is admitted.
Subsequent owner-approved S3 retirement and S4 broker-centered creation/control
are explicit bounded exceptions to this package's initial name-only policy.

## S sequence and gates

| S | Bounded deliverable |
| --- | --- |
| S1 | Read-only full referent inventory, ABI decisions, hashes and migration map. |
| S2 | NTW32/Win32Record migration, complete-tree referent and original-name preservation gates, full production verification and intermediate eight-file publication. |
| S3 | Owner-added abnormal-exit/re-launch lifecycle investigation and explicitly approved broker-owned retirement repair; highest-priority authenticated close, bounded startup admission, repeated-launch/failure/isolation tests and coherent publication. |
| S4 | Owner-added broker-centered launch/control migration, including authenticated Console takeover/return coordination; actual Console operations stay in NTKVM and worker/frontend I/O stays direct; no mirror edits. Remove replaced paths, test both worker kinds and publish a coherent eight-file package. |
| S5 | After S4 delivery, architecture/code cleanup of owner-approved audit findings 1-5: consolidate duplicate control transport, clarify shared implementation ownership, remove launcher target completion dependence, clear NTKVM dead state and obsolete tests; preserve behavior and pass the full production gate. |
| S6 | After S5 cleanup delivery, NTW32 -> NTVWM native worker/component migration. Name-only equivalence, consumer/build/test/package updates and coherent ntvwm.exe + ntkvm.exe eight-file publication; preserve existing behavior, Win32Record and kind values. No GUI routing implementation. |
| S7 | Common/two-protocol organization after S6; consolidate suitable project-added shared mechanisms. Complete service source separation transfers to S8, not a claimed S7 delivery. |
| S8 | NTSRV provenance/block ledger and service-private source separation; preserve original mirror semantics and one state/lock authority, regress and publish. |
| S9 | Native GUI routing through NTVWM and service-held handles; launcher classification precedes admission as in DOS/WOW. Default launcher returns on startup success. Preserve --wait/shared workers. Monitor/UNBOUND display belongs to the queue-head NTMON T candidate. |
| S10 | Rapid relaunch/frontend lost-wakeup repair after S9: one lock-protected NTSRV pending-work notification mechanism, deterministic race and fast CMD/COMMAND reuse tests, unchanged completion/Console-return/isolation semantics and full publication gates. |
| S11 | Frontend NTCON migration after S10 repair delivery; build/test/wiring/name checks and verified final eight-file publication. |
| S12 | Unified frontend logical_surface for NTVDM/NTVWM text, fixed upper-left projection against actual canvas and symmetric current-state DOS/native handoff; implementation, regression and coherent publication after S11. |
| S13 | Former S12 final ownership/semantic-diff and naming audit, indexes/authorities, clean committed/pushed delivery; owner decides T acceptance. |

## Owner-added S12 unified text surface

S12 is approved for sequential execution after S11 naming delivery, not a second
active packet. NTCON then means the frontend formerly named NTKVM; NTVWM remains
the native worker. Generalize frontend-owned dos_surface into logical_surface
and require both workers' text to enter it before Console/Window presentation.
Remove replaced native visible-Console and text-render bypasses. The native
worker retains its hidden execution Console; original mirrors and execution
remain untouched. This is an explicit bounded presentation/handoff extension
to the initial name-only scope, not a new backend or lifecycle policy.

The logical grid, viewport and cursor are independent of physical canvas size.
After any attempted physical resize, use measured actual dimensions for fixed
upper-left intersection painting and blank-margin clearing, including attributes.
Clipping never discards logical data; out-of-view cursors hide instead of moving
the viewport. Both handoff directions commit/acknowledge the old final state,
apply/acknowledge the incoming worker's supported state, then release input and
execution. Inherit current state, not initial CMD geometry; retain original
DOS height selection and cell-grid resize, not paragraph reflow or compensation.
Host scrollback is not promised. The
[S12 checklist](../etc/operations/t424-worker-frontend-renaming-plan.md#s12-unified-logical-surface-and-dosnative-handoff)
owns production wiring/removal, failure/race/geometry tests and publication
gates. Current active admission remains solely in CURRENT; final audit is S13.

## Owner-added S10 rapid relaunch repair

The owner reports occasional hangs when rapidly repeating run16 cmd -> exit
in one outer CMD and inserts the repair before frontend naming. Latest source
inspection still finds service_frontend_idle resetting frontend_capability
without accounting for undecided frontend joins. A join published between
NTKVM's join check and channel-empty check can therefore lose its notification.
This is a source-proven possible race, not a claimed live reproduction or the
proven cause of every hang. The
[S10 checklist](../etc/operations/t424-worker-frontend-renaming-plan.md#s10-rapid-relaunch-and-frontend-notification-checklist)
records inspected paths, ownership, retained distinct event contracts and tests.

Keep pending state and event updates under the existing NTSRV service lock;
one private mechanism covers all work represented by the shared notification.
Use deterministic interleaving tests before rapid product repetition. Preserve
the condition-variable lease wait, ten-second admission deadline, direct
receipt and Console-return acknowledgement; no delay/retry/polling workaround,
new transport/process/component or original mirror/guest change is authorized.
Publish only after the full production gate. S9 remains the only active packet;
S10 is planned, not implemented/admitted or accepted. Earlier numbering and
active-stage statements in the retained approval chronology below are historical
and superseded by the current S table and CURRENT.

Owner direction on 2026-10-02 adds lifecycle investigation before further
renaming. Earlier S3/S4 migration labels in this proposal now refer to S11/S13
after the owner-approved architecture, native-GUI and NTVWM naming insertions.
Both run16 cmd and run16 command reportedly hang before any prompt after an
apparent abnormal exit; NTMON reports Ready but no worker. Preserve the live
scene, distinguish a resident root from an unusable lease and investigate
NTSRV admission, completion and workerless-grace ordering. This is research
scope initially. The owner's subsequent explicit approval admits implementation:
NTSRV alone decides orderly frontend/worker retirement; NTKVM keeps the user
Console-close boundary, while NTVDM/NTW32 consume authenticated broker shutdown
events. Broker instructions take precedence over pending/lease/I/O state.
Orphan roots retire without an idle prerequisite or a no-worker grace; only a
live authenticated startup owner grants a bounded ten-second admission window.
NTSRV's own empty grace cannot start with live registered frontends/workers or
legal admissions. This overrides the investigation-only limitation, not S2's
name-only scope. Original DOS cleanup and native Console-close acknowledgement
remain in their respective workers; no helper, scheduler or tree kill is added.

## S4 owner-approved broker-centered boundary

This approval replaces project-added launcher/frontend/worker creation edges,
not original OpenNT execution, scheduling or task completion. NTSRV creates
and binds NTKVM, NTVDM and NTW32; run16 only finds/starts NTSRV, submits one
classified request and waits for its broker result when existing semantics
require waiting. The owner's subsequent refinement sends Console takeover,
return and restoration acknowledgement through NTSRV too; run16/NTKVM need
no direct IPC. NTKVM performs AttachConsole against the authenticated actual
caller while run16 remains alive/attached until takeover acknowledgement.
NTSRV never attaches to the Console. Root run16 returns only after the service
receives canonical buffer/input restoration acknowledgement; inner launchers
do not return the root lease. Preserve current geometry/grid/cursor instead
of restoring the startup snapshot. NTKVM/worker direct transport carries only I/O,
including input return, route activation and final-paint/restoration barriers;
it cannot own task completion or orderly component retirement.

NTKVM and both workers obey NTSRV's highest-priority control, broker loss and
their own unrecoverable faults. NTKVM additionally retains true user Console
closure. NTSRV owns finite ten-second startup/workerless-root deadlines and its
empty-service grace; NTKVM itself waits indefinitely without an idle timer.
Each connected nonmonitor component uses the authenticated broker process
handle for event-driven death detection, not an idle RPC assumption or a
heartbeat. NTMON remains a disconnected monitor until its user exits.

NTW32 still creates/waits on actual native targets and reports their actual
exit code and final I/O outcome to NTSRV; NTSRV stores the authoritative direct
receipt/result. DOS completion retains its original record implementation.
Neither process ancestry nor I/O association creates recursive kill policy.
GUI startup-only, Win16 registered startup-only, CLI arguments, resident reuse,
screen/input continuity and isolation are retained. No new process/component,
mirror/shared-library/guest change, Observed graph or scheduler is authorized.

The [S4 source-edge ledger](../etc/evidence/m0-t424-s4-broker-centered-launch-control.md)
defines migration rows and negative/runtime gates. Its initial audit is not
implementation or publication evidence. Original numbered rename steps above
are historical order; the S table is the current ordered working plan.

## S5 owner-approved architecture/code cleanup

After S4 delivery, close audit findings 1-5 as one bounded S5 before renaming.
The [working-plan checklist](../etc/operations/t424-worker-frontend-renaming-plan.md#s5-architecturecode-cleanup-checklist)
owns detailed source dispositions and tests: single completed-I/O-first control
transport, explicit shared implementation ownership, broker-only native-text
completion/return without launcher target-HANDLE decisions, NTKVM dead-state
and annotation cleanup, and assertion-preserving removal of obsolete bootstrap
tests. Retain the existing owned-client-library pattern where valid; do not
add a generic component or make nonworkers link worker-base. interface remains
declaration-only. Different frontend peer-death ordering is not merged blindly.
Preserve failure receipts, real exit codes and Console restoration barriers.
All five rows require production wiring, duplicate/dead-path removal, exact
tests and the full production-P publication gate. Original mirrors, guest,
shared lib, accepted polling, launch syntax and GUI behavior are unchanged.
Console-list consolidation and the broader service-file split are excluded.
S4 stays the only active packet until delivery; planned S5 does not excuse any
unfinished S4 gate. S6 naming, S8 GUI routing, S9 frontend naming and S10 audit
follow sequentially. This planning update is not runtime or closure evidence.

## S7 owner-approved common library and source separation

After S6 naming delivery, transform interface into a bounded common static-library
family: retain a declaration/IDL-only protocol submodule and consolidate
audited cross-component project-added mechanics in explicitly owned modules.
Keep worker-only mechanisms in worker-base and service/frontend/backend policy
in the executable owner. No new executable or helper is authorized.
The [S7 checklist](../etc/operations/t424-worker-frontend-renaming-plan.md#s7-common-library-and-service-source-separation)
owns provenance, source/link migration, Console snapshot mechanics and tests.

Classify base_service.c by source provenance, not its opennt directory name.
Split only project-added NTSRV implementation into service-private modules;
original OpenNT algorithms and selected mirror paths remain intact with minimal
registered binding hooks. Original execution/completion may not move into
common or worker-base. Record pinned-upstream comparisons and per-mirror diff
accounting, wire-version effects, deleted duplicates and retained differences.
S5 remains active. S6 naming precedes planned S7 common/service separation;
S8 GUI, S9 frontend naming and S10 audit follow.
This express bounded admission supersedes the previous generic-common ban
for S7 only; it does not authorize an unrestricted utility/framework component.

## Subsequent owner-approved native GUI boundary

Win32 GUI is no longer a local run16 CreateProcess exception in the target
architecture: native requests go run16 -> NTSRV -> NTVWM. The owner's subsequent
classification-order correction keeps GUI/text classification in run16 before
service admission and text frontend binding, matching DOS/WOW. NTVWM executes
the admitted kind rather than adding worker preflight/resubmission. This
changes routing, not default GUI startup-only or explicit --wait semantics.
NTVWM reports successful actual creation/binding/start to NTSRV; the service
can then return the default launcher's startup result. NTSRV retains the
authenticated GUI process handle after the worker releases this request and
after run16 returns; it removes registration on actual process exit. The owner's
final clarification assigns monitor/UNBOUND display to the queue-head
[NTMON candidate](proposal-ntmon-console-worker-tree-001.md), not S4-S8.
The future view uses service registration only, never local enumeration.
No Win16 per-task process handle is invented to imitate native GUI ownership.

Releasing a GUI request must not terminate the target or a worker that still
serves CMD. The owner's original-WOW clarification resolves release as request
occupancy, not carrier termination: retain the resident worker, BUSY if other
text work remains and READY otherwise. Later broker retirement is independent.
GUI segments do not propagate character frontend authority.
Keep kind values DOS=0, Win16=1, Win32=2 and existing monitor hotkeys. No helper,
observed descendant graph, scheduler, guest or mirror changes are admitted.

The owner's later clarification retains S4 as the active migration, superseding
the premature replanning conclusion and then-labelled S5 admission. Its open rows remain S4
release requirements, listed in the working plan and CURRENT. S5 cleanup starts
only after S4 delivery; naming is S6 and GUI routing is S8 after S6. No candidate publication or
production P is claimed by these
planning changes.

An externally fixed project identifier or imported-original conflict requires
review, not an alias. Original Console names already classified by S1 are
outside the requested product rename. Documentation-only P uses governance,
link and diff gates; every production P uses the complete runtime gate.

## Owner-added NTVWM naming stage

Owner direction on 2026-10-02 inserts native worker naming before frontend
naming. The subsequent naming-first reorder assigns NTW32 -> NTVWM to S6,
after S5 cleanup delivery, to express the native Windows worker role symmetrically
with NTVDM; S7 then reorganizes common/service code and S8 implements unified native text/GUI routing. S9 retains
NTKVM -> NTCON; final audit moves to S10. This is planning only: S4 stays active,
and current production/source names remain NTW32 until the admitted S6 migration.
Earlier NTW32 references describe S2-S4 inputs and delivered historical facts,
not an alternative final worker name.

S6 covers src/ntw32-exe -> src/ntvwm-exe, ntw32.exe -> ntvwm.exe, owner-local
symbols/guards, consumers, generated build selection, tests, configuration,
launch/package paths and relevant documentation. Audit provenance/referents
before renaming; preserve original OpenNT/MVDM and Windows API identities,
Win32Record, monitor kind values and imported KVM library names. No alias,
additional worker implementation, helper, launch-syntax or lifecycle change.
S8's GUI routing is not implemented or claimed by this name-only stage. Both worker
kinds continue to use existing interface/worker-base contracts and NTSRV-owned
common consumer paths. Coherently manage any proven wire/endpoint compatibility
effect rather than bumping an ABI for local spelling alone.

Apply the full production gate, including native GUI startup/explicit --wait,
native text interaction/return, DOS/native nesting, lifecycle/isolation,
Console17 + Window17 and retained WOW frontiers. S6 tests existing GUI behavior,
not the unimplemented S8 route. Before S7, publish and hash
verify the eight-file ntvwm.exe + ntkvm.exe intermediate set; remove the old
ntw32.exe only after recoverable backup. Final acceptance below reads NTW32
as the S2-S4 input identity and NTVWM as the post-S6 native worker identity.

## Acceptance

- Regenerate MSVC Win32/x86 /MT CCPU40 Ninja/MIDL graph; affected compilation
  and links have no stale source/object/deployment identities.
- Exercise direct/nested COMMAND/MEM/EDIT and native CMD/modern EDIT including
  S40 return, DOS/native handoff/resume, Console/Window switching, residency,
  faults, independent sessions and unchanged launch arguments. Keep all
  Console17 + Window17 assertions and WINMINE/SOL/WRITE retained frontiers.
- Prove run16/NTSRV/NTMON/NTVDM/NTW32/frontend agree on kind, real process
  identity, frontend ownership, direct receipt and teardown. Monitor must not
  mistake a frontend for a native worker.
- Win32Record remains one actual admitted direct request, with unchanged
  exit-code/receipt and stack behavior; no observed descendant record is added.
  Original DOS/WOW and genuine Console records remain byte/semantics unchanged.
- Scan all source, tests, scripts, config, manifests, current/history/archive
  documents, current generated graphs and package identities. Surviving raw
  original/substring names are explicitly reviewed, never hidden aliases.
- Publish the tested coherent eight-file set to O:/winnt, retaining recoverable
  previous files/config, verify all hashes, commit/push and leave clean main.
  Keep T open for owner acceptance.
