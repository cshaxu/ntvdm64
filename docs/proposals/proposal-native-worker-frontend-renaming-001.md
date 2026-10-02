# Proposal — Rename the native-text worker and Console frontend

## Status and objective

Owner closed the predecessor and admitted this former queue-head candidate on
2026-10-02. Only [CURRENT](../states/CURRENT.md) owns admission and publication;
the [working plan](../etc/operations/t424-worker-frontend-renaming-plan.md)
owns the ordered stages. The [S1 audit](../etc/evidence/m0-t424-s1-name-referent-audit.md)
pins pre-migration source identities in Git, inventories and hashes.
Names below are migration-normalized; they do not claim runtime acceptance.

| Role | S2 intermediate owner/product | Final owner/product |
| --- | --- | --- |
| Resident Win32 text worker, owns hidden Console and direct native targets | `src/ntw32-exe` / `ntw32.exe` | Same NTW32 worker identity. |
| Visible Console/Window frontend, owns display/input/presentation routing | `src/ntkvm-exe` / `ntkvm.exe` | S6 reserved frontend identity: `src/ntcon-exe` / `ntcon.exe`. |

The final eight files are run16.exe, ntsrv.exe, ntvdm.exe, ntw32.exe,
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
| S5 | After S4 delivery, native GUI routing/classification and service-held handles; default launcher returns on startup success. Preserve --wait/shared workers. Monitor/UNBOUND display belongs to the queue-head NTMON T candidate. |
| S6 | Former S5: frontend NTCON migration after S5 delivery; build/test/wiring/name checks and verified final eight-file publication. |
| S7 | Former S6: final ownership/semantic-diff and naming audit, indexes/authorities, clean committed/pushed delivery; owner decides T acceptance. |

Owner direction on 2026-10-02 adds lifecycle investigation before further
renaming. Earlier S3/S4 migration labels in this proposal now refer to S6/S7
after the subsequent owner-approved architecture and native-GUI insertions.
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

## Subsequent owner-approved native GUI boundary

Win32 GUI is no longer a local run16 CreateProcess exception in the target
architecture: native requests go run16 -> NTSRV -> NTW32, and NTW32 owns the
authoritative GUI/text classification before text frontend binding. This
changes routing, not default GUI startup-only or explicit --wait semantics.
NTW32 reports successful actual creation/binding/start to NTSRV; the service
can then return the default launcher's startup result. NTSRV retains the
authenticated GUI process handle after the worker releases this request and
after run16 returns; it removes registration on actual process exit. The owner's
final clarification assigns monitor/UNBOUND display to the queue-head
[NTMON candidate](proposal-ntmon-console-worker-tree-001.md), not S4/S5.
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
the premature replanning conclusion and S5 admission. Its open rows remain S4
release requirements, listed in the working plan and CURRENT. S5 starts only
after S4 delivery. No candidate publication or production P is claimed by these
planning changes.

An externally fixed project identifier or imported-original conflict requires
review, not an alias. Original Console names already classified by S1 are
outside the requested product rename. Documentation-only P uses governance,
link and diff gates; every production P uses the complete runtime gate.

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
