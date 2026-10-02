# T424 ordered worker/frontend name migration

Owner closed T423 and admitted the former queue-head package on 2026-10-02.
Only CURRENT owns the active S. The [proposal](../../proposals/proposal-native-worker-frontend-renaming-001.md)
retains the admitted scope and full gates; this record makes the ordered
execution stages explicit, not a second status authority.

Baseline: accepted T423 S40 `f64559086`, protocol/RPC 28, x86 /MT CCPU40,
eight-file O:/winnt package; see [closure](../../history/m0-t423-console-window-runtime-closure.md)
and [final evidence](../evidence/m0-t423-s40-native-alternate-screen-geometry.md).

| Stage | Deliverable and next-stage gate |
| --- | --- |
| S1 | Read-only referent inventory for source, symbols, paths, build, tests, package, endpoints and all documentation/archive; capture baseline hashes and version, classify original/library/fixed names and record an ordered migration map. No production rename. |
| S2 | Rename the former native worker and project-owned native records to NTW32 and Win32Record. Update all required code/docs/paths; prove zero old project-worker referents, preserving original Console identities/substrings per S1. Pass production regressions and publish coherent intermediate ntw32.exe + ntkvm.exe eight-file package. |
| S3 | Owner-added lifecycle investigation and subsequently approved repair: centralize orderly frontend/worker retirement in NTSRV, give authenticated close instructions priority over lease/pending/I/O, retain bounded startup admission and verify repeated launch, failure and session isolation before publication. |
| S4 | Owner-added broker-centered creation/control migration: NTSRV creates/binds frontend and workers and coordinates Console takeover/return; run16 only submits/waits on NTSRV; NTKVM performs actual Console operations; worker/frontend direct channel is I/O only. Remove displaced direct launcher/frontend/worker control paths; preserve mirror semantics and pass the full production gate. |
| S5 | Owner-added native GUI routing: finish undelivered S4 centralization rows, then route every native request through run16 -> NTSRV -> NTW32. NTW32 decides GUI/text; NTSRV retains actual GUI handles and exposes UNBOUND rows; default run16 returns on authenticated startup success, not window exit. Pass shared-worker, --wait, failure/exit cleanup and complete production gates. |
| S6 | Former S5: frontend NTKVM -> NTCON after S5 delivery; owner-local names, consumers, build/test/package gates and publication. |
| S7 | Former S6: final referent/semantic audit, indexes, regression and clean delivery; T closure owner-controlled. |

Rename only product-owned identities, preserving original OpenNT/MVDM and
generic imported KVM library source identities. No guest, extra process/helper,
observed-task graph or scheduler is admitted. The subsequent owner-approved
S3 retirement and S4 broker-centered creation/control changes are explicit
exceptions to the initial name-only launch/completion/lifetime exclusion.
Local spelling alone does not authorize an ABI bump; any actual copied-wire,
RPC/endpoint/version compatibility effect must be proved and coherently managed.
An externally fixed spelling requires owner review before production migration.

Owner direction on 2026-10-02 adds S3 investigation before further renaming.
The subsequent explicit owner approval admits the S3 lifecycle repair separately
from accepted name-only S2. No-worker roots retire on the broker's instruction,
without an idle prerequisite; a live authenticated startup owner receives only
a ten-second admission deadline. NTVDM/NTW32 obey broker close events instead of
making frontend-death retirement decisions. NTSRV cannot enter its ten-second
empty grace while a frontend/worker or legal admission remains registered.

The owner's subsequent architecture approval adds S4 before renaming. Its
[source-edge ledger](../evidence/m0-t424-s4-broker-centered-launch-control.md)
records current versus target edges, reusable mechanisms, ordered production
migration and exact acceptance. NTKVM waits indefinitely for broker work;
finite startup/workerless deadlines belong to NTSRV, never an NTKVM timer.
The subsequent Console refinement removes the direct launcher/frontend IPC
exception: NTSRV authenticates/coordinates takeover and restoration acknowledgements,
while NTKVM performs AttachConsole against the actual caller, not NTSRV. NTSRV
creates the frontend but does not acquire its Console or relay user I/O. Both workers
report real direct results to NTSRV; native GUI and Win16 startup-only semantics
remain unchanged. No mirror edit is planned or authorized by this S.

## Owner-added native GUI stage and carried work

Owner requests S4 conclusion and adds S5. S4 is bounded replanning, not a
delivered implementation P. Preserve all candidate changes and exact test
evidence. S5 first completes its still-open DOS/WOW service creation, broker-only
native submission/preflight/final status/resume, obsolete path/fixture removal
and regression/publication rows. This disposition is not a claim that the
broker-centered architecture is already usable or published.

The new native GUI checklist is:

- [ ] Keep launch syntax and broad DOS/Win16/native family discovery; move
  the authoritative native GUI/CUI subsystem decision from run16 to NTW32.
- [ ] Route native submission/startup acknowledgement only through NTSRV;
  remove run16's local GUI CreateProcess path. Classify before text-frontend
  binding so GUI-only launches do not acquire a character frontend.
- [ ] NTW32 creates the actual target and registers a restricted real process
  handle/identity against the authenticated service request. NTSRV retains
  that GUI handle after startup/launcher return, independently of worker
  occupancy; release it on actual process exit through an event wait.
- [ ] Default GUI run16 returns on successful launch acknowledgement; preserve
  explicit --wait completion/exit-code semantics through the service. Failure
  returns a structured startup error and creates no surviving UNBOUND row.
- [ ] Release GUI worker occupancy without killing the GUI or a shared text
  worker. Whether an otherwise dedicated carrier is retired after release is
  a separate broker policy; owner clarification is pending. Never force-kill
  a reused worker carrying the parent CMD to interpret “release”.
- [ ] Add a service-only UNBOUND snapshot projection and NTMON section. Current
  monitor has no independent UNBOUND section. Audit Win16 task identity and
  its existing startup registration; do not pretend each Win16 task has an
  independent host process handle. Keep kind 0/1/2 and existing three hotkeys.
- [ ] Verify direct GUI, text -> GUI -> text, GUI -> fresh text frontend,
  process exit cleanup, authentication/isolation, launch failure, worker
  release while GUI survives and explicit --wait results. No descendant
  observation, Job tracker, helper, scheduler or new executable.

S6 naming and S7 audit remain sequential and cannot claim S5 runtime work.

The [S1 name audit](../evidence/m0-t424-s1-name-referent-audit.md) supplies the
complete-tree inventory, exact original Console exclusions and baseline hashes.
The application version advances once to 0.0.424; unchanged wire layout keeps
protocol/RPC 28. Original OpenNT Console-source paths are not product aliases.

Each production-code P retains x86 build, focused identity/protocol/lifecycle
tests, COMMAND/MEM/EDIT direct/nested return, native CMD/modern EDIT including
S40's actual-return regression, Console17 + Window17 and existing WOW frontiers,
coherent O:/winnt publication with prior recovery set, commit/push and clean tree.
Document-only admission/audit P runs governance/link/diff checks without
claiming a fresh runtime pass or changing the accepted publication.
