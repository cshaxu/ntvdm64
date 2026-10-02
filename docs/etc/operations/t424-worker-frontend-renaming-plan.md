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
| S5 | After S4 delivery, architecture/code cleanup of owner-approved audit findings 1-5: consolidate duplicate control transport, clarify shared implementation ownership, remove launcher target completion dependence, clear NTKVM dead state and obsolete tests; preserve behavior and pass the full production gate. |
| S6 | After S5 cleanup delivery, native worker NTW32 -> NTVWM. Rename product-owned component/executable/symbols and all consumers, preserving existing native text/GUI behavior and Win32Record identity; no GUI routing implementation. Pass name-only equivalence, production regression and coherent ntvwm.exe + ntkvm.exe eight-file publication. |
| S7 | After S6 naming delivery, convert interface into the bounded cross-component common static-library family, consolidate suitable project-added shared mechanisms and split NTSRV project-owned service implementation from original OpenNT carriers. Preserve mirror topology/control order and pass provenance, dependency and full production gates. |
| S8 | After S7 common/service separation delivery, native GUI routing/classification in NTVWM and service-held handles. Default launcher returns on startup success; preserve --wait/shared workers and full gates. Monitor/UNBOUND display belongs to the queue-head NTMON T candidate, not S4-S8. |
| S9 | frontend NTKVM -> NTCON after S8 delivery; owner-local names, consumers, build/test/package gates and publication. |
| S10 | final referent/semantic audit, indexes, regression and clean delivery; T closure owner-controlled. |

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

## S5 architecture/code cleanup checklist

Owner inserts audit findings 1-5 immediately after S4 delivery. This is a
planned S5, not a second active packet or an expansion of S4. Use S4's delivered
source, tested eight-file manifest and retained runtime frontiers as baseline.
Maintain an origin/current owner/target owner/consumer/disposition ledger for
each row, with exact production callers, test cases and removed-line accounting.

- [x] Consolidate ntw32-exe/channel_io.c and run16-exe/native_request_io.c,
  whose bodies match after symbol/include normalization. Keep one linked
  implementation; remove the duplicate. Preserve completed-I/O-first ordering,
  partial reads/writes, EOF, peer death, cancellation and OVERLAPPED completion
  before releasing resources. Do not substitute worker-base's stricter
  peer-death-first frontend transport merely because it looks similar.
- [x] Resolve shared transport, packet-codec and native-launch-primitive
  ownership currently under run16-exe and consumed by NTSRV/NTKVM/NTW32. Keep
  one explicit implementation owner and audited link path; retain the existing
  owned-client-library pattern where appropriate. Use admitted roots/static
  libraries, not a generic common component. interface remains declaration-only;
  worker-base remains worker-only. No duplicate forwarding provider or helper.
- [x] Remove the native-text launcher completion target-HANDLE parameter and
  local process-status gate for Console return. NTSRV must communicate enough
  authenticated result/handoff state to distinguish a completed target with
  failed final I/O from an unfinished target/infrastructure failure. NTW32
  still executes/waits on the real target; NTSRV owns receipts/results; run16
  waits broker completion and restoration only. Do not merely delete the
  guard or return early. Preserve actual exit codes, one-time consumption,
  worker-loss handling and canonical Console restoration before outer CMD
  resumes. Version any actual copied-contract change coherently and regenerate
  MIDL; unchanged GUI explicit --wait is a separate retained boundary.
- [x] Audit and remove proven write-only NTKVM session fields
  capability/restored/borrowed/admitted and redundant parameter plumbing.
  Check all current callers before removing APIs. Correct session_service.h's
  obsolete launcher-owned retire description. Preserve Console ownership,
  park/reuse, channel joins/cleanup and failure/restoration semantics.
- [x] Retire tests/app/frontend_bootstrap_test.c's removed direct-bootstrap
  calls and obsolete root registration. Map every assertion to retained broker
  bootstrap/scope fixtures before deletion; migrate missing coverage. Remove
  stale build/docs references and redundant test aliases only after preserving
  authentication, reuse, restoration and broker-loss tests.

Verification: x86 /MT CCPU40 affected closure and dependency-driven relink;
frontend-request-client-test, ntw32-execution-lifetime-test,
frontend-scope-lifetime-test, broker bootstrap and service reservation/RPC
fixtures. Add simultaneous completed-I/O/peer-death, pending cancellation,
partial transfer, EOF and handle-lifetime checks for the single transport.
Cover target completion plus final-I/O failure, unfinished worker failure,
broker loss, forged/stale receipts, nested DOS/native parent return,
owned/borrowed restoration and independent sessions. Prove production native
launcher completion/return no longer depends on a target process handle.
Retain Console17/Window17, COMMAND/MEM/EDIT, modern EDIT return and existing WOW
frontiers without weakening assertions. Apply the full production-P gate:
recoverable coherent eight-file O:/winnt publication, hashes, reviewed
commit/push and clean synchronized worktree. All five rows must close; an
unlinked shared wrapper or a new duplicate is not completion.

Non-goals: audit item 6's Console-list consolidation and broad base_service.c
split are not included. Preserve approved capture polling and backend retry
policy. No worker/frontend rename or GUI routing in S5; no original mirror,
guest/shared-lib edit, new process/component/helper, scheduler, observed graph,
authentication weakening or launch-syntax change. Naming follows as S6,
GUI routing as S8, frontend naming as S9 and final audit as S10.


## S7 common library and service source separation

Owner's latest reorder places this stage after S6 NTVWM naming.
CURRENT owns the active packet. This is planning authorization, not an
immediate source move or a claim of S7 admission/delivery. S6 naming now precedes this S7; S8-S10 follow in order: native GUI routing,
frontend NTCON naming, final audit.

- [ ] Inventory every interface declaration and candidate shared implementation
  by provenance, current owner, consumers, state/resource ownership, lock and
  failure contract. Convert src/interface into src/common with explicit
  protocol/declaration and specialist implementation submodules. Suggested
  families are protocol, transport/codec, authenticated clients and Console
  snapshot mechanics; finalize physical paths from this inventory, not from
  a generic utilities bucket. Generate RPC artifacts only under build/.
- [ ] Migrate genuinely cross-component project-added implementations into
  this common static-library family and wire all consumers. Reuse S5's single
  transport/codec implementation instead of recreating it. Link only selected
  modules: common must not drag rendering, target creation or service state
  into unrelated consumers. Preserve distinct transport ordering contracts.
  Endpoint policy, task registry, authorization decisions and retirement
  remain NTSRV-owned; frontend ownership/rendering remain frontend-owned.
  Shared authenticated client mechanics may move; the service itself may not.
  Worker-only lifecycle mechanisms remain in worker-base, which may depend
  on neutral common modules; common must not depend on worker-base or EXEs.
- [ ] Consolidate repeated GetConsoleProcessList allocation/growth/read/error/
  release mechanics when full contracts permit. Keep anchor liveness,
  same-Console joining, hidden-Console quiescence and parent-resume decisions
  in their owners. Audit capacity limits and failure behavior; do not turn
  snapshots into polling, task records, ancestry or a lifecycle authority.
- [ ] Audit ntsrv-exe/opennt/source/base_service.c and its related headers
  block-by-block against pinned original OpenNT inputs. Its path and banner
  are not provenance proof. Separate retained original/subset code, derived
  same-shaped adaptations and independently added project mechanisms.
  Publish a function/block ledger with original path/hash, classification,
  current/target location and retained-versus-removed diff accounting.
- [ ] Split project-added registration/connection, frontend admission and
  retirement, native request/receipt and management projection into bounded
  NTSRV-private modules with one explicit service-state owner and documented
  locks/borrowed resources. These are specialist service modules, not common
  policy or a second registry. Choose final filenames after dependency review;
  do not merely scatter the large file or expose all private fields publicly.
- [ ] Preserve actual original OpenNT/MVDM code in its upstream-relative mirror
  paths. Restore exact upstream bytes wherever normalized content is equal.
  Never move original DOS/WOW execution, record completion, ordering or cleanup
  into common/worker-base and reverse-call it. Project additions within mirror
  files require the same provenance review as additions elsewhere; keep only
  the smallest necessary registered owner-local hooks. Do not expand mirror
  diffs just to support the split. Preserve notices and DIVERGENCE/README rows.
- [ ] Update includes, audited source/build manifests and static-link owners;
  remove replaced files, forwarding wrappers and stale interface references
  from current production paths. Historical source/evidence identities remain
  historical. A directory/library move alone does not change the wire ABI;
  actual wire changes require paired application/RPC revision and MIDL.

Verification: source provenance ledger and pinned-upstream byte/normalized
mirror comparisons; dependency/link selection and no duplicate implementation;
x86 /MT CCPU40 affected builds plus all component links/MIDL generation.
Retain S5 transport, receipt, broker/worker loss, authentication, startup/rollback,
restore/return, owned/borrowed Console, residency, nested and isolated-session
assertions. Add snapshot growth/error tests with owner-specific policy tests.
Run Console17/Window17, modern EDIT return and existing WOW frontiers and
publish the coherent recoverable eight-file set under the production-P gate.
Report actual removed duplicates, remaining specialist differences and
per-mirror diff accounting; compile-only or wrapper-only extraction is not closure.

This explicitly admits common as the successor to interface for this bounded
cross-component library purpose, overriding the prior blanket common-root ban
only here. It is not a new executable/helper, compatibility framework,
scheduler, observer or resource-ownership policy. Do not rename workers/frontends,
add GUI routing or alter guest/shared-library code in S7.

## Owner-added native GUI stage and carried work

The owner's latest clarification keeps S4 active until its centralization
delivery. Following the owner's naming-first reorder, GUI routing belongs to
planned S8 after S6 NTVWM naming. This supersedes the premature
replanning conclusion/admission, without discarding candidate changes or exact
test evidence. S4 retains its DOS/WOW service creation, broker-only native
submission/preflight/final status/resume, obsolete path/fixture removal and
regression/publication rows. No architecture or runtime closure is claimed.

The new native GUI checklist is:

- [ ] Keep launch syntax and broad DOS/Win16/native family discovery; move
  the authoritative native GUI/CUI subsystem decision from run16 to NTVWM.
- [ ] Route native submission/startup acknowledgement only through NTSRV;
  remove run16's local GUI CreateProcess path. Classify before text-frontend
  binding so GUI-only launches do not acquire a character frontend.
- [ ] NTVWM creates the actual target and registers a restricted real process
  handle/identity against the authenticated service request. NTSRV retains
  that GUI handle after startup/launcher return, independently of worker
  occupancy; release it on actual process exit through an event wait.
- [ ] Default GUI run16 returns on successful launch acknowledgement; preserve
  explicit --wait completion/exit-code semantics through the service. Failure
  returns a structured startup error and creates no surviving GUI row.
- [ ] Release GUI worker occupancy without killing the GUI or a shared text
  worker. Owner asks for original shared-WOW semantics: release this request,
  not the resident carrier. A worker still carrying text remains BUSY;
  otherwise return READY. Later broker retirement is independent.
- [ ] Retain authoritative GUI registration/identity for the queue-head NTMON
  T candidate. Monitor/UNBOUND rendering is excluded from S4-S8; the future
  view uses service data, not local enumeration or invented Win16 process handles.
- [ ] Verify direct GUI, text -> GUI -> text, GUI -> fresh text frontend,
  process exit cleanup, authentication/isolation, launch failure, worker
  release while GUI survives and explicit --wait results. No descendant
  observation, Job tracker, helper, scheduler or new executable.

Source disposition of the owner's clarification: original `BaseSrvExitWOWTask`
in opennt-host/base/win32/server/srvvdm.c removes the matching WOWRecord, not the
shared WOW process. Win16 remains hosted by WOW throughout execution; native
GUI is an independent Windows process. Thus GUI startup releases only NTVWM
request occupancy; its real GUI handle/management record remains owned by NTSRV
until process exit. No GUI-launch-success -> worker-termination rule is added.

## Owner-added native worker name stage

Owner direction on 2026-10-02 places naming before GUI routing: S6 renames
NTW32 to NTVWM, a native Windows worker identity chosen to pair with NTVDM.
S6 changes names only and preserves existing behavior; S8 then supplies the
unified native text/GUI routing. Naming is not proof of that future capability.
The new component/product is src/ntvwm-exe / ntvwm.exe. Original OpenNT/MVDM,
Windows API names, Win32Record, kind DOS=0/Win16=1/Win32=2, and imported KVM
libraries are not renamed. No new executable role, alias or duplicate worker
is added. The accepted current NTW32 package remains current until S6 delivery.

S6 audits all product-owned NTW32 referents in paths, C symbols/guards,
consumers, build targets, tests, configuration, launch/package identities and
documentation. Classify historical evidence and original/fixed identities
before substitution; preserve recorded hashes/results and source provenance.
Use the existing worker-base and interface paths, not a parallel implementation.
Verify actual wire/endpoint compatibility effects; a spelling change alone
does not authorize an ABI change. Publish a coherent recovered/tested eight-file
package and remove the old basename only after its recoverable backup.

S9 frontend naming and S10 audit remain sequential and cannot claim S8 runtime
work. This insertion changes the plan only; S4 remains the sole active packet.

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
