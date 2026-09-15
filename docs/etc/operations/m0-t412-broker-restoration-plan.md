# T412 broker restoration working plan

## Authority and scope

Owner admission on 2026-09-13 covers the queue-head broker package and asks
for S decomposition first. [Status](../../states/CURRENT.md) alone owns the
active packet. S1 design is closed by the
[consolidated review](../evidence/m0-t412-s1-design-closure.md); S2 reached its
source-recovery conclusion and S3 is active. S4--S6 remain sequential planned
work. The [single broker proposal](../../proposals/proposal-cross-process-broker-closure-001.md)
retains the detailed requirements; this plan supersedes its provisional stage
grouping without creating another proposal.

The owner subsequently approved completing the whole T according to this
proposal, including the finite OS-managed resource-attachment exception. S1--S6
advance sequentially on evidence, without repeated routine approval. Current
S1 feasibility outputs use build/M0-T412/S1/rpc-resource; they are test-only
transport evidence, never a substitute for the original BaseSrv service owner.

Restore D01--D07/D10 as one original BaseSrv/BaseClient lifecycle: command
selection, PIF, first-VDM state, current directories, exit, reentry, client
capture/wait/retry and image classification. Include CheckVDM, UpdateVDMEntry,
registration, dispatch, rollback and release, not isolated function bodies.
Original service policy remains in opennt-host; original MVDM behavior stays
in mvdm-host. Adapters/broker bind finite unavailable interfaces, not duplicate
those state machines. App owns composition.

## Sequential packets

| S | Work | Exit evidence |
| --- | --- | --- |
| S1 — Baseline audit and design | Freeze source/artifact/provider lineage; inventory scoped mirror diffs, overlays and autonomous policy against original owners. Design process state, wire/resource contracts, Console identity and launch reservation. | D01--D07/D10 disposition ledger; measured removable and retained lines by category; finite boundary and test matrix; prerequisite disposition; executable naming/deployment migration design. Report before implementation. |
| S2 — Original BaseSrv/BaseClient recovery | Restore the coherent selected original lifecycle including capacity, ownership, wait/retry, errors and cleanup; finite same-shaped bindings only. | Original selected bodies compile/link on x86 and pass focused positive/negative tests. A fixture duplicating provider policy is not proof. Track transitional harnesses for integration/removal. |
| S3 — Three programs and IPC | Compose run16.exe, basesrv.exe and ntvdm.exe; authenticate endpoint, reserve launch, let launcher create/register requested worker, dispatch and clean disconnects. Preserve original worker entry and consistent OS/SoftPC command-line facts. | Formal three-program build/provider maps; concurrent startup/registration and failure tests; copied versioned messages and explicit resource ownership. Update publication rules before deploying renamed products. |
| S4 — DOS execution and streams | Connect BOP 54:08 to run16, original DOS reentry/native-child waits, and asynchronous command/environment/stream capture. | Real COMMAND/MEM/EDIT and DOS/native-child runs; spaces, environment, stdin/stdout/stderr, EOF, exit codes, rollback and disconnect. Explicit direct CLI versus guest COMMAND redirection tests; no invented generic cmd parser. |
| S5 — WOW coordination and broker lifecycle | Restore broker-side WOW acquisition/PIF/notification/waits/failures; singleton and empty-broker exit. Worker residence and exit remain the original OpenNT worker/COMMAND/Console lifecycle. | Source-owned coordination tests and reachable real-worker evidence; simultaneous startup, reconnect, broker drain/arrival, restart/first-VDM and exact-once cleanup. No worker timeout or reaper exists. |
| S6 — Integrated verification and removal | Build/test final composition, remove superseded autonomous policy and obsolete bridges, reconcile registers and publish verified products. | Final per-row disposition; measured mirror diff/overlay/autonomous line deltas; real-program and process-race matrix; no mandatory unproved contract; clean committed/pushed worktree and indexed closure evidence. |

## Detailed execution

The detailed sequence below is an execution specification, not a claim that
the inventory or designs have already been verified. Work groups are deliberately
package-sized; they are not new numeric S tasks or per-symbol admission gates.

## S1 detailed procedure — establish the recovery contract

1. Freeze the entry revision, selected upstream provenance, formal build
   manifests, deployed executable hash and available regression evidence.
   Separate historically observed results from freshly reproduced results.
   Any later diagnostic run records its exact source/artifact identity.
2. Trace each of D01--D07/D10 from real caller through the selected provider
   to its outgoing boundary. Include compiled-out alternatives and orphaned
   duplicates, but distinguish them from active product code. Inspect matching
   mirror overlays, adapters, app and broker/session code together.
3. Produce one row per behavior group, recording original source path/function,
   current file/provider, source/build selection, changed control/data/failure
   semantics, four-rung recovery disposition, replacement prerequisites and
   the exact tests needed to remove the current implementation safely.
4. Design the whole launch and return sequence: classification -> service
   selection -> launch reservation when needed -> worker creation/registration
   -> command delivery -> completion/reentry -> resource release or reuse.
   Every transition records caller, original policy owner, finite binding,
   precondition, payload/resource owner, failure rollback and observable result.
5. Resolve Console compatibility and worker eligibility before implementing
   reuse. An unrelated run16 invocation must not inject a command into a busy
   interactive COMMAND merely because both share a Console. Reuse follows
   original source-proven selection and guest readiness, not a generic pool.
6. Specify security, transport, resource transfer, idle/shutdown and public
   entry contracts. Record concrete designs and rejected alternatives with
   reasons. No wire opcode/layout or platform mechanism is final merely because
   it appears convenient in this planning document.
7. Review existing worker prerequisite evidence against the actual provider:
   thread binding, wait/alert, guest return, termination and reset. Separate a
   missing test from a source-proven defect. Name the dependency and disposition
   before advancing work that relies on it; do not restart a global CPU audit.
8. Report removal estimates separately for mirrored changed lines, overlays,
   duplicate autonomous behavior and retained modern mechanics. State counting
   commands, baseline, generated/vendor exclusions and overlapping categories.
   Do not add overlapping totals or count relocated code twice.

S1 delivers an indexed audit/design record containing that ledger, source-backed
transition table, product-entry/deployment decision, finite ABI design and test
matrix. It does not deliver a new runtime implementation. Unresolved mandatory
design choices remain explicit and prevent claiming the design is complete.

## S2 detailed procedure — restore source policy in coherent groups

1. Restore service admission and ownership together: CheckVDM, selected
   DOS/WOW records, UpdateVDMEntry, worker association, failure rollback and
   release. Keep original record semantics and ordering; represent only missing
   host resources through bounded package-private bindings.
2. Restore command and client behavior together: GetNextVDMCommand, capacity
   negotiation, PIF, first-VDM state, current-directory exchange and the original
   BaseClient capture/copy/wait/retry flow. Include short-buffer retries without
   consuming commands, correct retry exit-code handling and source-defined
   consume-on-get behavior. Classification D10 belongs to the original owner,
   not a second image parser in run16.
3. Restore completion and reentry together: task exit status, notifications,
   SetReenterCount, native-child wait, record freeing and disconnect cleanup.
   Validate original source ordering before inventing locks or durable states.
4. After each group, compile/link the selected original owner and run focused
   contract tests. Test doubles may supply unavailable OS mechanics but must
   not implement the policy being asserted. Record selected symbols/link maps.
5. Replace or remove the superseded provider once its replacement is selected
   and verified. Do not keep two production policies behind a diagnostic switch.
   A temporary fixture transport is explicitly test-only and cannot count as
   cross-process acceptance; S3 must replace it with real IPC evidence.

S2's bounded source-recovery conclusion selects the original service/client
lifecycle in formal owner archives and tests, with finite bindings enumerated.
The earlier wording also required removal of the old product provider before
S3's transport existed. That dependency ordering is corrected explicitly:
product selection and removal of the local fallback are mandatory S3 gates;
final obsolete-source deletion remains S6. They have not passed in S2 and are
not waived or exported to another T. A test using an old autonomous provider
still cannot count as original-body evidence. See the
[S2 transition record](../evidence/m0-t412-s2-original-lifecycle.md#s2-bounded-conclusion-and-s3-transition).

## S3 detailed procedure — connect the real process topology

1. Apply the S1 naming/publication decision to the authoritative design/rules,
   formal build graph and staging tool before publishing a renamed executable.
   Build the three x86 programs from the same recorded source configuration.
   Decide explicitly whether any temporary ntvdm32 entry remains; never silently
   preserve it as a fourth independent launch-policy implementation.
2. Implement run16 entry as a thin composition of selected original client
   behavior: positional target, existence/type errors, untouched guest tail,
   environment and stream capture, broker connection/start, requested worker
   creation and registration. No user-required diagnostic switches.
3. Implement broker endpoint discovery and authenticated handshake. Concurrent
   starters converge on one healthy instance in the agreed user/session scope.
   A stale endpoint is not a healthy broker; failure must be bounded and visible.
4. Bind original BaseClient/service calls through validated copied records.
   Validate peer identity, version, type, lengths, arithmetic and IDs before
   invoking the service. Define how native stream/process resources become
   process-local handles without serializing handle values as resource identity.
5. Connect original worker startup and GetNextVDMCommand call sites. Compare
   OS command line and SoftPC argv consumers so startup flags cannot disagree.
   Prove which process owns worker creation, registration and execution.
6. Exercise first launch, existing-broker launch, existing-eligible-worker
   selection, required-new-worker selection, duplicate registration, failed
   creation and disconnect at each launch boundary. Assert no orphan reservation
   or command loss, and no unauthorized peer can register/control a worker.

S3 exits with real-process command delivery/return evidence and no shadow local
service fallback. Full DOS stream/reentry behavior is validated in S4, not
inferred from successful handshake.

## S4 detailed procedure — preserve DOS command semantics end to end

1. Establish fresh direct positional MEM, COMMAND and EDIT results in the
   three-program topology, including normal task return and launcher exit code.
   Record the Console host used; do not substitute an independent GUI window.
2. Trace the actual COMMAND BOP 54:08 helper call through run16 classification
   and the original permcom/reentry path. Test both a DOS child and a native
   child, including return to the same interactive COMMAND session.
3. Audit and bind the async capture boundary before worker creation: copied
   command tail, double-NUL environment and each standard stream's lifetime.
   Test cancellation and failure after capture but before dispatch/completion.
4. Validate quoted paths/spaces, arguments, current directory, environment,
   input/output/error streams, EOF, child nonzero exit and parent wait/wakeup.
   Use fixtures only for missing controlled behaviors; compiled fixture outputs
   remain in build/, never among the owner-provided runtime binaries.
5. Run separate redirection cases: host shell consuming `>` before run16;
   a literal `>` delivered to the CLI; and redirection parsed inside guest
   COMMAND through BOP. Assert the declared S1 contract for each case. A literal
   operator is not silently claimed to have shell semantics.
6. Regress real EDIT keyboard and mouse movement/click/release, including
   dismissal of its welcome dialog before interaction assertions. Capture and
   assert observable behavior; injected input alone is not a successful test.

Automate repeatable Console/process tests where possible. Do not offload
routine diagnostic iterations to the owner. Report an automation limitation
honestly when a visible interactive result cannot be asserted.

## S5 detailed procedure — coordination, retention and failure races

1. Trace the selected real WOW startup client's command acquisition, PIF,
   first-task, notification, wait/retry and completion/error paths. Separate
   service coordination from WOW provider execution and private USER boundaries.
2. Exercise source-defined DOS/WOW separation, no-work waits, command arrival,
   caller disappearance and notification delivery. Check source-owned results,
   not an invented success response for unavailable WOWEXEC/USER behavior.
3. Recover and verify the original worker completion/exit path. Do not derive
   a standalone ready/idle state, timer or cooperative-exit request: worker
   residency is owned by the original COMMAND/Console/`ExitVDM` lifecycle.
4. Race new commands against broker drain, worker registration
   against broker drain, and two launchers against startup. Each accepted
   request must either execute once or receive a defined retry/failure; it
   cannot disappear or execute twice.
5. Exercise worker abnormal exit, launcher disconnect and broker loss with
   owned test processes. Verify deterministic pending-request outcomes and
   bounded cleanup. Do not kill arbitrary cmd/conhost/ntvdm processes by name.
   Restart does not silently replay an indeterminate previously executed task.
6. Confirm empty-broker exit only when workers, reservations, queues and client
   operations drain; then prove a new launch starts a fresh service with correct
   first-VDM semantics and no host boot-file mutation.

## S6 detailed procedure — prove and deliver the final replacement

1. Rebuild final programs from the recorded formal graph; record architecture,
   source/configuration identity, selected original providers and file hashes.
2. Run the complete S2--S5 suite against that same final product. Include real
   DOS workloads, original broker-side WOW contracts, security negatives,
   startup/disconnect and broker-empty-grace intervals. A previous build's
   success is not acceptance of an untested final executable.
3. Audit remaining old entry paths, single-slot local service policy, shadow
   ownership tables, compatibility seams and test-only bridges. Delete only
   proved superseded/unreachable code; preserve required finite interfaces with
   original-owner rationale and exact positive/negative evidence.
4. Recompute the source diff/overlay/autonomous ledger against the S1 baseline.
   Name recovered original functions and deleted autonomous groups, retained
   seams and their removal conditions. Added transport code is reported, not
   hidden by gross deletion totals.
5. Review all requirements and update source registers, current authority,
   evidence and closure records. Stage only verified formal products under the
   revised publication contract; all logs remain under the runtime logs path.
6. Review actual Git changes, run governance and diff checks, commit/push and
   verify clean local state and upstream synchronization. T closure requires
   every mandatory scoped contract to pass or an explicit owner-approved scope
   disposition; it cannot be inferred from a neat worktree.

## Delivery cadence and failure handling

Implement one coherent group at a time, then build and run its focused tests;
run the selected real workload after changes affecting its path. Complete the
active S evidence and sequential P delivery before advancing the Status packet.
Do not ask for renewed permission for unchanged already-admitted T scope;
material boundary changes still follow the execution rules.

A failed group stays open: retain its baseline, exact error, selected provider
and narrow next check. Preserve previous artifacts by recorded identity when
needed for comparison; never silently replace a known-working product with an
unverified diagnostic build. Do not revert unrelated user work or declare a
test passed because it was skipped, unavailable or terminated by a timeout.

## Required S1 decisions (binding checklist)

- run16 validates/classifies, requests service selection, creates a worker only
  when requested and registers it. basesrv owns the original service policy;
  ntvdm owns one original CCPU40 machine. Do not substitute a new broker policy
  first and promise original-source recovery later.
- Reconcile the current ntvdm32.exe-only publication rules with three-program
  naming before changing build/deployment. Admission itself replaces no EXE.
- Specify per-user/session singleton identity, endpoint authorization, message
  version/length validation, stable IDs, process-local resource duplication,
  reservation expiry and exact-once completion. No native/guest pointer or
  trusted sender-local HANDLE value enters a command record. Required resources
  use only the approved separate authenticated OS-managed attachment boundary.
- No worker idle lease, cooperative worker-exit request or worker timeout is
  permitted. Assigned/queued tasks, active guests, Console residency and
  original task completion are governed by the original worker lifecycle.
  The broker's empty-grace applies only after no worker, reservation, queue or
  client remains, and must account for in-flight clients and startup/arrival races.
- Preserve service-instance first-VDM semantics and existing protection against
  mutation of host IO.SYS/MSDOS.SYS. Original-source recovery does not authorize
  host installation changes.
- Capture command tail, environment and stream identities before asynchronous
  guest leases expire. Keep stream lifetime and failure cleanup source-shaped.
- Revalidate U07-B/U08-B dependencies on worker-local U07-T/U08-W evidence.
  Historical closure and isolated binder/stub fixtures do not prove complete
  worker reset/wait/termination or rendering. Explicitly promote a source-proven
  missing direct dependency; do not add a second machine lifecycle to broker.

## Acceptance discipline

### Owner-approved S7 extension

On 2026-09-15 the owner accepted direct launch after the S6 VDM TIB storage
repair, closed that S at its bounded delivery, and admitted S7 to repair the
second interactive COMMAND's MEM, COMMAND and EDIT launches. T412 remains open.
S7 first reproduces typed commands in the actual interactive shell, audits
the original EXEC/BOP/reentry selection and its immediate host bindings, then
implements the smallest source-shaped repair and verifies child completion
and return to a usable parent prompt. Missing-child failure and direct launch
are regression controls. No new classifier, generic shell parser, worker
reaper or CPU change follows from this admission.

The S6 outstanding same-build S2--S5 matrix and final removal accounting are
still required before T412 closure. They are not silently waived by either
S6 owner-directed closure or S7 admission. The current Status owns the S7 brief.

The first no-source-change control, `run16 COMMAND.COM /c MEM.EXE`, displays
MEM output and returns zero (`O:\winnt\logs\m0-t412-s6-nested-mem.txt`). It
does not emit the native-child report and does not reproduce the owner's typed
interactive-shell failure; it must not be substituted for that workload.

Runtime verification uses the formal Win32/x86 CCPU40 product and records
artifact identity, selected providers, expected/observed outcomes and trace
markers. Build/intermediate outputs stay in build/; real-package observations
stay in O:\winnt\logs. Newly required autonomous semantics need divergence
reasons and exercised traces, not merely compilation success.

Full WRITE application recovery belongs to the later WOW16 package. This does
not waive mandatory WOW broker contracts: missing selected coordination blocks
closure. A tested unsupported result proves only its failure contract, never
successful guest execution. Distinguish source inspection, focused original
provider tests, real-process integration and full application success.

Measure changes against the frozen baseline separately for mirror changed
lines, private overlays and autonomous policy. A relocation is not a semantic
reduction. Zero autonomous policy is an objective, not a promise that modern
IPC requires no new glue. Restore composable original source before finite
same-shaped bindings/private overlays; newly authored behavior is last resort.
Mandatory unresolved work needs an explicit approved disposition, not a silent
transfer to a later package. Preserve unrelated work.
