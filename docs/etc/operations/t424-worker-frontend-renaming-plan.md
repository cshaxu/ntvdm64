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
| S7 | After S6 naming delivery, convert interface into common and unify service control RPC plus direct worker I/O pipes. Consolidate suitable shared mechanisms; pass affected provenance/dependency and full production gates. Service source split is explicitly transferred to S8, not claimed completed. |
| S8 | Owner-separated NTSRV provenance review and service-private source split. Reuse S7 inventory and implementation evidence, retain one state/lock authority and original mirror semantics; build, regress and publish. |
| S9 | Native GUI routing through NTVWM and service-held handles. Keep launcher classification before service admission, matching DOS/WOW order. Default launcher returns on startup success; preserve --wait/shared workers and full gates. Monitor/UNBOUND display belongs to the queue-head NTMON T candidate. |
| S10 | Owner-added rapid relaunch/lost-wakeup repair after S9 delivery. Centralize NTSRV frontend pending-work notification under its existing lock; deterministic interleaving tests and rapid CMD/COMMAND reuse, completion/Console-return and isolation regressions before coherent publication. |
| S11 | Frontend NTKVM -> NTCON after S10 repair delivery; owner-local names, consumers, build/test/package gates and publication. |
| S12 | Owner-approved unified frontend logical_surface for both NTVDM and NTVWM, fixed upper-left presentation and symmetric DOS/native text handoff. Execute after S11 delivery; remove replaced bypasses, verify and publish the coherent package. |
| S13 | Former S12 final referent/semantic audit, indexes, regression and clean delivery; T closure owner-controlled. |

Rename only product-owned identities, preserving original OpenNT/MVDM and
generic imported KVM library source identities. No guest, extra process/helper,
observed-task graph or scheduler is admitted. The subsequent owner-approved
S3 retirement and S4 broker-centered creation/control changes are explicit
exceptions to the initial name-only launch/completion/lifetime exclusion.
S12 is an additional bounded text-storage/presentation and handoff exception,
not permission to change original worker execution or lifecycle semantics.
Local spelling alone does not authorize an ABI bump; any actual copied-wire,
RPC/endpoint/version compatibility effect must be proved and coherently managed.
An externally fixed spelling requires owner review before production migration.

Owner direction on 2026-10-02 inserts the lost-wakeup repair as S10 and shifts
the former frontend naming/final audit S10/S11 to S11/S12. The table above is
the current sequence; earlier numbering/replanning chronology below remains
historical. The subsequent owner approval inserts unified text storage/handoff
as S12 and moves final audit to S13. S12 is approved for sequential execution
after S11 delivery; CURRENT alone records the active packet. This planning
update does not interrupt S10, admit a second active packet or claim runtime
implementation/acceptance.

## S12 unified logical surface and DOS/native handoff

After S11, NTCON is the frontend formerly named NTKVM; NTVWM is the native
worker. The frontend owns logical_surface. NTVWM still owns its hidden
execution Console and actual native targets. This is project-added frontend
adaptation, not original OpenNT storage or a Windows-compatibility exception.
Use the delivered S11 package as the regression baseline.

### Production closure checklist

- [x] Audit provenance and both text routes before extraction. Rename and
  generalize dos_surface into logical_surface; route NTVDM operations/frames
  and NTVWM frames through the same storage and presentation path. Remove
  replaced native direct-to-visible-Console writes and text-render bypasses;
  a field rename or unused wrapper is not completion. Leave original
  OpenNT/MVDM execution and mirror algorithms at their existing owners.
- [x] Store complete logical buffer cells/attributes, buffer extent, logical
  viewport origin/extent, buffer-relative cursor, shape/visibility and the
  existing style/font/palette metadata. Host dimensions never truncate this
  storage. Normalize buffer/viewport coordinates once: an 80x30 buffer with
  viewport rows 2..29 and cursor row 29 presents 80x28 with local cursor row 27.
- [x] Publish a consistent snapshot under explicit instance state and locks.
  Per-operation locks alone must not expose mixed geometry, row tiles and
  cursor from different native publications. Define commit/failure behavior
  using existing channel barriers/generations where sufficient; no generic
  scheduler or second task registry. Rendering reads a consistent committed
  snapshot; failed/partial/stale publication cannot replace it.
- [x] On a logical dimension change, attempt the appropriate physical resize,
  then query the actual canvas. Resize rejection or implicit host reflow must
  not mutate logical content. Do not classify Terminal tabs or depend on host
  product names. Window pixel sizing/scaling is separate from text geometry;
  all text remains text rendering, not a large-frame bitmap fallback.
- [x] For Console presentation, map the logical viewport's upper-left to the
  actual canvas upper-left. Paint the width/height intersection; clear all
  unused visible right/bottom cells and attributes. Never bottom-align, pan,
  reflow or apply row compensation. Smaller canvases clip presentation only;
  larger canvases reveal preserved content. Hide an out-of-intersection
  cursor rather than clamping it or moving the host viewport. Map mouse
  input through this same intersection; blank margins are not guest cells.
  Window uses the same logical snapshot and existing text renderer semantics.
- [x] Implement the same handoff order in both directions: old owner drains
  and submits final grid/geometry/cursor; frontend acknowledges that snapshot;
  convert only for the incoming worker's capabilities; incoming worker applies
  and acknowledges the actual state; only then release execution/input or
  resumed-parent output. Preserve input order/return and existing I/O barriers.
  Old-owner late output must not overwrite the new owner's state.
- [x] Seed workers from the current logical state, never the initial outer CMD
  snapshot or physical host size. Native applications may explicitly change
  geometry; pass that current geometry into the next handoff. DOS keeps the
  original height mapping: <=23 -> 22, 24..26 -> 25, 27..35 -> 28,
  36..46 -> 43, >=47 -> 50. Other capability conversions require audited,
  deterministic worker-supported rules, not host-driven heuristics.
- [x] Keep original cell-grid resize semantics: no paragraph reflow; growth
  keeps existing rows and appends blanks; shrink shifts the cursor-containing
  tail only when the old cursor is outside the new height, transforming cursor
  consistently. Ordinary DOS release retains current geometry/grid/cursor;
  do not restore launch-time CMD geometry/cursor/page. Stream-I/O-specific
  restoration remains special. DOS graphics stays on its existing graphics
  route, not logical_surface. Host scrollback history is not promised.
- [x] Preserve NTSRV registration/binding/lifecycle/results, thin run16 and
  direct worker/frontend I/O. Frontend storage/rendering stays frontend-local;
  suitable common transport/validation and worker-base client mechanics remain
  shared. No helper, executable, component, Job/Observed graph, scheduler,
  guest/mirror/shared-lib change or launch-syntax change. Actual wire changes
  require paired application/IDL versions, MIDL regeneration and mismatch
  negatives; local storage changes alone do not require a version bump.

### Required evidence and release

- [x] Test wider/taller, narrower/taller, wider/shorter and both-smaller canvases,
  accepted/refused/unapplied physical resize, nonzero viewport origin, edge
  cursors hidden/reappearing, blank-margin characters/attributes and mouse
  coordinates. Prove clipping does not alter logical data or move the viewport.
- [x] Test complete/partial/failed publication, concurrent render/capture,
  stale-owner output, disconnect and both handoff acknowledgements. Assert the
  resumed parent cannot output before final-state application is confirmed.
- [x] Exercise DOS -> native -> DOS and native -> DOS -> native, nested and
  repeated COMMAND/CMD DIR/MEM, modern EDIT/EDIT.COM full-screen redraw,
  program-requested sizes, Console/Window switches and independent sessions.
  Check coherent grid, geometry, prompt and cursor at every ownership boundary.
- [x] Build affected x86 targets; retain lifecycle, handles, native completion,
  DOS resume, Console17 + Window17 and existing WOW frontier assertions. Record
  exact commands/results, remaining limits and eliminated duplicate paths.
  Publish/hash-check the validated eight-file package to O:/winnt, commit/push
  under execution rules and hand off for owner side testing. Only then S13
  final audit may begin; T closure remains owner-controlled.

The [S12 ledger](../evidence/m0-t424-s12-logical-surface.md) records production
delivery, actual test commands, failures and release identities. Physical
resize requests are grow-only; an unapplied/refused request uses actual-canvas
projection without logical mutation. Desktop/RDP manual behavior remains an
owner check, and the single unreproduced management-close failure is explicitly
retained rather than claimed repaired. Neither limits original grid semantics.

## S13 final audit delivery

The [S13 ledger](../evidence/m0-t424-s13-final-audit.md) maps every admitted
stage to its delivery evidence and audits the actual current source/build/
publication. Original executable mirror algorithms, immutable media and KVM
library inputs are unchanged. Obsolete product aliases are absent; historical
evidence filenames and README-only move markers remain classified. Current
design/source documentation reflects delivered common/service/frontend
ownership, not the superseded candidate topology. This is S delivery only:
CURRENT retains T424 as open for the owner's final product audit.

## S10 rapid relaunch and frontend notification checklist

Owner additionally approves the causally reproduced DOS/native snapshot-origin
repair within S10. Keep the logical text dimensions and current cell grid/cursor,
but express native seed geometry in canonical-buffer coordinates under the
existing ownership/snapshot lock. No VT disabling, row compensation, old CMD
snapshot restore, new transport or original mirror change.

Owner reproducer: rapidly repeat run16 cmd -> exit -> run16 cmd -> exit in
one outer CMD. The read-only source audit of the latest S9 worktree finds
the previously identified reset unchanged in NTSRV's project-owned
opennt/source/frontend_registry.c. AcquireFrontendRoot publishes an
undecided frontend_join_caller and sets frontend_capability under service->lock;
service_frontend_idle checks only frontend_request_root before resetting the
same event. NTKVM checks join candidates before checking channel requests in
session_service.c. A join inserted between those checks can lose its wakeup,
leaving NTKVM asleep and the launcher's acquisition waiting until its existing
ten-second deadline. This is a source-proven possible interleaving, not an
observed reproduction or proof of every reported hang's cause.

Audit method: inspect every SetEvent/ResetEvent and adjacent predicate/wait
in NTSRV frontend/worker/native/lifecycle modules, NTKVM session/presentation/
input queues, NTVWM admission/execution and common transport. The existing
verify-frontend-relaunch.ps1 delays each line by 1600 ms and does not cover
rapid consecutive interactive native launches. Record exact tested source and
artifact identities in S10's implementation evidence rather than treating
the earlier probe as acceptance of this race.

- [x] Reconfirm the failing interleaving against the delivered S9 baseline;
  distinguish startup acquisition, direct completion and Console-return waits.
  Keep the reported hang open until causal evidence classifies it.
- [x] Give the existing shared frontend notification one NTSRV-private
  pending-work predicate/update mechanism under service->lock. Include
  undecided joins and all actionable pending channel/route delivery work;
  reset only when all work represented by this event is absent. Keep state
  authoritative and the event merely its wakeup projection.
- [x] Apply that mechanism to join creation/decision/cancellation, channel
  request/delivery, route cleanup and connection rundown. Remove displaced
  partial checks and scattered maintenance, not just add a wrapper beside them.
  Define cancellation, event-operation failure and resource ownership explicitly.
- [x] Prevent busy loops: a decided join awaiting lease availability is not
  an undecided authentication job. Preserve its existing frontend_changed
  condition-variable predicate/recheck and finite admission deadline.
- [x] Retain separate retire/restored lease acknowledgements, direct receipts,
  private ownership waiters, input readiness and per-operation OVERLAPPED events.
  Recheck adjacent reset/wait contracts; repair another site only if a causal
  defect of the same class is proved, otherwise document why it stays independent.
- [x] Add a deterministic production-service fixture: insert a join after
  NTKVM's empty join check but before its channel-empty check, then assert the
  event remains signaled and the join is processed without an unrelated wakeup.
  Cover simultaneous join/channel work, refusal/cancellation/disconnect,
  pending native routes and the last-item-consumed reset without event spinning.
- [x] Exercise rapid same-Console interactive CMD exit/relaunch and COMMAND/
  native alternation, resident reuse, concurrent launchers, independent Console
  isolation, broker/worker loss and restoration-before-outer-CMD-return. Record
  which phase any timeout blocks; do not use Sleep to make the race disappear.
- [x] Pass affected x86 /MT CCPU40 builds, existing RPC/reservation/receipt/
  lifecycle gates, Console17/Window17 and retained WOW frontiers; publish the
  coherent recoverable eight-file set to O:/winnt with hashes, reviewed P
  commit/push and clean synchronized tree. T closure remains owner-controlled.

Bounds: repair project-added NTSRV frontend notification and approved NTKVM
native snapshot coordinates, not
original OpenNT/MVDM execution. No mirror/guest/shared-lib modification,
new process/component/helper/protocol, scheduler, Observed records, polling
cleanup, arbitrary retry/delay or frontend rename. No planned wire change;
if one becomes essential, re-review scope and synchronize protocol/RPC/MIDL.
Queue order is unchanged. Before implementation, admit the bounded S10 packet
in CURRENT only after S9 delivery; this checklist is planned work, not closure.

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

Owner revision: service source separation is now the next S8, not an S7 exit
requirement. Retain this heading as an existing link anchor. S7 closes only
common organization and two-protocol unification after its retained production
gate. Previous S8-S10 become S9-S11. Existing research and candidate changes
are preserved; transfer is not functional completion.

Owner clarification: this stage also unifies communication into at most two
message protocol families: authenticated NTSRV service RPC for all control,
and the same direct NTKVM-worker I/O protocol for both workers. Remove the
extra NTSRV-NTVWM native-control pipe and NTSRV-NTKVM bootstrap-ack pipe;
moving either old pipe implementation into common does not meet this goal.
Typed capability/event/process attachments remain resources, not additional
message protocols. Original DOS/WOW APIs and scheduling remain unchanged.

Latest owner clarification: common carries both protocol families and their
bounded shared client/transport mechanisms for all appropriate consumers.
NTSRV control uses RPC; direct NTKVM-worker I/O retains named pipes. The
briefly discussed I/O-to-RPC migration is withdrawn. Neither protocol depends
on or relays through the other. Worker-base may depend on common, never the
reverse. Endpoint policy, registry, execution and rendering stay local.

Owner clarification on shared mechanics: common owns a bounded pipe-transfer
primitive covering exact/partial transfers, cancellation and draining,
disconnect, checked lengths and explicit resource release. Select completion-
first or peer-death-first ordering explicitly where the owner contract differs.
Share this mechanism across surviving I/O clients/services; do not preserve
obsolete control messages or pipes merely to provide another consumer. Business
state, frame interpretation, handle ownership and task policy remain local.

- [x] Replace native GetNext's control-pipe attachment with copied bounded
  command data and authenticated typed resources delivered by service RPC.
  Use NTSRV's existing direct record for launch outcome and final I/O status;
  retain real target completion, nested/resume behavior, failure rollback and
  exactly-once receipt consumption. No second task registry or scheduler.
- [x] Replace the frontend bootstrap reply pipe with an authenticated RPC
  startup outcome for the exact admitted process/capability. Preserve the
  ten-second admission deadline, early-death handling, startup rollback,
  Console acquisition and existing restoration acknowledgement barrier.
- [x] Remove superseded pipe headers, transfers, handle exports, validators,
  production links and obsolete fixtures after equivalent boundary tests.
  Retain only worker-frontend I/O pipe creation/exchanges. Version real RPC
  changes in both APP_PROTOCOL_VERSION and IDL major and regenerate MIDL.

Owner's latest reorder places this stage after S6 NTVWM naming.
CURRENT owns the active packet. This is planning authorization, not an
immediate source move or a claim of S7 admission/delivery. The latest owner
revision places service source separation at S8; S9-S11 follow in order:
native GUI routing, frontend NTCON naming, final audit.

- [x] Inventory every interface declaration and candidate shared implementation
  by provenance, current owner, consumers, state/resource ownership, lock and
  failure contract. Convert src/interface into src/common with explicit
  protocol/declaration and specialist implementation submodules. Suggested
  families are protocol, transport/codec, authenticated clients and Console
  snapshot mechanics; finalize physical paths from this inventory, not from
  a generic utilities bucket. Generate RPC artifacts only under build/.
- [x] Migrate genuinely cross-component project-added implementations into
  this common static-library family and wire all consumers. Reuse S5's single
  surviving codec/client implementations instead of recreating them. Do not
  preserve obsolete control-pipe messages as a common protocol. Recover usable
  byte-transfer mechanics into one common primitive with explicit wait ordering.
  Link only selected
  modules: common must not drag rendering, target creation or service state
  into unrelated consumers. Preserve distinct transport ordering contracts.
  Endpoint policy, task registry, authorization decisions and retirement
  remain NTSRV-owned; frontend ownership/rendering remain frontend-owned.
  Shared authenticated client mechanics may move; the service itself may not.
  Worker-only lifecycle mechanisms remain in worker-base, which may depend
  on neutral common modules; common must not depend on worker-base or EXEs.
- [x] Consolidate repeated GetConsoleProcessList allocation/growth/read/error/
  release mechanics when full contracts permit. Keep anchor liveness,
  same-Console joining, hidden-Console quiescence and parent-resume decisions
  in their owners. Audit capacity limits and failure behavior; do not turn
  snapshots into polling, task records, ancestry or a lifecycle authority.
- [x] **S8 implementation:** Audit ntsrv-exe/opennt/source/base_service.c and its related headers
  block-by-block against pinned original OpenNT inputs. Its path and banner
  are not provenance proof. Separate retained original/subset code, derived
  same-shaped adaptations and independently added project mechanisms.
  Publish a function/block ledger with original path/hash, classification,
  current/target location and retained-versus-removed diff accounting.
- [x] **S8 implementation:** Split project-added registration/connection, frontend admission and
  retirement, native request/receipt and management projection into bounded
  NTSRV-private modules with one explicit service-state owner and documented
  locks/borrowed resources. These are specialist service modules, not common
  policy or a second registry. Choose final filenames after dependency review;
  do not merely scatter the large file or expose all private fields publicly.
  The [complete S8 ledger](../evidence/m0-t424-s8-ntsrv-service-separation.md)
  records 113 unchanged definitions moved to service_core, worker_registry,
  frontend_registry, native_commands, lifecycle and management; 24 original-
  interface/resource adapters remain in base_service. Existing state/lock and
  public contracts are unchanged. Delivery status remains CURRENT-owned.
- [x] Preserve actual original OpenNT/MVDM code in its upstream-relative mirror
  paths. Restore exact upstream bytes wherever normalized content is equal.
  Never move original DOS/WOW execution, record completion, ordering or cleanup
  into common/worker-base and reverse-call it. Project additions within mirror
  files require the same provenance review as additions elsewhere; keep only
  the smallest necessary registered owner-local hooks. Do not expand mirror
  diffs just to support the split. Preserve notices and DIVERGENCE/README rows.
- [x] Update includes, audited source/build manifests and static-link owners;
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
planned S9 after S8 service separation. This supersedes the premature
replanning conclusion/admission, without discarding candidate changes or exact
test evidence. S4 retains its DOS/WOW service creation, broker-only native
submission/preflight/final status/resume, obsolete path/fixture removal and
regression/publication rows. No architecture or runtime closure is claimed.

The native GUI checklist is delivered by S9; exact results and limits are in
the [S9 ledger](../evidence/m0-t424-s9-native-gui-routing.md). This does not
claim the later NTMON view or the separately inserted S10 race repair.

- [x] Keep launch syntax and DOS/Win16/native discovery. Native GUI/CUI
  classification remains in run16 before service admission, matching the
  existing DOS/WOW classification -> BaseCheckVDM -> worker order. NTVWM
  executes the admitted kind; do not introduce worker preflight/resubmission.
- [x] Route native submission/startup acknowledgement only through NTSRV;
  remove run16's local GUI CreateProcess path. Classify before text-frontend
  binding so GUI-only launches do not acquire a character frontend.
- [x] NTVWM creates the actual target and registers a restricted real process
  handle/identity against the authenticated service request. NTSRV retains
  that GUI handle after startup/launcher return, independently of worker
  occupancy; release it on actual process exit through an event wait.
- [x] Default GUI run16 returns on successful launch acknowledgement; preserve
  explicit --wait completion/exit-code semantics through the service. Failure
  returns a structured startup error and creates no surviving GUI row.
- [x] Release GUI worker occupancy without killing the GUI or a shared text
  worker. Owner asks for original shared-WOW semantics: release this request,
  not the resident carrier. A worker still carrying text remains BUSY;
  otherwise return READY. Later broker retirement is independent.
- [x] Retain authoritative GUI registration/identity for the queue-head NTMON
  T candidate. Monitor/UNBOUND rendering is excluded from S4-S8; the future
  view uses service data, not local enumeration or invented Win16 process handles.
- [x] Verify direct GUI, text -> GUI -> text, GUI -> fresh text frontend,
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
S6 changes names only and preserves existing behavior; S9 then supplies the
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

S11 frontend naming, S12 logical-surface/handoff work and S13 audit follow the new S10 repair and cannot claim S9 runtime
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
