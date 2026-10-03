# T423 S12 NTW32 Backend

T424 S11 naming normalization: frontend labels/current source links now use NTCON.
This does not claim the new basename existed in the recorded historical package.
Exact earlier source, commands and product names remain in Git and sealed build
evidence; recorded hashes, dates, results and limitations are unchanged.

Current result: implementation/verification closed in S12 P2; the verified
eight-file O:/winnt package awaits owner side testing. See
[final delivery](#final-publication-and-closure-r131-r132). Earlier observations
below are retained chronology, not a second current-state authority.

## Admission and inherited evidence

Owner approves the standalone native-text backend after the S11 stream-only
screen counterexample. Owner clarifies that S11 must first complete its mouse
repair and pressure acceptance. On 2026-09-28 the owner accepted S11 and
explicitly admitted S12; CURRENT now contains its sole active packet.
Only native continuity/retirement restructuring transfers; S11 mouse defects
do not. Candidate changes and failed tests are retained in the working tree.
S11 mouse repair is now closed and its seven-file package is published.
At admission no NTW32 candidate was published; S11 965083eec was the accepted
baseline. The then-planned RDP S13 was later cancelled by the owner. Authorized
Queue/WOW side-chat documents are reconciled in the final P2 delivery without
admitting their implementation.

Inputs: current main/worktree, S8 d253e55af native_console_host/capture/view,
S9 ConPTY transport/parser, S11 cursor and screen-contract fixtures and mouse
pressure results. Source-first boundary: original MVDM screen copy and task
algorithms stay in their mirror. Windows supplies native Console semantics;
OpenNT ntw32/CSR internals cannot compose without recreating the prohibited
system server. Reuse the existing public-API mechanics, not that server.
No external source/lib/guest import is needed for the new product shell.

## Migration ledger

### Public run16 native execution migration

run16_frontend_scope_launch_native now selects an existing NTW32 through the
authenticated client call or reserves/creates its sibling ntw32.exe through
the same run16_worker_prepare transaction used for VDM. It resumes only after
Prepare, submits directly to the worker and retains its own target/receipt.
It does not invoke the frontend's old target-creation route. Initial NOT_READY
is retried only before request acceptance, while checking worker/frontend
survival; a failed accepted request is never replayed. Public presentation
integration is still pending and this candidate must not be published.

The native reservation table now rejects a second same-Console native startup
under its existing lock, including before Prepare/Connect. Distinct execution
Consoles remain independent; release permits a subsequent startup. DOS/WOW
reservation cardinality is unchanged. The focused reservation test passes
pending, claimed, distinct-Console and released cases; true simultaneous-launch
testing and stale-abandoned startup coverage remain required.

Extended the real RPC test to invoke actual run16.exe from the second launcher
process with authenticated inherited capabilities. That public CLI selects the
already resident NTW32, runs CMD and returns 61. Existing 37/19, second-client
73, forged-context rejection and worker-survival checks also pass. The root
is a test-registered frontend, not NTCON presentation: this proves the public
execution/return route, not visible interaction or initial-worker creation via
the public CLI. Build logs: build/M0-T423/S12/public-native-build-r1.log and
r2.log; runtime O:/winnt/Logs2/t423-s12-public-native-r1.log and .err.log.
The scope fixture remains explicitly mocked; its denied native stubs are not
counted as native execution evidence. Eight-file publication and S12 closure
remain pending.

### Real second-launcher NTW32 reuse

Extended --ntw32-execution with a separate --ntw32-reuse-child process. The
parent supplies restricted inherited frontend/execution capabilities; a PID
argument is an assertion of the expected instance only, never selection input.
The child connects through actual RPC, rejects an unrelated execution event,
binds the authenticated execution capability and proves CommandWorker is not
granted merely by binding. SelectNativeWorker then returns the existing NTW32;
duplicate reservation is rejected. A new CMD target executes on that NTW32 and
returns 73 through its actual process/receipt. Child disconnect/exit leaves
the original worker alive. The earlier 37/19 and failed-launch/alias/EOF cases
also pass in the same run. This is a real process/RPC test, not the service's
mock Console-query fixture.

Build: build/M0-T423/S12/native-reuse-rpc-build-r1.log. Runtime:
O:/winnt/Logs2/t423-s12-native-reuse-rpc-r1.log and .err.log.
Remaining: public run16 migration, concurrent creation/admission, presentation
activation and continuity, actual-member retirement/monitor closure and full
product gates. The fixture launcher is not public run16 acceptance. No candidate
publication or S12 closure is claimed.

### Native selection RPC binding

The service selection now has an interface-owned typed SelectNativeWorker RPC,
authenticated server wrapper and BaseClient binding. Client handle validation
and failure cleanup share the existing CommandWorker wrapper body. Protocol
identity is now 13 in both IDL and version.h, with matching generated interface
references; this is an unpublished candidate change, not an update to S11.
All six candidate EXEs and base-client-rpc-first-test.exe rebuild successfully
(build/M0-T423/S12/native-selection-rpc-build-r1.log). The actual NTW32 RPC
fixture first verifies SelectNativeWorker returns NOT_FOUND/null for an empty
service, then retains successful actual-worker creation, 37/19 results, redirected
alias/EOF and failed-launch reuse. Runtime log:
O:/winnt/Logs2/t423-s12-native-selection-rpc-r1.log and .err.log.
Positive second-process selection still has only the prior service fixture
evidence, not real RPC/public-run16 acceptance. That test and public caller
migration remain open, alongside presentation and retirement integration.

### Independent subsequent-launcher native selection

Added service-side SelectNativeWorker over the existing registered worker watch
list. After the existing authenticated execution-Console binding, an explicit
selection grants a query/synchronize reference to its unique live native worker.
The launcher connection pins that generation, not a PID or frontend lifetime.
RetainCommandWorker subsequently resolves that exact instance and fails after
death instead of silently replacing it. A selected launcher cannot then create
another native reservation, issue DOS CheckVDM or rebind its execution context.
Original DOS/WOW record and scheduling bodies are unchanged.

The --native-worker fixture proves connection alone does not grant command
access, wrong generation is rejected, a second authenticated connection can
select/retain the existing worker, and duplicate reservation creation fails.
Its Console-query provider is the established mock, not real host membership
evidence. frontend-channel, reenter-nested-return, launcher-exit-survival and
unfinished-worker-exit service modes also pass. Build:
build/M0-T423/S12/native-selection-build-r1.log. Logs are under
O:/winnt/Logs2/t423-s12-native-selection-<mode>-r1.log.

The typed RPC/client call, real-process reuse/authentication negatives,
concurrent admission and run16 migration remain open. This is a service-side
implementation, not public CLI reuse acceptance, publication or S12 closure.

### Completed native-request reclamation

The independent main previously collected a finished request only after the
next WaitWorkerChannel delivery. A resident idle worker therefore retained the
last request's sender/frontend/execution references and thread resources.
Moved request completion cleanup into execution.c: a worker-local request group
has a shared cancellation event and an idle event; each request releases its
attachments and storage before decrementing the active count. Main still uses
the same blocking authenticated receive, with no polling helper or scheduler.
Group close stops local waits and waits for cleanup; it never terminates targets
or signals their completion as though they had exited. This is native request
resource ownership, not a new shared worker or DOS task lifecycle policy.

Formal ntw32-execution-lifetime-test.exe exercises the actual execution.c body:
12 malformed requests reclaim themselves without a subsequent delivery;
16 blocked header reads cancel; real native targets remain live after request
group close and their completion receipts remain unsignalled. The fixture alone
then explicitly disposes its known test target. After one process-creation
warmup, repeated operations return to the exact handle-count baseline. The
initial run observed a seven-handle first-launch increase; this test proves no
per-request growth after warmup, not attribution of every host startup handle.
Latest result: 267 checks, zero failures, zero remaining handles relative to
that warmed baseline. Build: build/M0-T423/S12/request-lifetime-build-r3.log;
runtime: O:/winnt/Logs2/t423-s12-request-lifetime-r3.log.

Rebuilt actual ntw32.exe and reran base-client-rpc-first-test.exe
--ntw32-execution against the candidate broker: 37/19 results, redirected
alias/EOF, failed launch/reuse and resident worker all pass. Logs:
O:/winnt/Logs2/t423-s12-request-lifetime-execution-r1.log and .err.log.
The presentation activation/return and public caller migration remain open;
no production P, publication or closure follows from these bounded tests.

### Native presentation endpoint transport

Added NTW32-local presentation.[ch], selectively retaining the existing NTVDM
console_client request ordering, generation/sequence validation and atomic
BEGIN/DATA frame protocol. It borrows authenticated local attachments; it does
not authenticate PIDs, own a frontend, select an execution task or auto-activate
on paint. Native publication rejects DIBs. Inactive-owner errors stay ordinary
replies so a later acknowledged handoff can reuse the connection; transport or
framing failure is latched instead of replaying an uncertain request.

The byte-transfer declaration now lives in NTW32-local io.h. Actual NTW32
execution/channel_io no longer include the superseded ConPTY control/client
header. No cross-component record or lifecycle body moved into worker-base or
interface. This does not yet remove every historical ConPTY client source.

The formal ntw32-presentation-test.exe uses a real local byte pipe and the
existing production video receiver. It proves chunked complete publication,
NOT_READY without implicit activation, wrong generation/version, contradictory
reply, truncated reply and pre-signalled cancellation with no second request
after a failed stream. Latest result: 78 checks, zero failures; build log
build/M0-T423/S12/presentation-channel-build-r4.log and runtime log
O:/winnt/Logs2/t423-s12-presentation-channel-r4.log. Early fixture failures used
a process pseudo-handle in a multiwait; the fixture now supplies the real
synchronize handle used by product attachments. Pipe EOF/error 233 is normalized
to the same disconnected result as the NTVDM client, not treated as success.

The actual NTW32 EXE was rebuilt and --ntw32-execution passed again, including
37/19 direct results, aliased output/EOF and failed-launch reuse; logs
O:/winnt/Logs2/t423-s12-presentation-execution-r1.log and its .err.log companion.
The new presentation endpoint is still not invoked by the worker execution
loop. Activation/return and screen/input pumping remain mandatory production
wiring; the named-pipe test is transport evidence, not public CLI acceptance.
No O:/winnt publication, production P or S12 closure is claimed.

### Shared-interface review and formal text-frame regression

Reviewed the latest interface/worker-base split against the executable owners.
Corrected current architecture/coding clauses that still assigned service IDL
to NTSRV, declarations to product-abi, or ConPTY to NTW32. Historical S9/S11
implementation descriptions remain explicitly historical; they are not backend
alternatives. Endpoint implementation and process-private state remain local.

The service's OPENNT_BASE_WORKER_INFO is a local snapshot projection translated
field-by-field by Server_TaskSnapshot into the interface-owned IDL record.
It is not independently serialized. Do not move its callbacks, process handles
or service implementation into interface merely to share a local C header.
The remaining declaration review must distinguish such local projections from
actual duplicated wire records; this review alone does not close that sweep.

Added ntw32-text-frame-test.exe to the formal x86 Ninja graph. It links the
NTW32 producer directly to the unchanged NTCON/NTVDM frame receiver and checks
font banks, palette, cursor, PC glyph mapping, chunked atomic publication and
malformed input. Fixed a clipped surrogate-pair trailing cell: the preceding
high surrogate may be outside the viewport but remains inside the captured
screen buffer. The second cell stays blank; an unpaired low surrogate remains
the explicit unsupported-glyph question mark.

Verification: build/M0-T423/S12/interface-frame-build-r1.log; run
ntw32-text-frame-test.exe with O:/winnt/Logs2/t423-s12-interface-frame-r1.log
reports 50 checks, zero failures. vdm-protocol-test.exe also passes its eleven
operation envelopes and eight payload-field negative/overflow checks.
Documentation governance and git diff --check pass. These are native x86
contract tests, not a guest/CCPU execution or production-channel acceptance.
The NTW32 producer still needs channel/active-owner wiring; no candidate has
been published, committed as a production P, or declared S12 complete.

### Independent NTW32 entry and actual native execution

main.c no longer accepts the inherited --control/--peer/--registry entry. The
formal NTW32 target now composes execution.c in place of the old control/launch
dispatcher. worker-base connects and watches the broker; WaitWorkerChannel
uses the service's existing capability-arrival condition and worker-death wake,
not a polling loop or new task scheduler. Protocol remains unpublished v12.

execution.c selectively recovers NTCON native_console_request.c: validate the
copied packet; accept authority only from authenticated broker attachments;
duplicate the pinned sender's allowed streams preserving aliases; invoke the
existing native CreateProcess body without ConPTY; return the real target
process; observe direct completion. Request teardown cancels its own I/O/wait
and closes resources, never tree-kills running targets. The old source remains
only for the still-unmigrated frontend path and must be removed at final binding.

Tests: base-client-rpc-first-test.exe --ntw32-execution uses actual ntw32.exe,
not the fixture child as backend. It proves two CMD launches returning 37/19,
aliased stdout/stderr and pipe EOF, missing-image error with no target, subsequent
successful request and worker survival. --native-worker-startup now tests the
blocking channel RPC; --native-reservation retains startup rollback/reuse checks.
frontend-request-client-test covers both destination paths with actual target 37,
rejected, malformed-version, contradictory, truncated and EOF replies.
Build logs: build/M0-T423/S12/ntw32-entry-build-r1.log and r2.log; runtime logs:
O:/winnt/Logs2/t423-s12-ntw32-entry-{ntw32-execution,native-worker-startup,
native-reservation,client}-r2.log. All pass; six x86 EXEs link.

This does not prove public run16/NTCON integration: their old private NTW32
caller is now incompatible and cannot be published. Native final-frame receipt
currently observes target completion only, not presentation. Full frontend
binding, later-launcher reuse, input/screen continuity, member retirement,
management and full runtime regression/publication gates remain open. No
candidate replaced the accepted S11 runtime package and no S12 P is delivered.

### Direct launcher-to-worker attachment

The existing one-pending-channel-per-client attachment implementation now
selects either its retained frontend destination or an admitted native worker.
SubmitWorkerChannel/TakeWorkerChannel reuse validation, source-process pinning,
resource duplication and cleanup; they do not introduce a command queue or
target scheduler. Only the launcher's existing prepared reservation currently
grants worker selection. Later-client admission/re-entry remains open.
The worker receives the sender process, byte pipe, execution capability and
authenticated frontend capability. NTCON cannot take a worker-directed request.
Source shapes reused: service_submit/take frontend channel in base_service.c;
the service contains only attachments, not launch payload or process creation.

This RPC addition advances interface/service.idl and APP_PROTOCOL_VERSION to 12;
APP_VERSION remains 0.0.423. Both generated bindings and version identities are
rebuilt. No accepted runtime package is replaced. Existing published S11 peers
must not be mixed with this unpublished candidate protocol.

Evidence: build/M0-T423/S12/worker-channel-rpc-build-r1.log links six x86 EXEs
and fixtures. Logs2/t423-s12-worker-channel-rpc-r1.log passes real RPC suspended
startup/claim, hidden Console, frontend binding and separate direct worker
pipe byte exchange with authenticated sender and capabilities; the test child
returns 73. The service case worker-channel-service-r1.log tests wrong generation,
foreign event, wrong pipe end, duplicate pending admission, frontend interception
rejection, exactly-once delivery and root-rundown cancellation. Eight retained
service modes pass in Logs2/t423-s12-worker-channel-*-r1.log: frontend-channel,
frontend-root, frontend-rundown, three re-entry orders, launcher-exit-survival
and unfinished-worker-exit. These are service/RPC fixtures, not production NTW32
program execution or a full S12 regression pass.

Next binding must replace the private NTW32 frontend-owned entry and move the
existing native request executor into NTW32, then connect independent re-entry,
frame/input and retirement. The old frontend-directed submission remains only
for the still-unmigrated candidate caller and must disappear with that migration.

### VDM protocol declaration ownership

The remaining copied VDM message/startup/value/span declarations moved from
four service transport headers into src/interface/vdm_protocol.h. Original
names, field ordering, sizes and version remain unchanged; local encode input
pointers and validation/encoding functions stay in the transport component.
The native service-local reservation discriminator is not a wire enum and stays
local. Management RPC serialization is already declared by interface/service.idl;
the service's local snapshot projection is not a second wire record.

Extended tests/broker/vdm_payload.c asserts the six structure sizes and stream
offset, includes both owner and wrapper headers, and retains eleven operation
and eight-field positive/negative cases. The formal graph now exposes the
vdm-protocol-test.exe target with a separate test object (the first ad-hoc
compile exposed a same-basename object collision, not a product failure).
S12 interface-vdm-test-r3.log passes. The first formal fixture failed because
its overridden flags lacked the src include root; reviewing the transport
flags also exposed absent /showIncludes dependency tracking. Both are fixed.
interface-vdm-product-build-r2.log rebuilds all six transport objects and links
six x86 EXEs. interface-vdm-target-test-r2.log and
interface-vdm-native-service-r2.log pass the protocol and independent-worker
service cases against that rebuilt closure, superseding r1 linkage evidence. These logs
are under build/M0-T423/S12. No publication or functional S12 closure follows:
the retained private NTW32 entry still needs migration to independent admission,
direct native launch/re-entry and the shared production frame endpoint.

Latest owner approval replaces ConPTY with an ordinary hidden Console attached
to and owned by the independent NTW32 worker. No private helper/bootstrap,
NTSRV Console owner, or second selectable ConPTY backend. NTCON remains a
presenter of the unchanged NTVDM text ABI and bitmap glyph mapping. Older
ConPTY experiments below are retained evidence, not pending authorization.

| Existing owner | Disposition | Required proof |
| --- | --- | --- |
| S8 Console screen/cell operations | Reuse in attached NTW32 boundary, not old frontend helper owner | Actual Console readback and shared-buffer witness |
| NTCON ConPTY resource and terminal parser | Remove from production on cutover; NTW32 uses actual hidden Console state | One independent NTW32 reused across DOS intervals |
| NTCON native launch packing and authenticated request | Reuse validated resource marshalling, route actual creation into backend | Redirected streams, exact process result and rollback |
| NTSRV frontend/worker authentication and management | Extend named native-backend registry separately from DOS/WOW records | Cross-session, stale ID, wrong version and rundown negatives |
| NTMON snapshot/termination | Extend registered backend kind and session-close result | Retained clients, explicit close and unrelated survivor |
| S11 local parser import/branch restoration | Remove when real backend synchronization replaces it | No stale cursor, lost text or duplicate output |
| S11 mouse input repair | Prerequisite owned and completed by S11, then regression only | Retain S11 pressure, button/key and suspension evidence |

## Required implementation gates

- [x] Real Console attached backend primitive and x86 fixture.
- [x] Versioned authenticated transport, startup and per-frontend reuse.
- [x] NTSRV registration, native members, liveness and NTMON session shutdown.
- [x] Production native target creation/completion with GUI behavior unchanged.
- [x] Both screen-transfer directions, final-output ordering and input ownership.
- [x] Native descendants, retirement/admission races and fault isolation.
- [x] Previously accepted S11 mouse and candidate production regression gates.
- [x] Coherent eight-file publication, governance and source review for P2; commit/push confirmed by the delivery's Git state.

Only checked production-path evidence closes these rows. Compile or a direct
fixture is supporting evidence, never NTW32 product acceptance. All build
outputs start under build/M0-T423/S12; runtime logs use O:/winnt/Logs2.

The checked rows reflect cumulative production evidence through r132, not the
early chronological observations below. The final section records published
checks; Git identifies the P2 delivery and remote synchronization.

## Current observations

### Worker-side library restriction implemented

worker-base now contains only connection.c/h and README. Both workers use its
service connection/broker-watch lifecycle. run16/ntsrv/ntcon/ntmon product edges
no longer link this archive. Input conversion and byte transport return to their
endpoint owners; NTVDM retains its original exchange/frame logic. NTW32's old
private control candidate still awaits production replacement, not acceptance.

Build: build/M0-T423/S12/worker-scope-build-r1.log, six x86 EXEs and fixtures.
Console normal (73), broken-pipe (0), native request/result negatives (0) pass:
O:/winnt/Logs2/t423-s12-worker-scope-<exe>-<case>-r1.log and stderr peers.
verify-frontend-link-ownership.ps1 passes including leakage controls; explicit
inspection confirms four non-worker edges contain no worker-base dependency.
Governance/diff checks pass. No publication or S12 closure.

### Mixed frontend/native declaration separation

interface/frontend_protocol.h owns the unique bootstrap/native request/reply
and launch-packet records; native_console_protocol.h owns retained candidate
control/ready records. Local HANDLEs/state/functions stay in their components.
No layout/version changes; this does not admit a second production display ABI.
Build passes: build/M0-T423/S12/interface-packets-build-r1.log. Actual target 37
and protocol negatives pass in O:/winnt/Logs2/t423-s12-interface-packets-r1.log.
The ordinary hidden Console fixture also passes cells/cursor/active-buffer,
members/descendants, raw/cooked input, mouse pair and control negatives:
tests/observation/verify-ntw32-console-state.ps1 -BuildRoot
build/M0-T423/S12/interface-console-r1 -LogPath
O:/winnt/Logs2/t423-s12-interface-console-r1.log. Its marker explicitly states
product-wiring=pending. No publication or S12 closure.

### Interface migration and worker connection ownership

console_io.h, console_mouse.h, console_video.h, version.h and service.idl now
have unique src/interface paths; layouts and versions are unchanged. Source,
tests and the generated graph use those paths. product-abi is README-only.
Mixed native request/control declarations still require migration review.
worker-base/connection.c is actually called by NTVDM bootstrap and NTW32 main
for ConnectCurrent/WatchBroker initialization, with failed-watch cleanup.
The native RPC child uses it too. Frame exchange returned to NTVDM; the
overbroad worker-base/frontend.c/h files are removed. Channel/codec cleanup
and the formal NTW32 entry migration are still pending.

build/M0-T423/S12/interface-owner-build-r1.log proves regenerated RPC and six
x86 EXE links. Real RPC --native-worker-startup and --reservation-parent pass;
console-client-test passes copied 80x50/43-row frames and its original close
callback (expected exit 73). Logs use O:/winnt/Logs2/t423-s12-interface-owner-
with native-worker-startup, reservation-parent and console case suffixes r1.
Governance/diff checks pass. No product publication or S12 closure is claimed.

### Owner restriction: worker-base is worker-side lifecycle only

The owner's latest clarification supersedes the broad sharing model recorded
below. Only NTVDM/NTW32 worker-side lifecycle belongs in worker-base. run16,
NTSRV, NTCON and NTMON each retain their own common two-kind handling path.
Transport/codec/frame extractions are not admitted by reuse alone and remain
pending relocation; non-worker worker-base link edges must be removed too.

The first correction moves worker_launch.c/h back to run16-exe and renames
the function run16_worker_prepare. Both original launcher and native startup
fixture call the same body; no duplicate worker creation path was introduced.
The generated graph links obj/run16/worker_launch.obj explicitly instead of
including it in worker-base.lib. x86 build passes in
build/M0-T423/S12/launcher-owner-build-r1.log. Real RPC modes
--native-worker-startup and --reservation-parent pass with an isolated broker;
logs: O:/winnt/Logs2/t423-s12-launcher-owner-<mode>-r1.log and stderr peers.
No candidate is published, and remaining placement correction is not complete.

### Existing Console/frame exchange recovered into worker-base

worker-base/frontend.[ch] extracts the existing console_client exchange and
chunked video publication, including version/generation/sequence validation,
reply size/operation checks, stable transport errors and video serials.
NTVDM embeds this state and selects both functions in production; its owner
session, lock, mouse and close callbacks stay local. No wire layout changes.
NTW32 still must replace its private control entry with this shared path;
linking the archive alone does not prove that migration.

Build: build/M0-T423/S12/shared-frontend-build-r1.log, regenerated x86 graph
with source hashes for frontend.c/h. console-client-test.exe normal verifies
80x50 dual-font copied text, malformed-frame retention, 43-row replacement,
retirement and original close callback (exit 73). --broken-pipe exits 0;
--close-hang exits 0xc000013a. frontend-request-client-test.exe passes actual
target 37 and its protocol negatives. Logs: O:/winnt/Logs2/
t423-s12-shared-frontend-<executable>-<normal|broken-pipe|close-hang>-r1.log
and matching stderr. Candidate NTVDM/NTW32 link; no runtime publication.

### Shared NTVDM byte transport

NTVDM client_transfer now calls worker-base/channel_io.c instead of keeping
its own overlapped loop. The frontend entry retains the existing peer pre-check,
peer-first wait and PIPE_NOT_CONNECTED result. The request entry retains
completed-I/O precedence, optional stop and PROCESS_ABORTED. Both drain cancelled
operations. Original DOS close callbacks and task policy remain NTVDM-owned.

Build: build/M0-T423/S12/shared-transport-build-r1.log, x86 /MT NTVDM, NTW32,
NTCON and affected fixtures; VdmTib ownership passes. console-client-test.exe
normal exits 73 as expected; --broken-pipe exits 0; --close-hang exits
0xc000013a. frontend-request-client-test.exe verifies target result 37 and
rejection/version/contradictory-reply/EOF/partial-reply failures. Logs use
O:/winnt/Logs2/t423-s12-shared-transport- with executable, case and r1/r2 suffixes.
The first unprefixed broken argument accidentally exercised normal mode; only
the corrected --broken-pipe r2 is counted. This is focused transport evidence,
not complete NTW32 acceptance. The published package remains unchanged.

### Existing route rundown and native reattachment

The proposed extra ReleaseFrontend operation is unnecessary for real root
rundown and was not implemented. Existing service_clear_frontend removes a
delivered route; the shared worker identity/watch remains. ERROR_ALREADY_EXISTS
from a second Take on a live route is deliberate duplicate-delivery rejection,
not evidence of a missing release operation. A transient pipe failure must not
be promoted to root death or silently trigger task replay.

Extended tests/adapter-basesrv/base_service_reservation_test.c --native-worker
now uses distinct launcher, worker and frontend process identities. It verifies
root-context rundown leaves the same worker alive, removes its old capability,
returns NOT_READY with cleared Take outputs, denies the old capability, and
accepts a new registered frontend generation through the existing request,
attachment and Take APIs. Stale frontend generation is rejected; no DOS/WOW
record is created. This tests service mechanics, not the still-unmigrated NTW32
production entry or later-task admission after the original launcher exits.

x86 incremental build: build/M0-T423/S12/native-root-rebind-build-r4.log.
Passing run: O:/winnt/Logs2/t423-s12-native-root-rebind-r2.log and its stderr
peer. The first fixture incorrectly registered a second connection for the
same PID and failed at line 252; corrected by giving the frontend its own
test process. Its suspended worker was explicitly identified and terminated
before relinking. A preceding LNK1104 was that fixture's executable lock,
not a source/compiler failure. No package executable was replaced.

NTVDM console_input_watch still invokes original CntrlHandler(CTRL_CLOSE_EVENT)
on actual frontend death, with its existing bounded close completion.
console_command_ready explicitly rejects rebinding a dead DOS session.
Those are DOS policy, unlike shared service route rundown: they must not be
blindly generalized into worker-base or presented as native re-entry proof.

### Shared input codec and native control dispatch

The original NTCON encode_input and NTVDM decode_input bodies move unchanged
to worker-base/console_input.c; both production callers select those symbols.
NTW32 control version 3 adds bounded INPUT_WRITE using the same wire record.
It validates the entire packet before inserting anything and rejects the
DOS-private relative mouse record. Native Console input uses the recovered
S8 ntw32_input_write, not a new terminal parser or line editor.

x86 ntvdm/ntcon/ntw32 plus console-client/console-frontend targets link:
build/M0-T423/S12/shared-input-build-r1.log. Hidden Console r6 passes raw/cooked
input and control dispatch, including invalid trailing record causing no
partial insertion and valid DOS-private relative input rejected for native.
Runtime: O:/winnt/Logs2/t423-s12-hidden-console-r6.log.

The initial ad-hoc Console client runner incorrectly expected zero; the test
actually reached its original close callback and intentionally returned 73.
Revalidation against verify-frontend-transport-contracts.ps1's existing exit
contract passes normal 0x49, broken-pipe 0, close-hang 0xc000013a, and frontend
0, with required output witnesses. Logs2/t423-s12-shared-input-<test>-<case>-r2.log
records each. This is component acceptance, not DOS program regression or
complete NTW32 production cutover. No package publication occurred.

### Native worker shares the existing frontend attachment route

NTSRV records the typed reservation on its authenticated connection. Only a
creator with a retained, prepared native reservation may select that worker;
Console membership alone grants nothing. Existing RequestFrontend,
FrontendRequest, AttachFrontendRequest/AttachFrontend, TakeFrontend and
WorkerFrontendCapability now serve native identities as well as DOS. No new
native-only frontend RPC or synthetic DOS/WOW record is introduced. Admission
of later native tasks/re-entry remains separate unfinished work; a startup
reservation is not claimed to prove those task lifetimes.

x86 service and real-RPC builds pass (native-shared-route-build-r1.log and
native-shared-rpc-build-r1.log under build/M0-T423/S12). The service fixture
passes wrong capability, stale generation, non-worker denial, one-shot route
delivery, launcher/connection-loss survival and real process rundown, with
DOSHead/WOWHead empty. Log: O:/winnt/Logs2/t423-s12-native-shared-route-r1.log.
Eight retained DOS re-entry/launcher-loss/completed/unfinished-worker cases
pass: Logs2/t423-s12-shared-route-<case>-r1.log.

Real RPC native-worker-startup additionally passes shared suspended creation,
rollback, native claim, actual frontend capability/pipe delivery and a byte
returned by the hidden worker, followed by exact exit 73. Evidence:
O:/winnt/Logs2/t423-s12-native-route-rpc-r1.log. This tests production service
and transport but not the unfinished NTW32 entry; no publication or S12
closure is claimed.

### Shared channel transport

worker-base/channel_io.c replaces NTCON native_request_io.c and the duplicate
NTW32 channel_io.c. All native/frontend callers now select worker_base_transfer;
both old source files are removed. Framing/authentication remain at call sites.
NTVDM's different peer-death precedence/error and the frontend's additional
DOS-input readiness wait are not silently changed by this extraction.

build/M0-T423/S12/worker-base-channel-build-r1.log records successful x86 links
for run16, ntcon, ntw32 and frontend-request-client-test. The frontend ownership
verifier and leakage controls pass. The protocol fixture returned access denied
inside the sandbox; authorized execution passes actual target exit 37, request
rejection, version mismatch, contradictory result, EOF and partial reply.
Evidence: O:/winnt/Logs2/t423-s12-worker-base-channel-r1.log. Documentation
governance passes. This is not independent NTW32 production acceptance or
publication; lifecycle migration and the complete regression gates remain open.

### Shared worker startup transaction

Owner naming: src/worker-base and worker-base.lib replace the initial run16-local
file location below. Production run16 and the RPC fixture link this archive.
build/M0-T423/S12/worker-base-build-r1.log records a passing x86 build.
O:/winnt/Logs2/t423-s12-worker-base-native-worker-startup-r1.log and
t423-s12-worker-base-reservation-parent-r1.log pass. Native startup also checks
zero reservation rejection and rejected-Prepare cleanup before valid claim.
NTW32 production migration remains incomplete; no publication is claimed.

Owner requires actual code sharing, not parallel similar lifecycle paths.
run16-exe/worker_launch.c is extracted from main.c's existing suspended create,
startup-only Job containment, authenticated Prepare and disarm. Main's real
DOS/WOW route now calls it; kind-specific BaseUpdateVDMEntry and Resume remain
in the original sequence. It inherits no target streams, creates no long-term
Job policy and does not change original re-entry or completion owners.

Incremental x86 /MT build of run16.exe and base-client-rpc-first-test.exe passes:
build/M0-T423/S12/shared-worker-build-r1.log. Real RPC native-worker-startup,
native-reservation and original reservation-parent cases pass; logs are
O:/winnt/Logs2/t423-s12-shared-native-worker-startup-r1.log,
t423-s12-shared-native-reservation-r1.log and
t423-s12-shared-reservation-parent-r1.log. Native worker has its own hidden
Console, starts suspended, claims the prepared identity and returns 73.
This fixture is not a migrated NTW32 production entry; that remains required.

### Approved ordinary hidden Console, r1-r5

No helper, no ConPTY and no frontend are present in this primitive fixture.
The driver creates the test worker with CREATE_NEW_CONSOLE and SW_HIDE; the
worker verifies its window is not visible. Production entry migration remains
pending: these results do not close the production gates above.

- r1: compile failed on missing user32.lib for IsWindowVisible; corrected.
- r2/r3: CREATE_NO_WINDOW creates a usable initial buffer but the resize
  assertion fails locally. r3 encodes original height 9001 and Win32 error 87;
  no claim is made that this reproduces on all Windows configurations.
- r4: CREATE_NEW_CONSOLE plus SW_HIDE passes the unchanged resize and all
  screen/cursor/active-buffer/member/descendant/version/sequence assertions.
- r5: also passes actual raw input, cooked ReadConsole line completion,
  mouse press/release, processed Ctrl-C/Break and raw Ctrl-C key delivery.
  Production ntw32_input_write selectively recovers S8 input_records; no VT
  parser or synthetic line editor is added. Authenticated product input and
  DOS/native pending-input handoff remain unconnected.

Commands: tests/observation/verify-ntw32-console-state.ps1 with BuildRoot
build/M0-T423/S12/hidden-console-r1 through r5 and LogPath
O:/winnt/Logs2/t423-s12-hidden-console-r1.log through r5.log (r1 has only
build.log). x86 /MT, no physical desktop interaction or package publication.
The selected creation contract starts hidden; it does not allocate a visible
Console and hide it after flashing. No claim of full S12 verification,
submission, or eight-file publication is made.

### First implementation slice: attached Console operations

Recovery selection: d253e55af:src/ntcon-exe/native_console_capture.[ch]
supplies the existing grow/move/shrink ordering, exact rectangle writes,
active CONOUT$ reopening and bounded tile reads. Recover these mechanics into
ntw32-exe/console_state.[ch], with owner names updated, not a second renderer.
The current NTCON visible-output functions remain selected until the formal
composition can share the recovered source without changing dependencies.
That temporary duplication is migration WIP, not accepted final architecture.

Original ntw32/CSR owns the historical shared Console buffer and membership;
that system-server shell and private CSR transport are excluded by architecture.
Its modern public equivalents are used from the attached backend, preserving
real Console state instead of inferring native intent from VT. No mirror
changes or external imports are needed. S8 native_console_host.c already used
GetConsoleProcessList; retain that exact membership source, return actual PIDs
for backend-local checks, exclude self explicitly, and never tree-kill them.
New registry/IPC bindings still require authenticated peers and instance identity.

The first fixture executes the recovered operations inside a real ConPTY:
native readback after cell/cursor transfer, invalid geometry refusal, actual
attached child enter/leave, self exclusion and active-buffer selection. This
is a prerequisite only; registration, launch wiring and product closure remain
unchecked. Build roots: build/M0-T423/S12/console-state-r1 through r5.

### Attached Console and dispatch evidence

Entrypoint: tests/observation/verify-ntw32-console-state.ps1, BuildRoot
build/M0-T423/S12/console-state-r5, LogPath
O:/winnt/Logs2/t423-s12-console-state-r5.log, using VS2022 BuildTools
VsDevCmd -arch=x86 -host_arch=x64, cl /W4 /MT. Build log contains no warnings.
The fixture launches through current native_conpty/native_console_launch
into a real Windows pseudoconsole; its attached test role calls the new
ntw32-exe source. This is not yet the formal ntw32.exe process or authenticated
NTSRV route, and it does not replace a product regression.

Observed exit zero and NTW32-STATE PASS: native cell/attribute readback, actual
cursor continuation, active-screen-buffer selection, changed-geometry refusal,
real attached child membership, retained descendant after direct-target exit,
empty membership only after departure, detached survivor excluded, protocol
and APP_VERSION mismatch, sequence replay refusal, oversized payload rejection,
bounded capture/apply/write/read/member dispatch and unknown-operation error.
No target tree kill or Job is used. The r1 executable passed but its PowerShell
report check had positional Select-String arguments reversed; r2 fixed that
runner error, r3 added lifecycle cases, r4 dispatch tests and r5 detached survivor.
All raw records are retained, not rewritten as one uninterrupted success.

SHA256:

- console_state.c: BBE2CBFD2416C6E64C408839025395203D6BE3E97CB9B6133344D7FEDA520B1D
- control.c: E679CA7043CCEE43ADA33D15CF7E905724811B583C8883FDC9588218115F5336
- ntw32_console_state_test.c: E2B1E81C74B66627A18E8BB8E3CA0D80203C351B62D7AB8226AE35BC8E3472FC
- r5 fixture: 3E6DB140B3DFB6927FE584F93B4F7CC8750A972168FE434CD7E74C8D1255572C

control.[ch] is the finite copied-record adapter for the selected public Console
operations. It contains no native HANDLE/pointer in its records. Dispatch
requires an already authenticated single peer; version/sequence validation
does not replace authentication. The registry/transport step must establish
that peer before exposing it. Snapshot content is not atomic against concurrent
native writers; geometry races are reported, not fabricated into success.
S12 remains open. No eight-file candidate is published or production P claimed.

### Separate NTW32 process and private channel

Run r6 of the same verification script additionally builds src/ntw32-exe/main.c
as build/M0-T423/S12/console-state-r6/ntw32.exe and runs ntw32_channel_test.c.
The carrier really attaches to ConPTY and serves versioned requests over a
connected inherited duplex pipe. The inherited wait/query process capability
must match the pipe's actual server process; a command-line PID/handle number
alone is not authorization. Parent startup restricts inheritance to the pipe
and that capability and rejects a foreign pipe connection before child launch.
No arbitrary named endpoint discovery, process scanning or Job is introduced.

Observed NTW32-CHANNEL PASS and backend exit zero: actual separate executable,
member query excluding itself, screen capture begin/end, matching replies and
clean channel disconnect. Original real-state/descendant/negative fixture also
passes in r6. Logs: O:/winnt/Logs2/t423-s12-console-state-r6.log. This pinned
private parent channel is only startup transport, not NTSRV registry acceptance.
The new process still has no registered management row or production frontend
selection, and cannot yet be published as a complete backend.

Remaining next integration: authenticated NTSRV backend ownership and atomic
reuse; explicit Console-session stop/confirmation distinct from carrier death;
actual native launch result resources; NTCON real-screen synchronization and
NTMON projection. Keep retirement/admission races open until those paths are
tested. Carrier channel loss by itself never kills native clients.


No whole-product NTW32 runtime acceptance, publication or production-code
delivery is claimed. Retain S11's 115 model controls and real
identical-stream counterexample as negative regression evidence rather than
silently deleting the disproved approach.

### Registered executable and management RPC checkpoint

The formal generator now selects ntw32.exe, its private channel client archive
and ntw32-channel-test.exe. Existing incremental x86 /MT cache:
build/M0-T423/S1/restart-formal-x86. No published candidate is replaced.
APP_PROTOCOL_VERSION and MIDL interface advance from 9 to 10 for backend
registration/reporting and the snapshot process_id field; APP_VERSION remains
0.0.423. PID is display data, never management authority. Management still
selects the authenticated server epoch and registered sequence.

NTSRV keeps a native backend on its authenticated connection, separate from
original DOS/WOW records. One live backend per frontend generation is enforced
under the existing service lock. Native stop sets the owner's request event
and waits for actual Console-closed acknowledgement and carrier completion;
carrier TerminateProcess is not used to manufacture success. Existing DOS/WOW
management termination remains unchanged.

Local service test: basesrv-service-reservation-test.exe --native-backend,
O:/winnt/Logs2/t423-s12-native-registry-r1.log, passed owner/version identity,
event restrictions, duplicate refusal, members, stale management epoch,
unacknowledged-stop timeout without carrier kill and rundown. This local test
did not prove transported event access rights.

Real RPC test entrypoint: tests/observation/ntw32_channel_test.c, formal
ntw32-channel-test.exe with the absolute sibling ntw32.exe argument. It starts
only its own sibling ntsrv.exe, refuses an existing broker, and uses a real
ConPTY without touching the physical desktop. The fixture acts as the frontend
owner; no unregistered mode is compiled into the product. The carrier validates
its pinned pipe creator against the authenticated frontend owner, registers,
reports actual members and only then sends ready. Startup has a ten-second
deadline; this is not a native-target lifetime timeout.

Observed run sequence, retained rather than rewritten:

- registered-channel-r1/r2: error 87 at backend registration. Copied management
  events lacked the query-state right required by NtQueryEvent validation.
- r3: passed after adding only the needed query right (closed stays non-writable
  in NTW32); foreign capability and duplicate instance are rejected.
- r4: fixture incorrectly equated CreateProcess return with completed Console
  attachment. Real membership initially returned zero; no product fallback was
  added. The fixture now waits boundedly for the actual child attachment.
- r5: passed separate registered executable, copied versioned control, clean
  disconnect and re-registration, actual attached member, management snapshot
  identity/start-time/count, and management-triggered real ConPTY close. The
  attached native fixture process actually exits before the test passes.

Logs: O:/winnt/Logs2/t423-s12-registered-channel-r1.log through r5.log.
Build logs: build/M0-T423/S12/console-state-r6/registered-build*.log. First
attempt lost compiler PATH to a differently cased duplicate environment entry;
the next sandbox Ninja was explicitly cancelled after inspection showed no
compiler children or output. Approved normal-host build exposed and fixed the
three stale MIDL v9 symbol references, then linked NTW32, NTSRV and NTMON.

Remaining: frontend production selection and one-backend reuse, launch resource
binding, continuous actual-membership observation, admission/retirement races,
real-screen handoff and NTMON UI completion. The test owner closing ConPTY is
not evidence that the production frontend already does so. Full inherited
regression, coherent eight-file publication and P delivery remain mandatory.

Final checkpoint rerun uses verify-ntw32-console-state.ps1 with BuildRoot
build/M0-T423/S12/console-state-r7 and FormalBuildRoot
build/M0-T423/S1/restart-formal-x86. O:/winnt/Logs2/t423-s12-console-state-r7.log
contains both NTW32-STATE PASS and NTW32-CHANNEL PASS; the registry negative
suite passes again in O:/winnt/Logs2/t423-s12-native-registry-r2.log. Member
reports now publish the same snapshot returned to the frontend, not a second
racing query. Documentation governance and git diff --check pass.

Tested SHA256 (not publication):

- ntw32.exe: 475C160178EBC8FCEB67C52ED2B9BD3BD44549DE47F420838E8A2086A927FDA4
- ntsrv.exe: 9F98CE5B3D4657ED2EEC173ED9114EB172DB14A69CDD3A9D1D2384800934645B
- ntw32-channel-test.exe: 6D9FE9DC1B57C3F0DDF57FE5A69F8156BEAA14ED9025901FFC66E143FE11DE84

### Native launch and production frontend migration checkpoint

Channel protocol 2 adds serialized launch begin/write/commit/abort. The selected
CreateProcess body is the existing native_console_launch.c, now called inside
NTW32's attached Console. The pinned frontend supplies finite stream and
authenticated capability attachments; the actual created process query/wait
reference returns to the existing request/launcher completion path. NTCON no
longer creates a PTY per native branch: one backend persists across DOS intervals.
The pre-migration WIP frontend/backend sources are preserved below
build/M0-T423/S12/native-migration-input-r1, not revived as a second provider.

Formal incremental build logs in that directory: launch-build-r1 through r3
and frontend-build-r1 through r3. frontend-build-r2 also rebuilds/relinks the
worker after the shared protocol version changed. Runtime selection is x86 /MT,
CCPU40 for guest paths. No guest/shared-library or published-package changes.

Real registered-channel test, native-launch-r3.log under O:/winnt/Logs2:
PASS chunked 20,000-character environment, aliased output/error pipe, real exit
37, foreign frontend rejection, incomplete commit/abort/reuse, actual members,
management snapshot and confirmed Console close. r2 failed because the test's
environment fill overwrote its equals sign; the fixture, not product semantics,
was corrected. r1 was the earlier attached-target launch checkpoint.

Production entrypoint tests/observation/verify-native-root-console.ps1 selects
the formal build and private-desktop console-startup-observer.exe. Logs2
t423-s12-native-root-r1 through r4 preserve these findings:

- Root CMD output, interactive input, shell, nested/interactive CMD, and prior
  Console history pass their exact text and direct-result checks.
- The old GUI fixtures omitted --wait although they asserted GUI exit 37.
  Both batches now explicitly request the already accepted synchronous policy;
  default GUI behavior is unchanged.
- r3 exposed a real missing-last-line defect: actual members reached zero,
  and the presentation loop also stopped painting queued native output. Input
  remains member-gated, but output/final refresh is no longer suppressed by zero
  members. Both GUI tests pass in r4, including three GUI boundaries and distinct
  character frontend identities before/inside/after them.
- The runner now observes NTCON and NTW32 retirement before cleanup. All eight
  cases have a retirement witness across r3/r4, not just launcher completion.
  A single all-cases rerun against final production inputs is still required.

Real guest staging is build/M0-T423/S12/p, mapped to unused Z: for original
DOS path constraints. The first handoff-r1 run started before the copy process
finished; locked-file copy failures exposed a mixed S11/S12 stage. That run is
invalid and not acceptance. The test process ended, the stage was recopied only
after checking no candidate process remained, and six executable hashes were
verified before handoff-r2. O:/winnt was never replaced.

handoff-r2 failed strict MEM text: real Console writeback was followed by an
immediate DOS operation that repainted an older asynchronous VT projection.
After writing actual native cells/cursor, the same frontend screen transaction
now imports that state into its projection. This is not a parser-only substitute
for the remote write. Rectangle tiles also respect whole-row write constraints.
handoff-r3 passes native-cmd-dos, dos-native-dos and both frontend-chain-a/b,
including intervening MEM text and nested native results 37/23. Entrypoint:
tools/audit/Verify-CommandExitStatus.ps1, OrdinaryFrontend, private desktop,
S11/input-r4/observer.exe, PackageRoot Z:, ProcessPackageRoot S12/p.

Exact tested candidate hashes at handoff-r3:

- ntcon.exe: 33458293A4006B9C0551D679880BDEB26660226B8E39076DDA8C416A04D651EE
- ntw32.exe: 73400614C151C8270CC23BC1BBC41C5393B1A1040052C22BE7A550AE43FC951E
- ntsrv.exe: 9F98CE5B3D4657ED2EEC173ED9114EB172DB14A69CDD3A9D1D2384800934645B
- ntvdm.exe: 423BE6CAA2D5F38CCC08794B9CE504580D0D0A88E21D25663772031E83745C80

Subsequent guards check truncated module paths, invalid screen geometry and
release a malformed capture response; frontend-build-r4 compiled them. Tests
above do not establish arbitrary concurrent-output ordering,
full screen modes/cursor shapes, management UI, admission races, Window/mouse,
twelve-target chains, final WOW frontiers or coherent eight-file publication.
Those remain open S12 gates, not deferred out of scope or implicitly passed.

Console17: all seventeen expected text/result cases pass in
Logs2/t423-s12-console17-r1-summary.json using the handoff-r3 candidate,
OrdinaryFrontend and original G7.COM test fixture from S11/branch-r2. The
subsequent test-runner change includes NTW32 in candidate identity and cleanup;
it does not retroactively change that completed run's scope.

Window17 r1 is incomplete: empty passes; native-zero returns zero but the final
observer snapshot reports ERROR_RETRY (1237), so Merge-ConsoleTextSnapshots
correctly rejects it. The observer readback versus buffer-teardown timing needs
investigation; neither the case nor the remaining Window matrix is passed.

NTMON now renders NTW32 PID, member count, state and local start time in its
existing horizontally scrollable detail row. Native confirmation explicitly
closes the Console session, rather than describing guest tasks. The unchanged
DOS row/confirmation remains selected for DOS. tests/observation/
monitor_layout_test.c adds active/idle/closing/details/confirmation assertions;
the x86 hidden-private-buffer fixture passes, retaining the existing 80x25,
scrollbars, selection, colors and empty-state checks. Build log:
native-migration-input-r1/monitor-build.log; runtime:
Logs2/t423-s12-monitor-layout-r1.stdout.log. Real NTMon interaction is pending.

The next stricter real-guest concurrent-output check FAILS. Entrypoint:
tests/observation/verify-native-guest-output.ps1 with Observer S11/input-r4,
PackageRoot Z:, ProcessPackageRoot S12/p, BuildRoot S12/concurrent-r1 and prefix
t423-s12-concurrent-r1. The native target returns 37, but the visible snapshot
contains NATIVE-BASE, NATIVE-DURING-DOS, DOS-INTERVAL and DOS-RESUMED, missing
NATIVE-AFTER-DOS and its final PASS line. Native/DOS visible ordering also
requires investigation: the fixture releases native output after DOS-INTERVAL.
The strict text test fails, not a success inferred from process completion.
The local projection correction is therefore necessary but does not prove
the full simultaneous-writer/final-output contract. Next work must resolve
actual native Console state versus delayed ConPTY repaint ordering, not add a
sleep, waive text assertions or publish the candidate.

Concurrent-r1 candidate hashes after frontend-build-r4:

- ntcon.exe: 024AD57842DFE9178B6D795B827CF8EC666CA1B51DA6D923CC95F4CF7A4933C6
- ntmon.exe: 22D880EEB5330AC96070BED017BAA01102FDCD18F2976095B89E7D4A4FACFD9F

Other executable identities remain as above. O:/winnt remains S11. This is
uncommitted S12 implementation/research with documented failures, not closure.

## Ownership and exact-text-contract correction

Owner clarification requires NTW32 to create/manage/close ConPTY and own VT,
input and screen state. NTCON owns only visible Console/Window and routing.
Both text producers must use existing product-abi/console_video.h verbatim;
NTW32 publishes TEXT_FRAME only, with the same bitmap character mapping.
The proposal, architecture/layout and active brief now reflect this boundary.
The retained mixed implementation is not accepted production ownership.

Audit: ntvdm-exe/win32/console_text.c publishes console_text_style plus packed
glyph/attribute pairs. softpc/mvdm_softpc_text_video.c copies active EGA font
banks, including downloaded fonts. In contrast, ntcon-exe/window_frame.c uses
a private Unicode-to-PC map and generated 14-line ROM font to rasterize native
text into a graphics frame. That native path must be superseded, not retained
behind a renamed RPC. Font-bank/default-font equivalence and text-only wire
tests are added as explicit remaining gates. Arbitrary Unicode glyph coverage
is not represented by the current byte-glyph payload; preserve backend state
and explicitly audit existing Console Unicode acceptance rather than invent
a native-only frame ABI or silently claim equivalence.

Recent retained narrower evidence, independently reread:

- Logs2/t423-s12-console-row-r2.log: 9 row conversion, 28 screen-import,
  15 handoff, 17 Unicode and 13 concurrent assertions, zero failures.
- Logs2/t423-s12-screen-projection-r4.log: real registered backend viewport
  heights 15/14/22, history 0/1/1, each marker once and cursor 0,4; exit 37.
- Logs2/t423-s12-channel-r4.log: authenticated channel/launch/stream alias,
  incomplete-transaction reuse and confirmed session-stop assertions pass.
- Window native-zero remains a real duplicate-header failure, not merely
  observer instability. None of these narrow passes closes the full matrix.

### Can the ConPTY owner also access its real Console?

Test source: tests/observation/conpty_owner_attach_test.c. Reuses current
native_conpty.c and native_console_launch.c mechanics without changing them.
MSVC x86 /MT /W4 /std:c11; sources/include are repository inputs; objects and
test.exe reside in build/M0-T423/S12/owner-attach-r4. Runtime command:
`test.exe O:\winnt\Logs2\t423-s12-owner-attach-r4.log --running`.

The test owner detaches only itself, creates ConPTY and drains output, launches
an event-gated test target, attaches after the target reaches its ready event,
queries actual members, releases target execution, reads actual screen cells,
then detaches and closes its own ConPTY. It does not manipulate the desktop,
introduce a product helper or test arbitrary user processes.

- r1: attachment to CREATE_SUSPENDED target before initialization fails 31.
- r2/r3: after-ready attachment succeeds, members=2; target returns 91 because
  the fixture inherited redirected standard handles. This is not attach failure.
- r4: matches existing native_console_launch.c STARTF_USESTDHANDLES with NULL
  Console slots. Target exits 37, actual screen contains OWNER-ATTACH-OK,
  remaining members=1 (owner itself), detach/close succeeds. Log reports PASS
  and explicitly product-acceptance=no. Test SHA256:
  B7E4892A3CD832535C57F8A248EB850A0B9645DB786A189F87F9B80009D1A60D.

Conclusion: one ConPTY-owning process can attach and inspect its backend;
the pre-resume failure does not prove the architecture impossible. This
controlled ready event is test-only: arbitrary production applications do not
offer it. Race-free initial attachment, a target exiting before attachment,
retained descendants and screen initialization before target output remain
unproved and must be resolved before production migration. Do not add a helper,
Job, debugger or private Console API implicitly to make this test pass.
O:/winnt remains the accepted S11 package; no S12 production P is claimed.

### Shared text-frame producer checkpoint

Added ntw32-exe/text_frame.[ch], selectively reusing the existing private
PC437/control-picture conversion from ntcon-exe/window_frame.c. Windows Console
Unicode capture remains backend-local. The producer emits only the unchanged
NTVDM description/style/glyph-attribute ABI, copies both supplied font banks
byte-for-byte and supplies palette/cursor metadata. Original mirrors and shared
libraries are unchanged. This module is not yet connected to NTW32's production
channel; source presence is not runtime closure. The old frontend conversion
must be removed at switchover, not kept as a parallel provider.

Reproducer: MSVC x86 /MT /W4 /WX /std:c11 /Isrc, compiling
tests/observation/ntw32_text_frame_test.c, src/ntw32-exe/text_frame.c and the
unchanged src/ntcon-exe/console_video.c, with /Fo and /Fe under
build/M0-T423/S12/text-frame-r1. Run test.exe with the new log path
O:/winnt/Logs2/t423-s12-text-frame-r1.log. Result: 46 checks, zero failures.
This tests the real production NTVDM receiver without a native decoder:
description size/stride/kind, two copied font banks at heights 14 and 32,
glyphs, reverse colors, cursor, wide/surrogate fallback, complete-only chunk
publication and malformed geometry/size rejection. Supplied font bytes are
synthetic discriminating patterns: this proves lossless bank transport, not
default ROM selection or live DOS downloaded-font handoff.

An explicit outstanding contract gap was exposed: the accepted native renderer
used a second underlined font bank, but the unchanged DOS attribute byte has no
underline/grid flags. The new producer returns ERROR_NOT_SUPPORTED for those
flags rather than silently dropping them or modifying the supplied font map.
The negative test records this honest boundary; it is NOT acceptance of lost
native styles or authority to publish. Default fonts, these attribute styles,
Console Unicode preservation and live authenticated channel integration remain
open. Existing full native regressions must still pass before replacement.

Migration inventory (final ownership, not current implementation claims):

| Existing mechanism | Disposition |
| --- | --- |
| ntcon native_conpty stream/create/close | Move implementation and state to NTW32; frontend retains copied client calls only. |
| ntcon native_terminal parser/history/input encoding | Reuse in NTW32; remove frontend link to parser after migration. |
| NTW32 capture/members/launch/auth tests | Retain and adapt bootstrap ownership; preserve actual Console readback and lifetime assertions. |
| ntcon window_frame native_glyph/native raster producer | Supersede with NTW32 text producer and remove duplicate mapping; audit pointer presentation separately. |
| NTVDM console_video.h and NTCON console_video receiver | Retain unchanged common frame format and complete-publication rules. |
| NTVDM active EGA fonts/default ROM source | Retain original owner; bind equivalent backend font state without modifying guest or shared lib. |
| Frontend management HPCON-close acknowledgement | Replace with NTW32-owned close confirmation; no frontend HPCON remains. |

### Initial attachment race: deterministic negative result

Extended conpty_owner_attach_test.c with --exited: resume an ordinary test
target (no ready/release handshake), wait for its real process exit while
retaining the process handle, then attempt attachment. This models the owner
being descheduled until a fast target finishes, without PID reuse or guessed
delays. Build: MSVC x86 /MT /W4 /WX /std:c11, the same three sources as r4,
outputs build/M0-T423/S12/owner-attach-r5. Runtime:
`test.exe O:\winnt\Logs2\t423-s12-owner-attach-r5.log --exited`.
Observed target exit=37, output-bytes=139, AttachConsole fails with error 5.
This is a retained FAIL for the bootstrap strategy, not a passing product test.
It rules out relying on a fast post-launch AttachConsole retry as correctness.

Public contract review (2026-09-28):
[AttachConsole](https://learn.microsoft.com/en-us/windows/console/attachconsole)
attaches by an existing client's PID, while
[GetConsoleProcessList](https://learn.microsoft.com/en-us/windows/console/getconsoleprocesslist)
queries the caller's current Console, not an HPCON-selected remote Console.
[ConPTY creation](https://learn.microsoft.com/en-us/windows/console/creating-a-pseudoconsole-session)
creates the host resource before child process creation. None of these cited
interfaces supplies an AttachConsole-by-HPCON operation. This is not a claim
that all conceivable Windows interfaces have been proved impossible.

Proposed bounded resolution requiring owner decision: NTW32 alone creates its
ConPTY and starts a short-lived private bootstrap role of ntw32.exe attached
to it. The main NTW32 attaches while that role is explicitly held ready; after
verified attachment the bootstrap exits, leaving only one steady-state NTW32.
Only then initialize screen state and launch user targets. No extra executable,
per-task helper, observer, Job or scheduler; NTCON never receives HPCON. r4
proves the core attachment sequence but does not validate this product mode,
authentication, rollback or integration. The proposal's no-implicit-helper
rule requires explicit approval before implementing this additional role.
Do not silently substitute hidden Console or frontend ownership.

## Independent worker lifecycle audit after owner clarification

The owner subsequently directs NTW32 to be a worker peer of NTVDM, with the
same external startup/handoff/re-entry/completion/exit management, not bounded
by one NTCON lifetime. This changes the candidate ownership/registration model,
not merely its retention timeout. The bootstrap proposal remains unapproved;
that separate question does not prevent auditing the newly admitted lifecycle.

Actual selected-source comparison:

| Operation | Existing NTVDM owner/path | NTW32 candidate gap and disposition |
| --- | --- | --- |
| Admission and reuse | run16-exe/main.c BaseCheckVDM; base_service.c OpenNtBaseServiceCheck; original srvvdm.c DOS records | frontend_scope.c currently submits to NTCON. Replace frontend execution submission with NTSRV worker admission; keep backend-specific execution records separate from original guest records. |
| Create and authenticate | run16 main ReserveWorker, CREATE_SUSPENDED, PrepareWorker, BaseUpdateVDMEntry, ResumeThread; base_reservation.c claim by live OS process identity | ntw32 channel_client creates a frontend-bound carrier. Reuse reservation/prepare/claim mechanics, not a new PID-trust or process registry. Pre-handoff rollback remains distinct from post-handoff lifetime. |
| Worker identity and death | base_service.c OpenNtBaseServiceConnect claims reservation, registers one-shot service_worker_terminated process watch | native_root currently identifies frontend generation and management enumerates live RPC connections. Replace with independent worker identity/process watch; keep frontend binding separate. |
| Readiness and nested work | Check distinguishes an actual BaseSrvDOSWorkerWaitPending from merely VDM_READY; original GetNextVDMCommand owns guest re-entry | NTW32 request service must remain receptive while a direct target waits. Do not mistake a live or occupied worker for a pending command consumer, and do not SuspendProcess user targets to emulate DOS state. |
| Direct completion | original parent wait plus BaseCheckForVDM; worker death with missing completion yields ERROR_PROCESS_ABORTED | frontend_scope_wait_native currently waits a duplicated native process plus frontend paint receipt. Preserve actual Win32 exit code, but move completion ownership to worker/service; paint receipt is not execution completion. |
| Disconnect versus process exit | OpenNtBaseServiceDisconnect explicitly does not call VDM cleanup merely because RPC closes; retained process watch owns original cleanup | ntw32 main ends on its pinned frontend pipe/peer loss. Remove this as an independent worker's lifetime owner; classify reconnect, logical Console shutdown and worker failure separately. |
| Management | NTVDM watch identity plus source DOS/WOW record state; retained process rights and confirmed operation | native_root/native_members and frontend-owned close events are not parity. NTMON must consume the same worker management view, with NTW32 performing its own session shutdown. |

The reusable reservation implementation already separates launcher identity,
worker identity/generation, execution Console identity and stream receipts. Its
shared_wow discriminator and service claim's unconditional fVDM/update/cleanup
are VDM-specific: do not pass a native worker through them as pretend DOS/WOW.
Keep one management/transport path with narrowly selected original-VDM versus
native execution bindings. Do not create a second scheduler or mechanically
copy srvvdm.c into an autonomous native queue.

Safe implementation order within S12:

1. Extend the existing worker reservation/watch identity binding with explicit
   backend kind while preserving current DOS/WOW callers and negative tests.
2. Route native admission, ready/re-entry and completion through the same
   service/client lifecycle; reuse native payload/stream validators and direct
   target process completion. Cut the NTCON execution-submission dependency.
3. Bind independent NTW32 process startup and resource ownership; resolve the
   still-pending initialization mechanism without assuming approval.
4. Connect only copied input/text frames to NTCON and worker management to
   NTMON. Remove old frontend-generation ownership/close proxies after tests.
5. Verify worker/launcher/frontend failure separately, nested direct results,
   pending admission versus shutdown, existing screen/input regressions and
   the full eight-binary publication gate. No step alone closes S12.

Baseline verification this audit: Ninja target basesrv-reservation-test.exe
under build/M0-T423/S1/restart-formal-x86, MSVC x86 /MT. The current test source
was rebuilt and linked against existing reservation bindings. Runtime log:
O:/winnt/Logs2/t423-s12-worker-reservation-baseline-r1.log, exit 0 / PASS.
Source assertions cover claimant identity/generation, typed stream ownership
and EOF, claimed-worker survival on launcher abandonment, WOW null-console
identity, and termination of an unclaimed suspended test child. This is the
existing reusable mechanism baseline, not proof of native-worker integration.

### Common reservation implementation with explicit worker kind

Implemented the first migration step in existing base_reservation.[ch], not a
parallel registry: a service-local DOS/WOW/NATIVE discriminator and typed
CreateKind/ClaimWorkerKind entry points share existing creation, process-handle
validation, claim, abandonment, receipt and release implementation. Existing
DOS/WOW call signatures remain and delegate to that implementation. VDM-only
claim rejects a native reservation before changing its claimant generation;
it cannot accidentally run original DOS/WOW record handling on NTW32.
Native identity is an execution identity and does not require a frontend
process or generation. No native admission RPC is selected yet: this is a
production binding change with unit coverage, not connected NTW32 completion.

Expanded tests/adapter-basesrv/base_reservation_test.c checks native creation,
invalid kind/missing identity rejection, launcher generation, VDM-only refusal
without consumption, typed claim, claimed-worker survival, wrong-generation
reclaim/release and final registry emptiness. Existing DOS/WOW and stream/EOF
assertions remain. x86 Ninja rebuilt both basesrv-reservation-test.exe and
basesrv-service-reservation-test.exe in the validated formal cache. Runtime
Logs2/t423-s12-worker-kind-r1.log reports PASS, exit 0.

Eight existing service integration cases also pass (all exit 0):
reenter-before-return, reenter-after-return, reenter-nested-return,
reenter-pending-command, launcher-exit-survival, launcher-disconnect-survival,
completed-worker-exit and unfinished-worker-exit. Exact executable:
build/M0-T423/S1/restart-formal-x86/basesrv-service-reservation-test.exe with
each --case argument; hidden test process, no physical desktop interaction.
Logs: O:/winnt/Logs2/t423-s12-worker-kind-<case>-r1.log and .stderr.log.
These retain original source-level record semantics but do not substitute for
COMMAND/MEM/EDIT end-to-end regressions or the pending native-worker route.
No original mirror/shared library changes, package publication or P delivery.

### Independent native process watch: service-local verification

The existing base_service worker watch now records backend kind and admits a
native reservation without manufacturing original DOS/WOW records. Native
death bypasses BaseSrvCleanupVDMResources and original parent-result scans;
process identity, one-shot death notification, reservation release and watch
removal reuse the existing path. DOS-only frontend/management selections now
explicitly select DOS kind. Native management stop returns ERROR_NOT_READY
until its real Console-close control is bound, never kills only the carrier
and falsely reports session closure.

Added --native-worker to tests/adapter-basesrv/base_service_reservation_test.c.
It tests wrong generation, duplicate reservation, prepare/claim, copied monitor
identity, absent frontend and absent guest records, launcher disconnect, worker
RPC disconnect, retained live watch, actual test-process death and final empty
service. This is a service fixture; the suspended fixture child is not a
running NTW32 or a real native target. It does not certify product admission.

MSVC 14.43 Win32/x86 /MT incremental build uses
build/M0-T423/S1/restart-formal-x86 and targets ntw32.exe, ntsrv.exe,
basesrv-reservation-test.exe and basesrv-service-reservation-test.exe. All link.
The sandbox invocation could not launch compiler children; the same approved
build outside the sandbox succeeded. Its stalled Ninja was explicitly stopped.
Warnings are inherited C4201 anonymous structures in historical ABI headers.

Runtime: O:/winnt/Logs2/t423-s12-native-worker-r1.log, exit 0 / PASS.
Reservation tests pass. All eight previously listed service cases pass again;
logs O:/winnt/Logs2/t423-s12-native-watch-<case>-r1.log and .stderr.log.
NTW32 SHA256: 85BBC23C330204666B0CB64689FB356DB5D14430DEA461A20C8C0062CF67A0C4.
Service-test SHA256: 762C868C93385DFB9D4B4140B3B86B9C1FB35F0B6BED09924A567B171827C28C.

Remaining: connect authenticated native admission RPC and production entry,
move ConPTY ownership out of frontend, resolve the recorded startup attachment
mechanism, bind actual native ready/completion/close and common text frames,
then perform full runtime/publication gates. The current linked NTW32 still
uses its old frontend-bound entry. No publication, commit or S12 closure.

### Native reservation through authenticated RPC

Extended the existing Reserve RPC with a bounded native-worker discriminator;
native admission requires task zero, while existing DOS/WOW callers retain
their public wrapper and original reservation path. The shared client exposes
ReserveNativeWorker; Prepare/Release/Connect stay common. The broker still
validates the attached caller process and generation. No frontend capability
or guest record is required for this admission. Service IDL and product protocol
advance together to 11; application version remains 0.0.423. All candidates
must be rebuilt coherently before publication; old live services are not reused.

Incremental x86 /MT builds of ntw32.exe, ntsrv.exe and
base-client-rpc-first-test.exe pass in the same formal cache. Real RPC fixture
--native-reservation passes reserve, duplicate rejection, release/reuse and
stale reservation rejection against a test-owned candidate broker. Log:
O:/winnt/Logs2/t423-s12-native-reserve-rpc-r1.log (exit 0).
Existing --reservation-parent passes real launch/prepare/claim, frontend
delivery and original command retrieval with protocol 11. Log:
O:/winnt/Logs2/t423-s12-protocol11-reservation-parent-r1.log (exit 0).

An initial no-argument invocation failed at its obsolete unreserved Update
expectation (line 504, ERROR_INVALID_PARAMETER); the source explicitly names
--reservation-parent as its supported positive route. Retain that failure in
O:/winnt/Logs2/t423-s12-protocol11-default-r1.log and .stderr.log; do not count
the default invocation as passed or weaken product authentication to satisfy it.
The selected production NTW32 entry still needs migration to this RPC.

### Public run16 concurrent first creation

This result supersedes the earlier first-create/concurrent-admission pending
statement, not the remaining presentation and retirement gates. The reproducible
case is tests/app/base_client_rpc_first_test.c --ntw32-public-startup. A fresh
candidate broker and fixture-registered frontend/execution capability start two
real run16.exe processes suspended, then resume both. No native worker exists
before admission. Each public launcher selects/reserves/prepares/submits through
production code and executes real CMD, returning its exact 61 or 62 result.
After both launchers exit, service selection succeeds for one unique still-live
NTW32; the fixture explicitly terminates only that selected worker for cleanup.

MSVC Win32/x86 /MT incremental build: build/M0-T423/S12/public-startup-build-r1.log.
Runtime: O:/winnt/Logs2/t423-s12-public-startup-r1.log and .err, exit 0 / PASS.
Broker logs use the public-startup-broker-r1 stem in the same runtime directory.
This is real launcher/worker/RPC execution evidence with a fixture frontend,
not actual NTCON frame/input, DOS handoff, or full product acceptance. One
concurrent run covers this interleaving, not every possible retirement race.
No candidate publication, production P, or S12 closure is claimed.

### Actual Console capture through the common text-frame receiver

ntw32_presentation_capture composes existing console_state capture/read,
text_frame packing and ordered presentation sends, without implicit activation.
No new wire format or frontend font mapper was added. Geometry-change errors
remain explicit; all local buffers and the capture lease are released on errors.
This source is not yet called by the independent worker loop.

tests/observation/ntw32_presentation_test.c adds a real active Console buffer
with a known Z cell, cursor at 3,2 and caller-provided font bytes. It captures
and transmits to the existing production video receiver over a real named pipe,
then asserts cell/cursor/font identity alongside retained transport negatives.
The hidden test reports 111 checks, zero failures. Build log:
build/M0-T423/S12/capture-build-r4.log; runtime:
O:/winnt/Logs2/t423-s12-capture-r4.log (exit 0).

The first three runs retain failed setup evidence: requesting an 80x25 host
window returned 87; the current host reports maximum 53x14. The test now uses
the actual reported capacity without resizing the desktop or weakening payload
assertions. This is not a change to product geometry policy. Synthetic 80x25
frame validation remains in the same fixture. Full worker activation, native
input/screen handoff and published-package acceptance remain open.

### Production wiring dependency audit

Current source inspection identifies why merely invoking the tested frame sender
from NTW32 main is insufficient. This is source evidence, not a runtime failure
or completed fix:

- ntcon-exe/console_channel.c already supplies one authenticated console_io
  dispatcher and frame receiver; reuse this endpoint for both workers.
- native_console_frontend.c::apply_dos_binding allows only one channel owner
  and rejects a different active owner with ERROR_BUSY. Its inactive path still
  seeds the frontend-owned native backend. Native return must instead switch
  authenticated worker I/O ownership, preserving pending keyboard input and
  retiring only the old mouse route. Painting must never acquire ownership.
- ntsrv-exe/opennt/source/base_service.c::service_take_channel clears the
  pending native attachment after transfer. FrontendUsage subsequently counts
  original DOS records but no active native request or real Console members.
  session_service.c also still queries the old frontend-owned backend for
  members. Therefore native request delivery alone cannot protect a real
  frontend from premature retirement. The execution-only fixture root does
  not run that retirement loop and cannot prove this property.
- NTW32 main waits only for worker execution channels; it does not consume
  WaitFrontend/TakeFrontend. Public native submission does not yet request the
  presentation route. Both sides must change together, not activate a sender
  that has no production consumer.

Implementation order: reuse the authenticated channel for backend-neutral I/O
activation/return; replace old frontend-native usage with worker-reported actual
Console usage plus pending admission under the existing retirement barrier;
then bind NTW32 capture/input and final-frame acknowledgement to execution.
Required integration cases include native-to-DOS nesting, DOS-to-native return,
an unfinished native target after launcher exit, a remaining attached descendant,
and a launch racing the last-member retirement. No new execution scheduler,
helper, recursive kill or fake DOS record is authorized by this dependency audit.

### Native membership in frontend retirement guard

FrontendUsage now treats a live registered native backend with unknown or
positive reported membership as a pending presentation user. It does not add
native members to the original DOS task count. An explicit zero report releases
this particular guard. This reuses the existing registration/report and locked
retirement path, without a new record or protocol layout.

The --native-backend service fixture now proves unknown and positive states
both reject RetireFrontend with ERROR_BUSY, while zero releases the usage bit;
tasks stays zero throughout. Six service cases pass: --native-backend,
--native-worker, --frontend-channel, --reenter-nested-return,
--launcher-exit-survival and --unfinished-worker-exit. Build:
build/M0-T423/S12/native-usage-build-r1.log. Runtime logs:
O:/winnt/Logs2/t423-s12-native-usage-<case>-r1.log and .log.err, each exit 0.

These are service-fixture assertions with explicit reports, not proof of
NTW32's actual Console membership or full retirement. Independent NTW32 still
needs the production reporting binding. A stale zero sample concurrent with
new admission must be covered by worker/service idle synchronization before
retirement can be accepted; this guard alone does not close that race. No
publication, production P, or S12 closure.

### Reject reports after frontend retirement

ReportNativeBackend now resolves its registered root under the same service
lock used by RetireFrontend and rejects a closing, missing or dead root with
ERROR_PIPE_NOT_CONNECTED. It cannot resurrect a retired I/O association by
publishing a late positive member count. This is I/O failure only: no worker
or target termination is introduced.

The --native-backend fixture reports zero, retires the root, then verifies a
late positive report fails and the backend process remains alive. It and
--native-worker/--frontend-channel pass. Build:
build/M0-T423/S12/native-retired-report-build-r1.log. Runtime:
O:/winnt/Logs2/t423-s12-retired-report-<case>-r1.log and .log.err (exit 0).
This proves the retirement-first ordering only. Admission-first/stale-zero
synchronization and production member sampling remain open; it does not
certify the full race or change publication status.

### Native delivered-request lifetime

Protocol 14 adds CompleteWorkerChannel to interface/service.idl. The existing
authenticated worker connection counts successful native channel deliveries
until the recipient completes request cleanup; taking the pipe no longer ends
its frontend usage hold. NTW32 acknowledges malformed requests, normal target
completion and local cleanup, including failure to start its request thread.
This is an aggregate delivery-resource count, not a DOS record or scheduler.
Only the authenticated native worker may return a delivery; underflow is
ERROR_INVALID_STATE. A zero member report while deliveries remain is ERROR_BUSY.
Taking a new delivery invalidates the earlier member count to unknown.

The service --native-worker case verifies a delivered channel still prevents
retirement after frontend attachment is consumed, rejects a launcher/wrong
generation acknowledgement, and releases that hold after worker completion.
--native-backend and --frontend-channel also pass. The direct attachment
lifetime fixture stubs only its absent broker acknowledgement and retains
267 successful checks with no per-request handle growth. Actual NTW32/RPC
--ntw32-execution and --ntw32-public-startup both pass again under protocol 14,
including 37/19/73/61 results, 61/62 concurrent public startup and worker survival.

Build: build/M0-T423/S12/request-lease-build-r1.log (six EXEs and fixtures).
Logs: O:/winnt/Logs2/t423-s12-request-lease-<case>-r1.log and companion errors;
lifetime uses t423-s12-request-lease-lifetime-r1.log. All selected runs pass.
Independent NTW32 member reporting, coordinated fresh idle sampling and
production presentation remain open. In particular, an old sample delayed
past completion is not proved safe by this delivery count alone. The full
retirement race remains an acceptance gate. No candidate publication or P.

### Versioned native member samples

Protocol 15 adds NativeSampleEpoch and includes its returned 64-bit epoch in
ReportNativeBackend. Channel delivery and completion advance the worker-local
service epoch under the existing service lock; overflow is an explicit error.
A missing/stale epoch returns ERROR_RETRY before changing member state. Thus
a sample obtained before an intervening delivery/completion cannot overwrite
the new membership state. This is an observation generation, not a task ID,
scheduler or claim that Console queries are atomic against arbitrary clients.

The --native-worker fixture proves completion advances the epoch;
--native-backend rejects missing/mismatched epochs and retains zero/nonzero
and retired-root checks; --frontend-channel also passes. The real RPC
--ntw32-public-startup case still returns 61/62 and retains one independent
worker. Six EXEs build under protocol 15. Evidence:
build/M0-T423/S12/member-epoch-build-r1.log and
O:/winnt/Logs2/t423-s12-member-epoch-<case>-r1.log; public uses
t423-s12-member-epoch-public-r1.log. Selected tests exit 0.

At that checkpoint no production NTW32 sampling loop consumed this API; the
following production test supersedes that wiring status, not the open full
idle/admission, frame/input, monitor-close and publication acceptance gates.

### Production member sampling and concurrent RPC context

The independent NTW32 entry now registers its authenticated frontend association
and samples its actual Console members, excluding the resident carrier. Each
sample queries the service epoch before GetConsoleProcessList and submits that
epoch; stale/busy observations retry. Reattachment invalidates previous samples.
No helper, process-tree termination or invented DOS record is introduced.

The first real --ntw32-execution run failed its new idle-usage assertion:
O:/winnt/Logs2/t423-s12-live-members-ntw32-execution-r1.log and companion
.log.err. Default serialized RPC context access let blocking WaitWorkerChannel
exclude completion and member sampling on that same worker connection.
interface/service.acf now marks those four operation parameters as shared
context access. Other operations retain exclusive access; service locks still
protect mutable fields. Close/rundown are not made concurrent with active calls.
The first ACF syntax attempt failed MIDL compilation; r3 is the corrected build.

Build evidence: build/M0-T423/S12/live-members-build-r3.log. Real RPC
--ntw32-execution and --ntw32-public-startup both exit 0 in
O:/winnt/Logs2/t423-s12-live-members-ntw32-execution-r2.log and
t423-s12-live-members-ntw32-public-startup-r2.log. The execution fixture now
asserts zero frontend usage after target completion while the worker remains
alive. Service cases native-worker, native-backend, frontend-channel,
reenter-nested-return, launcher-exit-survival and unfinished-worker-exit all
exit 0; corresponding logs are t423-s12-live-members-<case>-r1.log in Logs2.

This is production sampling evidence, not complete lifecycle acceptance.
Rebinding and monitor deduplication changes still need focused runtime proof;
actual Console closure, descendant/admission races and frame/input wiring remain
open. The stop event is not yet serviced and closed is deliberately never
signalled, so management cannot falsely report success. No S12 publication.

The subsequent --native-worker service test now registers membership on the
independent watched worker and asserts exactly one monitor row with that same
PID/generation and member count. It disconnects the old frontend, registers a
new one without recreating the worker, verifies the sample epoch advances,
rejects the old zero report and accepts a fresh zero report. The monitor still
has one unchanged worker identity. --native-worker and --native-backend pass
in O:/winnt/Logs2/t423-s12-rebind-monitor-<case>-r1.log and companion errors;
build: build/M0-T423/S12/rebind-monitor-build-r1.log. This is service-level
rebind/deduplication proof, not a real frontend screen/input reattachment test.

### Native input through the existing frontend protocol

presentation.c now consumes one bounded CONSOLE_IO_READ_INPUT batch and passes
validated native INPUT_RECORD values to the recovered ntw32_input_write binding.
Its field mapping follows ntvdm-exe/win32/console_client.c; no alternate wire
format, worker-base input implementation or implicit activation is introduced.
Validation covers the complete batch before Console mutation. Malformed or
partially delivered batches latch failure and are never replayed. Accepted
records are not claimed to have been consumed by a native target. DOS-relative
device events are not Windows Console input records and remain rejected; native
Window coordinate conversion must be supplied by the proper input route.

The retained real-pipe/hidden-Console presentation fixture now passes 155 checks:
keydown/keyup and button-free absolute mouse motion reach the actual input
buffer; an invalid final record prevents the entire batch from entering that
buffer, and a subsequent call retains the error without another transport read.
Previous frame/cursor/font/cancellation checks still pass. Build:
build/M0-T423/S12/input-endpoint-build-r1.log. Run:
O:/winnt/Logs2/t423-s12-input-endpoint-r1.log (exit 0, failures=0).
Worker-loop activation, pumping, screen seeding and return are still pending;
this is an endpoint contract test, not full native program interaction.

### Frontend screen import into the real native buffer

ntw32_presentation_seed reads the existing SCREEN_INFO, GET_CURSOR_INFO and
READ_CELLS_W operations into bounded copied cells. It validates geometry and
reads the final screen state before changing the local Console; changed state
returns ERROR_RETRY. It then reuses ntw32_screen_apply/ntw32_cells_write to
apply geometry, cursor and cells to the actual backend buffer. The caller must
hold the execution handoff before starting/resuming a target; this API does
not acquire that ownership or claim atomicity against concurrent text writers.
The existing screen-info ABI contains no palette/font banks, so those remain
an explicit frame/style integration requirement rather than fabricated values.

The extended real-pipe/hidden-Console fixture passes 193 checks. A scripted
frontend supplies a 20x8 screen; actual buffer readback verifies first/last
cells and cursor 4,2, then WriteConsole continues at that imported position.
When the source cursor changes during capture, the method returns ERROR_RETRY
and preserves the destination marker/geometry. Prior input/frame tests pass.
Build: build/M0-T423/S12/screen-seed-build-r1.log. Run:
O:/winnt/Logs2/t423-s12-screen-seed-r1.log (exit 0, failures=0).
This proves the backend import endpoint, not production frontend handoff,
font/palette transfer or full DOS/native continuity. Those remain S12 gates.

### Independent worker acquires its presentation endpoint

NTW32 now links presentation/text_frame and its worker-side loop takes the
existing authenticated frontend attachment without blocking command reception.
TakeFrontend joins the shared-context RPC operations. Acquired pipe, frontend
and ready handles are worker-owned and closed only after the loop joins.
Opening sends a barrier, not activation; NOT_READY/BUSY is valid inactive
attachment. Stop cancellation covers the initial pipe exchange.

Actual --ntw32-execution now installs a broker-authenticated route and checks
the real worker's first barrier version/generation/sequence, replies NOT_READY
and verifies worker survival. Existing CMD results, launcher/public reuse and
idle member clearing still pass. The first run failed because the fixture
attempted redundant selection while owning a reservation; the corrected fixture
reuses that authenticated admission. Builds: runtime-endpoint-build-r1.log and
runtime-endpoint-build-r2.log under build/M0-T423/S12. Passing runtime:
O:/winnt/Logs2/t423-s12-runtime-endpoint-r2.log (exit 0); r1 retains the failure.

Public run16 does not yet request this presentation route; the fixture supplies
it explicitly. Execution-edge activation, screen/input pumping, font/palette
handoff, management closure and full regressions remain open. No publication.

### Public launcher requests the shared presentation route

run16_frontend_scope_launch_native now requests the selected worker's existing
authenticated frontend route before submission. An already attached route is
reused; NOT_READY retries only before a native request is accepted. No input
pump, frontend ownership or task replay is added to run16.

The first actual execution test exposed a lifecycle defect: the former DOS
startup cancellation path invalidated an undelivered native route when its
short-lived launcher returned. Independent NTW32 must retain that I/O request
while its authenticated root and registered worker remain alive. Native routes
now retain their pinned worker and request identity for root-side delivery;
they do not require the departed launcher connection. Root departure still
removes them. DOS cancellation/tombstone behavior is unchanged. This is finite
attachment delivery, not a new execution registry or scheduler.

The real fixture no longer recreates the route itself. After actual public
run16 returns 61, it takes the original request, attaches a channel and receives
NTW32's barrier. --ntw32-execution and --ntw32-public-startup both pass with
the previous exact results and unique worker assertions. Logs:
O:/winnt/Logs2/t423-s12-public-presentation-<case>-r2.log and companion errors;
r1 records the exposed cancellation failure. Build:
build/M0-T423/S12/public-presentation-build-r2.log. Six service cases also pass:
native-worker, native-backend, frontend-channel, reenter-nested-return,
launcher-exit-survival and unfinished-worker-exit; logs are
t423-s12-public-route-<case>-r1.log under Logs2.

This supersedes the preceding public-request limitation. Activation and actual
screen/input pumping are still not connected to native launch/return, so public
interactive presentation and full S12 acceptance remain open. No publication.

### Explicit presentation execution-edge operations

presentation_begin serializes explicit activation followed by real Console
screen import; failed import attempts deactivation and returns the original
failure. presentation_end captures the real final buffer, completes a barrier,
then releases activation. Release is attempted even on capture/barrier failure;
that failure is not reported as successful completion. These endpoint operations
are not a scheduler or automatic activation on paint.

The hidden-Console/real-pipe fixture passes 238 checks. The normal sequence
imports a screen, performs WriteConsole at the imported cursor, captures the
final frame through the existing receiver and verifies the new character,
then verifies exactly one activation and release. A malformed source cursor
fails begin without destination mutation and still releases activation.
Build: build/M0-T423/S12/activation-endpoint-build-r1.log. Runtime:
O:/winnt/Logs2/t423-s12-activation-endpoint-r1.log (exit 0, failures=0).
Actual target startup/completion, periodic traffic, nested owner return and
final receipt still need binding. No publication or S12 closure is claimed.

### Prepare completion export before target creation

Execution review found that serve created the target before allocating and
exporting its completion event. Failure to export that event therefore returned
a launch failure after the target had already begun executing. The existing
NTW32-owned request executor now prepares that export first. This changes only
the admitted native attachment ordering, not original DOS policy or the rule
that a successfully handed-off target survives launcher/request cleanup.
No helper, new interface or termination mechanism is introduced.

The existing request-lifetime fixture now submits a valid launch with a real
sender process handle lacking PROCESS_DUP_HANDLE. Windows rejects the event
export with ERROR_ACCESS_DENIED. The reply contains no target/receipt handle;
a uniquely named event that the requested target would set remains unsignalled.
The target probe exits immediately if mistakenly launched; no user process is
enumerated or terminated. Existing blocked-read cancellation and handed-off
target survival cases remain. Result: 284 checks, zero failures and zero retained
handles in O:/winnt/Logs2/t423-s12-prelaunch-export-r1.log.

Graph/source identity was regenerated with New-T310OriginalSoftpcNinja.ps1;
prelaunch-export-graph-r1.log and prelaunch-export-build-r1/r2.log are under
build/M0-T423/S12. The x86 incremental build covers all six EXEs and the request
and RPC fixtures. Real --ntw32-execution and --ntw32-public-startup both pass:
exact 37/19/73/61 results, alias/EOF, failed-launch reuse, authenticated route,
and concurrent first-launch 61/62 with one resident worker. Runtime logs:
O:/winnt/Logs2/t423-s12-prelaunch-export-ntw32-execution-r1.log and
O:/winnt/Logs2/t423-s12-prelaunch-export-ntw32-public-startup-r1.log.

This test proves the event-export denial path, not atomicity of every possible
failure after CreateProcess. Requester death after creation still does not
authorize killing a handed-off target. Interactive activation, final-frame
receipt, native frontend ownership migration and complete runtime acceptance
remain open. The accepted O:/winnt binaries were not replaced.

### Remove native execution reception from the frontend service

The actual run16 path already selects and submits to NTW32, but NTCON's
session_service still contained the previous TakeFrontendChannel/native-request
execution receiver and request-lifetime list. That parallel receiver is removed.
The NTCON and frontend-video-observer links no longer include
native_console_request.obj. Frames still use the existing worker channel; no
new transport or worker scheduler is introduced. Historical request source and
other ConPTY inputs remain pending their individual migration, not production
ownership certification.

Retirement now uses OpenNtBaseClientFrontendUsage and RetireFrontend, whose
current service implementation includes native in-flight requests and actual
reported Console members. The frontend no longer queries its former local
ConPTY member count or owns a native request list. The existing service barrier
continues to reject a concurrent new admission; it is not replaced by a local
idle guess.

The existing frontend-scope-lifetime fixture removes the old receiver/member
substitutes entirely, so reintroducing calls would fail its link. Its new
retirement case holds service usage nonzero and proves the frontend thread
stays alive, then clears usage and injects ERROR_BUSY at the retirement barrier;
the next attempt succeeds and presentation drains exactly once. All prior
34-channel cleanup and launcher/frontend separation cases still pass. Runtime:
O:/winnt/Logs2/t423-s12-presentation-only-lifetime-r1.log and its stderr log.
This is a service-boundary unit test; actual Console-member evidence is in the
preceding independent-worker RPC tests, not supplied by this substitute.

Build graph and logs: build/M0-T423/S12/presentation-only-graph-r1.log and
presentation-only-build-r1/r2.log. The first link found a previously missing
fixture declaration for the launcher's RequestFrontend call; the fixture now
explicitly rejects that out-of-scope call rather than fabricating success.
NTCON, observer and lifetime test subsequently build successfully.
verify-frontend-link-ownership.ps1 passes against the current formal cache and
rejects an injected native_console_request.obj edge in NTCON, in addition to
its prior helper/rendering leakage controls. It explicitly does not certify
the still-pending ConPTY source migration. No publication or S12 closure.

## Vertical production integration, not acceptance

Owner direction: stop accumulating isolated success claims; close the real
frontend -> NTW32 -> DOS/native return -> cleanup chain and remove displaced
implementations. The first target remains COMMAND -> CMD -> exit -> COMMAND
-> exit, before the complete retained package gate.

The candidate frontend no longer runs its local native backend/pump. Both
workers use copied Console operations and the same text-frame receiver;
direct protocol 14 distinguishes activation input interpretation without
changing text-frame shape. NTW32 execution calls begin/end, and its membership
loop pumps real hidden Console input and output. The production/observer link
excludes native Console backend/capture/view and frontend-terminal.lib.
Remaining source/renderer deletion and link-test updates are not complete.

Build evidence: build/M0-T423/S12/vertical-graph-r1.log and
vertical-build-r1.log (six x86 EXEs plus frontend-video-observer); subsequent
NTW32 incremental builds vertical-build-r2.log and vertical-build-r3.log pass.
Candidate staging is build/M0-T423/S12/p, mapped as Z:, not O:/winnt.
The unchanged package media are retained; no guest changes or desktop input.

Procedure: MVDM_OBSERVER_PRIVATE_DESKTOP=1, then
tools/audit/Verify-CommandExitStatus.ps1 with Observer
build/M0-T423/S11/input-r4/observer.exe, PackageRoot Z:/,
ProcessPackageRoot O:/repos.hobby/ntvdm64/build/M0-T423/S12/p,
LogRoot O:/winnt/Logs2, Cases native-zero and OrdinaryFrontend.
LogPrefix t423-s12-vertical-r1/r2/r3 identifies the respective runs.
All three fail with observer timeout 0x53504354, not successful execution.

Run r1 displays real Microsoft Windows VER output but a DOS prompt overwrites
its beginning; final exit does not complete. The candidate transfers all 9001
scrollback rows repeatedly and run16 ignores its two-second presentation wait
result. This is an identified ordering risk, not yet proved the sole cause.
Run r2 caches acknowledged screen cells and publishes only changed cells;
the complete VER output and subsequent DOS prompt appear, but exit still
times out. Run r3 also avoids forwarding input while the observed native
Console client count is zero; it times out without the earlier line/input
witness, so it does not prove that fix or the complete return contract.

Next: diagnose the failed production handoff and startup witness, acknowledge
final presentation before completion, account for unread input and actual
native clients, then prove ordinary interaction/retirement and remove old
code. Existing isolated fixture counts do not certify this rewritten path.
No candidate publication, commit, push or S12 closure is claimed.

## Vertical handoff successor: real Console and Window regressions

The prior failures are retained above. Incremental build logs
build/M0-T423/S12/vertical-build-r4/r5/r7/r8.log record the subsequent
implementation: native request protocol 3 sends final presentation status on
the same request pipe; run16 no longer ignores a two-second receipt timeout.
DOS activation waits while native owns the screen. NTW32 returns only unread
Console keyboard records and only with zero actual native clients. Screen seed
uses bounded multirow rectangles rather than one RPC per scrollback row.

Same candidate build/M0-T423/S12/p and Z: mapping, private desktop, ordinary
frontend and observer as above. Runtime evidence remains in O:/winnt/Logs2:

- t423-s12-vertical-r5: native-zero passes, including final exit.
- t423-s12-vertical-r6: native-interactive-return passes; dos-native-dos fails
  its Windows-banner assertion despite completion. Not counted as a pass.
- t423-s12-vertical-r7: dos-native-dos times out with `iexit`; the extra input
  origin is not proved.
- t423-s12-vertical-r8: timeout before the input/startup witness; no unique
  cause is established and this is not dismissed as an environment issue.
- t423-s12-vertical-r9: native-interactive-return and dos-native-dos pass;
  exact native echo, Windows banner, MEM output and task results are checked.
- t423-s12-console-r10: all 17 retained Console cases pass.
- t423-s12-window-r11: all 17 retained Window cases pass.

For full suites omit Cases, pass GuestFixturePath
build/M0-T423/S12/vertical-regression-r10/G7.COM and ObservationTimeoutMs
60000. Window adds MVDM_OBSERVER_WINDOW_INPUT=1; both use
MVDM_OBSERVER_PRIVATE_DESKTOP=1. No owner desktop interaction occurred.
The guest COM is an authored test fixture, not modified product guest media.

Completion-client and scope-lifetime fixtures pass in hidden windows:
t423-s12-vertical-frontend-request-client-test-r1 and
t423-s12-vertical-frontend-scope-lifetime-test-r1 logs. Completion assertions
cover actual target result 37, delayed final success beyond two seconds,
final ERROR_WRITE_FAULT and completion pipe EOF. These are supporting fixtures,
not replacements for the real workload results.

Physical cleanup removes native_console_request.c/.h from NTCON and their
manifest/build entries. Updated frontend link ownership checks reject native
request, backend, capture, view and ConPTY/parser leakage. Regenerated graph:
build/M0-T423/S12/vertical-cleanup-graph-r11.log. Other displaced sources and
duplicate rendering remain pending; source deletion is not claimed complete.

Remaining mandatory vertical gates include surviving actual Console members
after direct-target completion, last-member retirement/admission races,
management stop, failure cleanup, deeper nesting and coherent publication.
In particular the current sampler still gates I/O on direct request users;
it must not strand a live Console descendant when that count reaches zero.
O:/winnt remains the accepted S11 package. No S12 commit/push or closure.

Window nesting successor t423-s12-nested-r12 passes all four selected real
workloads: native-interactive-return=1, dos-native-dos=1, frontend-chain-a=1,
frontend-chain-b=23. The chain cases check both MEM witnesses and native return
markers; they do not replace the separately required twelve-target chains.

The next candidate changes NTW32 membership pumping: direct request completion
checks actual Console clients before releasing presentation; the sampler keeps
serving attached clients after the direct request count reaches zero. A separate
presenting flag prevents screen reseeding over active native output. Final empty
membership is reported only after final screen/input publication and release.
Transient membership sampling failures are not treated as empty membership.
Build: build/M0-T423/S12/vertical-members-build-r13.log, x86 NTW32 success.
This invalidates affected prior runtime acceptance until rerun; dedicated
surviving-descendant and final-member tests remain required, not claimed passed.
The rebuilt completion fixture t423-s12-completion-r13.log passes final status
0/29/109 with the actual target result 37 in each case.

Latest candidate Window rerun t423-s12-members-r13 passes
native-interactive-return=1, dos-native-dos=1, frontend-chain-a=1 and
frontend-chain-b=23. This verifies retained nested return after the membership
change, not survival/interaction of a descendant left after direct-target exit.
The full 17-case suites must be rerun on the final candidate before delivery.

## Actual surviving Console client and retirement workload

tests/observation/ntw32_surviving_client_test.c is an ordinary native workload,
not a product helper/provider. The direct target starts a child on its real
inherited Console and exits 37. The child waits on a pinned parent process
handle, verifies 37, prints a prompt, reads `survivor` from the Console and
emits NATIVE-SURVIVOR-INPUT-OK before exiting 19. Thus the input witness cannot
precede direct-target termination. The existing observer supplies ordinary
Console/Window input through the production NTCON/NTW32 path.

Build target ntw32-surviving-client-test.exe; graph/build evidence
build/M0-T423/S12/survivor-graph-r14.log and survivor-build-r14.log.
Verify-CommandExitStatus.ps1 adds Cases native-surviving-client and
NativeSurvivorFixture pointing to that build artifact, copied only into the
candidate's tests directory. OrdinaryFrontend verifies one independent NTCON
and now requires its natural exit within 10 seconds after the surviving
client completes, before test cleanup. The direct run16 result must remain 37.

t423-s12-survivor-r14 passes Console interaction; r15 passes Window interaction
and the strengthened final-member retirement assertion. r16 fails before
input readiness with a blank Console. It is not counted as a pass. Its exact
test child was subsequently identified and stopped; the harness cleanup now
includes only this test fixture's path and observed process identities.

Source review finds a real stale-sample race: sampling members outside the
local I/O lock allows a pre-launch zero count to be combined with the later
completed-request count, releasing/reseeding over a surviving child's output.
The candidate now samples epoch/members inside the same local handoff lock as
begin/end. Broker epoch validation still handles admissions racing the report;
no process-tree tracking, additional helper or new scheduler is introduced.
Build evidence: build/M0-T423/S12/survivor-build-r17.log. Repeated Console/Window
runs are required; this race is not asserted to explain every prior timeout.

t423-s12-survivor-r17-1/-2 (Console) and -3/-4 (Window) all pass the full
surviving-client workload and natural NTCON retirement assertion. No candidate
publication or management-stop success is inferred. These results cover this
explicit real-client lifecycle, not all failure/admission races.

## Explicit management close and retained regression

t423-s12-full-r18-console and -window each pass all 17 retained cases on the
member-sampling candidate. The subsequent management change is newer than
these results and requires final package regression again.

NTW32 now handles its registered stop_requested event. It serializes Console
close against successful execution admission through CreateProcess, not against
target lifetime. The owner detaches, requests WM_CLOSE on its own hidden
Console window, and confirms window disappearance before acknowledging closed
and ending the worker. FreeConsole alone is not treated as closure with attached
clients. No process-tree traversal, helper, guest/library changes or NTSRV
Console ownership are introduced. Failed close never signals success.
This finite binding uses the documented distinction between detaching and
[closing the Console](https://learn.microsoft.com/en-us/windows/console/closing-a-console),
with actual host verification below rather than assuming the API call suffices.

Build logs: build/M0-T423/S12/close-graph-r19.log, close-build-r19.log and
close-build-r20.log. tests/observation/ntw32_close_test.c exercises the real
production close function with an attached test client: hidden window,
CTRL_CLOSE_EVENT receipt, client exit, window disappearance and repeated-close
ERROR_INVALID_HANDLE. O:/winnt/Logs2/t423-s12-close-r19.log passes.

tests/observation/verify-ntw32-management.ps1 starts the ordinary candidate
run16 -> NTW32 -> CMD on a private desktop and invokes the same authenticated
management RPC used by NTMON. It pins the worker and target process objects,
waits for their exit and checks launcher completion. The management fixture now
accepts an exact worker PID; absent a PID it requires a single row rather than
silently selecting the first among multiple sessions. Test cleanup is limited
to its isolated package and pinned processes. Real runs
t423-s12-management-r21 and -r22 pass, with CMD CTRL_CLOSE exit 0xc000013a;
r22 run16 returns ERROR_BROKEN_PIPE (109), not successful completion.
The newly explicit nonzero-launcher assertion matches this retained output;
t423-s12-management-r23 reruns and passes that assertion. The same candidate
also passes native-surviving-client=37 and dos-native-dos=1 in
t423-s12-post-close-r23, retaining normal interaction after the close binding
and serialized-launch change. These checks do not replace final full regression.

The monitor fixture's old wsprintfW introduced an unlinked USER32 dependency;
it now uses its existing CRT swprintf_s. monitor-build-r20 fails at that link;
monitor-build-r21/r22 pass. This is a test repair, not a product API change.
Real NTMON keyboard control, independent-session isolation, repeated stop,
close failure and remaining lifecycle gates are not certified by these tests.
No S12 publication, commit/push or closure has occurred.

## Independent-session isolation and displaced source removal

t423-s12-isolation-r24 runs verify-ntw32-management.ps1 with TwoSessions.
It starts two independent NTW32/CMD pairs on private desktops, holds the second
observer's input with a named test-only gate, and terminates only the first
worker's pinned PID through the authenticated management RPC. Both second-pair
processes must remain alive. Only then is input released; the second Console
must emit the exact ISOLATED-SESSION-OK line and return 23. This real workload
passes. The gate is solely in the test observer; production gains no observer
process, helper or special behavior.

The displaced native_console_view.c/.h have no remaining external callers;
the old native_console_capture.c/.h duplicate the Console binding now owned by
NTW32. These four tracked NTCON files are removed (395 deleted lines), along
with source-manifest and obsolete link edges. They remain recoverable from Git.
The existing native_console_capture_test.c keeps its assertions but now calls
ntw32_screen_apply/ntw32_cells_write and links the production NTW32 binding.
The native frame header no longer includes the deleted implementation header.
Other old backend/parser and duplicate bitmap-rendering code remains pending;
this is not a claim of total cleanup.

build/M0-T423/S12/cleanup-graph-r25.log and cleanup-build-r25.log prove x86
NTCON/NTW32 and the migrated Console test build. Frontend link-ownership
negative controls and component-minimization provider checks pass: no obsolete
view/Console writer/ConPTY symbol in NTCON, actual writer symbols in NTW32.
O:/winnt/Logs2/t423-s12-capture-r25.log passes real Console scrollback, Unicode,
palette, cursor, invalid-span and no-font-scaling assertions after migration.
Source deletion neither removes those assertions nor counts a renamed provider
as deleted capability. No guest/shared-library mutation or publication.

## Native TUI resize in the vertical path

The real NTMON workload failed in t423-s12-monitor-r25-console: the visible
120x9001 Console remained blank and the observer timed out before the title
input gate. The missing pre-input report was a consequence, not a pass.
NTMON's existing configure_presentation reduces its own buffer to 80x25.
NTW32's capture sender attempted BUFFER_SIZE before fitting the old frontend
viewport; console_frontend routes this to opennt_console_resize_grid, which
preserves rows but does not relax SetConsoleScreenBufferSize's viewport rule.
The sampler returned that failure, leaving the target waiting for input.

presentation.c now recovers the existing ntw32_screen_apply ordering over the
same copied operations: grow if necessary, fit the viewport, then shrink.
Neither the monitor, shared library, mirrors nor guest media are changed.
The unchanged real title/F3/exit assertions pass in both Console and Window:
O:/winnt/Logs2/t423-s12-monitor-r26-console.txt and -window.txt. The script now
reports a missing input-gate snapshot explicitly and includes the exact
candidate NTW32 path in its cleanup/identity set; it never kills arbitrary
Console members. This verifies ordinary TUI use, not the monitor's DEL action.

The existing presentation pipe fixture was updated for batched row reads and
the production copied screen operations. Its geometry peer rejects shrinking
below the old viewport, preserving all frame, font, cursor, input and protocol
failure assertions. t423-s12-presentation-r27.log reports 239 checks, zero
failures. This is protocol/unit evidence; the real NTMON runs above supply
the separate product/Console evidence. Build logs are resize-build-r26.log
and resize-fixture-build-r27.log under build/M0-T423/S12.

The real r26 native-interactive-return, interactive-native-dos-return and
dos-native-dos cases all pass (exit 1), including required output markers and
nested return. An earlier attempted invocation named nonexistent chain-a/b
cases and was rejected before runtime; it is not test evidence. The full r27
Console17 and Window17 runs each pass all 17 cases on the latest candidate.
The r28 two-session management test also passes: selected NTW32/target close
returns failure to its launcher, while the independent session accepts fresh
input, emits ISOLATED-SESSION-OK and returns 23. Font/palette handoff, remaining obsolete
source migration and the other S12 acceptance gates remain open; no P or
publication is claimed.

The unselected private-launch prototype src/ntw32-exe/launch.c/.h (106 lines)
has no caller or compile/link edge and is removed from the source manifest.
Its validated packet/stream/alias/CreateProcess duties are already selected in
execution.c; redirected-stream, EOF and exact-result tests exercise that owner.
The old private capability route is not retained as a fallback. Byte-identical
research copies remain at build/M0-T423/S12/superseded-ntw32-launch.c/.h; their
removal is not counted as a decrease against committed main (they were WIP).

## Twelve-target retirement and inherited WOW frontiers

The retained verify-twelve-target-chain.ps1 previously required frontend
retention after every target returned, explicitly for the old ConPTY design.
S12 instead requires empty frontend retirement while independent workers may
remain resident. The live topology, all 24 ENTER/RETURN records, original DOS
record depth/identity, direct results and every stage's visible output/input
assertions remain intact. The final check now waits for natural frontend exit;
candidate-only worker housekeeping is separate and verifies exact path/hash.
The explicit ProcessPackageRoot parameter prevents cleanup of unrelated images.

With current x86 frontend-chain-cui/gui/input fixtures (chain-build-r29.log),
both GGGWDWGGGDWD and GGGDDWGGGWWD pass. Evidence is
build/M0-T423/S12/chain-r29/summary.json and O:/winnt/Logs2/t423-s12-chain-r29-*.
Each chain has two distinct frontend identities, each stable within its text
group; final records show two empty READY native workers. The record parser
now checks native task/member emptiness too, with active/member/stale-task
negative controls. These are actual twelve-program chains, not chain-a/b
substitutes or static topology fixtures.

verify-wow-headless-frontiers.ps1 with the existing PackageNetworkProfile
passes the inherited three separate frontiers at t423-s12-wow-r30-*:
WINMINE main window, SOL original OOM modal and WRITE original OOM modal,
with no character frontend. The latter two remain incomplete applications;
headless observation does not prove gameplay. No guest/config change.
These r29/r30 runs precede the font-handoff change below and do not certify it.

## Copied active text configuration handoff

The previous native producer always used its initial 14-line font. The actual
frontend retained only channel-owned video pointers and cleared them on owner
handoff. SCREEN_INFO does not contain fonts or palette. Source owner recovery
therefore keeps the existing NTVDM-produced bitmap banks and text-frame ABI,
not a new font generator/renderer or imported USER/Console server policy.

interface/console_io.h advances the direct channel to 15 and adds a bounded,
revision-checked read of console_text_configuration (existing style plus 16
copied RGB palette entries). NTCON copies only the last complete text frame's
configuration, retaining no channel pointer. Cursor fields are not inherited.
The snapshot survives channel disposal; stale revision/invalid offsets fail.
Text frames are requested in Console mode as well because later native/Window
handoff needs the active guest font, not only the original default font.
Publication does not change display policy. Guest and shared lib are unchanged.

NTW32 reads the two bounded tiles only after acquiring I/O ownership, validates
their revision, size and font shape, imports the palette into its real Console
and uses the copied fonts for subsequent text frames. No snapshot means the
existing original-ROM default for an initial native-only session; it is not a
substitute for a snapshot that failed validation. NTCON still interprets no
Unicode and generates no new glyphs in this path. Existing text-frame layout
and one shared renderer are unchanged; the obsolete native renderer remains
until its accepted style coverage is migrated.

font-build-r31.log builds all six x86 EXEs plus the channel test. Protocol/real
Console tests at t423-s12-font-r31.log pass 303 checks, including exact two-bank
font bytes/palette, invalid font and inconsistent revision rejection.
frontend-text-handoff-test.exe uses actual NTCON storage, owner switching and
channel disposal; t423-s12-frontend-font-r32.log passes 22 checks. After the
zero-initialization-only test-source correction, font-build-r33.log and
t423-s12-frontend-font-r33.log repeat the 22-check pass. Latest frontend-link
ownership controls and component-minimization symbol checks also pass.
The fixture is host/unit evidence, not custom-font guest acceptance.
All six candidate EXEs were copied coherently to build/M0-T423/S12/p; the prior
tested six are retained under pre-font-r31. No O:/winnt publication occurred.
The real Window native-interactive-return and dos-native-dos cases pass at
t423-s12-font-roundtrip-r31, including actual text and exit 1. Full regression,
custom-font/attribute completeness and remaining cleanup still gate delivery.

## Vertical closure and common style receiver

r34 repeats all 17 real Window DOS routes on the protocol-15 font candidate;
every text/result assertion passes. This predates the following source change
and does not certify protocol 16.

The original common cell has a glyph and eight color bits; the old native
renderer independently selected an underlined font bank. Reusing bit 3 would
conflate brightness with underline. The owner explicitly approved a minimal
common protocol extension. Channel 16 accepts original pairs or optional
triples with one backend-neutral underline flag. DOS keeps pairs. Unknown
style bits are rejected before publication, preserving the previous complete
frame. No guest, mirror or shared-library changes. Grid styles remain
unsupported rather than gaining speculative rendering behavior.

The duplicate native_glyph and native rasterizer are physically removed from
ntcon-exe/window_frame.c. NTW32 owns character conversion; NTCON uses one
copied-frame decoder and the existing library raster path. Underline precedes
caret inversion, independently of font banks and color. The existing pixel
test now composes the actual NTW32 packer and common decoder, preserving
V7VGA bytes, wide trailing blanks, viewport, underline, caret, pointer and
capacity assertions. Four cells additionally prove normal/bright foregrounds
with and without underline independently.

unified-build-r36/r38.log link all six x86 EXEs and affected fixtures.
Logs2/t423-s12-unified-r36-* reports frame 57/0, presentation 303/0 and copied
configuration 22/0 checks/failures. unified-frame-r36/r37/r38.log pass pixel
and keyboard tests. r35 passed pixels but failed compiling the retired
ConPTY controller fixture; that mechanism is no longer built by this pixel
script. Its remaining source/build migration stays open; no controller runtime
pass is claimed. Actual Window interaction has separate production-suite gates.
The generator's orphan launch.c edge is removed; graph regeneration passes.

Six candidate EXEs were backed up under pre-unified-r36 and updated only in
build/M0-T423/S12/p. Actual Window native-interactive-return and dos-native-dos
pass at t423-s12-unified-roundtrip-r36, including output and return to DOS.
r38 then tightens style flags to underline only; its 57/303/22 checks pass,
and t423-s12-unified-roundtrip-r38 repeats both real Window round-trip passes.
The six exact candidate files are updated after backup under pre-unified-r38.
Frontend link-ownership, documentation governance and diff whitespace checks
also pass. Full regression, lifecycle/error closure, old backend deletion
and coherent eight-file publication still gate S12. O:/winnt remains S11;
there is no S12 commit/push or closure.

## Displaced backend retirement and current regression gate

r39-r42 physically remove NTCON native_conpty, native_terminal,
native_terminal_screen and native_console_backend source/header pairs;
NTW32 channel_client/channel and control source/header; and the private
native_console_protocol header. Hash-checked pre-deletion copies are retained
under build/M0-T423/S12/retired-backend-r39. The build generator no longer
materializes or links the terminal parser, private client archive or obsolete
backend fixtures. Historical research-script retirement remains open.

Retained Window controller tests now exercise copied input and a real ordinary
Console rather than the deleted local ConPTY owner. r40 builds six EXE targets
and migrated fixtures; the controller passes on a private desktop. Console
state r41 retains buffer, member, descendant, geometry, raw/cooked input and
control-event checks. r42 pixel, keyboard and 85-cycle channel lifetime tests
pass, including handle balance. Earlier r40/r41 channel failures were stale
assertions for visible-Console input flushing and suppressed Console-mode
text-configuration frames; migrated assertions test the shared-channel
contract explicitly. All preceding failure logs remain retained.

Complete Console17 invocation t423-s12-cleanup-console-r43 passes empty,
native-zero, missing, native-seven, native-streams, native-eof, mem and
nested-empty, then FAILS nested-mem's exact marker count. The process exits
with expected result 1. Per-command captures show three MEM executions;
the final .console.txt shows only two. Intermediate buffers are 80x28;
the final capture is 120x9001. This establishes a screen-history or final-
observation discrepancy, not its cause. No assertion was relaxed. Diagnose
restoration/capture before rerunning; later suite cases were not reached.
O:/winnt remains S11 and S12 is not closed or published.

## Stream-mode regression isolated

r44 identifies the cause of r43: native_console_frontend's unconditional
TEXT_FRAME_REQUIRED response, introduced for font collection, makes original
nt_graph.c::nt_graphics_tick call disable_stream_io even in Console mode.
The finite 80x28 screen then loses prior stream scrollback. The single
production change restores Window-only requests; t423-s12-stream-ab-r44 repeats
the identical nested-mem test and passes the unchanged exact three-result
assertion. Its final 120x9001 capture retains all three results and banners.
This is not a guest or observer repair. The earlier r42 assertion that always
requesting frames was the correct Console contract was itself incorrect.

stream-ab-r44-build.log builds NTCON; the previous candidate is retained under
pre-stream-r44. Only the build-root candidate was replaced. The corrected
channel fixture checks Console=false, CAF Window=true, return Console=false.
t423-s12-stream-channel-r44 exits zero and passes all 85 lifetimes and exact
post-warm-up handle balance. t423-s12-stream-console-r45 then passes all 17
Console cases, including nested-mem, repeated MEM, EDIT, native streams and
exact exit results. All runs use the private desktop; O:/winnt is unchanged.

This restores stream semantics but does not complete font configuration
handoff when no text frame has been requested. Transfer that configuration
without forcing the original stream/video transition, then revalidate Window
round-trips and the remaining S12 gates. No functional closure is claimed.

## Configuration-only transport candidate

r46/r47 builds six EXEs with Console protocol 17. The existing BEGIN/DATA
transport accepts a configuration-only kind with zero geometry and only a
font/style payload plus palette. Completion stores configuration separately
from pixels and publication serial; partial/invalid payloads cannot replace
the acknowledged surface. Original stream flush invokes the copied producer
without disabling STREAM_IO (four added mirror lines, original CRLF retained).
Missing not-yet-initialized video/palette state remains absent, not fabricated.
The client suppresses identical acknowledged configuration sends and
invalidates that cache after a different successful video publication.

The producer fixture verifies configuration without screen backing, copied
dual fonts/palette, startup absence and transport failures. The r48 storage
fixture passes 32 checks including partial transfer, invalid font rejection,
unchanged pixels and copied storage surviving producer disposal. These are
unit/host evidence, not proof of custom-font guest execution.

After six-file backup under pre-config-r47, build/M0-T423/S12/p alone receives
the coherent protocol-17 EXEs. t423-s12-config-console-r47 passes actual
native-interactive-return, nested-mem (three final results) and dos-native-dos.
The preceding full Console17 r45 is protocol-16 evidence, not certification
of this changed candidate. Window regression and the remaining lifecycle,
cleanup and full publication gates remain required. O:/winnt is unchanged.

t423-s12-config-window-r48 also passes actual native-interactive-return and
dos-native-dos with screen and exit assertions on the private desktop.
Documentation governance and diff whitespace checks pass. No S12 publication,
commit/push or closure is inferred from these bounded results.

## Native presentation failure and remaining source retirement

r49 fixes sample_members returning on fatal I/O failure while main can remain
blocked in WaitWorkerChannel and native users await unreachable input. After
a fatal loop result, the launch lock protects an actual Console-member sample.
An empty idle worker can remain resident. Live users or failed member sampling
invoke the existing Console-owner close and terminate the failed worker itself;
there is no process-tree enumeration, helper, Job or launcher-death policy.
Normal requested shutdown is excluded from this fault path.

verify-ntw32-management.ps1 now has FrontendLoss fault injection using a pinned
candidate NTCON process, retaining the existing independent-session gate.
t423-s12-frontend-loss-r49 passes: affected CMD exits with CTRL_CLOSE result,
NTW32 exits, launcher reports 0x3e3, and the independent CMD echoes
ISOLATED-SESSION-OK before returning 23. This proves real frontend-loss cleanup
and isolation, not every possible sampler-error disposition. fault-r49-build.log
records the x86 build; pre-fault-r49 retains the prior NTW32. O:/winnt is untouched.

The obsolete native_console_frame.h has no production consumer and is removed
after a hash-checked snapshot in retired-backend-r39. Its two fields needed by
pixel assertions are now test-local, not a second production frame contract;
unused geometry/codepage fields disappear. retirement-r50-graph.log regenerates
the formal manifest without the retired header. Latest full Window regression
uses prefix t423-s12-fault-window-r50 and must complete before being called passed.

The r50 Window suite completed: all 17 cases PASS with actual screen witnesses
and required exit codes, including nested MEM and EDIT. The retired-header
pixel fixture also exits zero at t423-s12-retirement-pixel-r50, preserving
underline, cursor, font, graphics and malformed-frame checks. Its existing
oversized-transport limitation remains explicitly a limitation, not a pass for
that capability. These results do not close outstanding S12 delivery gates.

## Latest twelve-target return regression

contracts-r51-build.log builds the retained transport regression targets;
this is build evidence only, not a suite execution pass.
t423-s12-chain-r51-WDW-DWD enters all twelve stages but stops after RETURN 12;
its io-11-RETURN capture cannot find READY-11-RETURN. The outer target returns
test failure 91, not a successful chain. r52 could not replace the in-use
candidate: processes used Z: aliases, not physical-path names. No r52 test ran.
Exact alias/hash-validated failed-test cleanup excludes unrelated system ctest.

With only NTW32 replaced by pre-fault-r49 (same protocol 17), r53 WDW-DWD passes
all 24 enter/return events, I/O, topology and final empty records. Restoring
current r49 NTW32 and repeating at r54 fails after RETURN 5, missing native
READY-4-RETURN. These two failures and one control pass are evidence requiring
investigation, not proof that the fatal-error wrapper caused the regression.

The surviving r54 NTW32 hidden Console was read through the existing read-only
CINPUT --snapshot fixture at t423-s12-chain-r54-native-hidden.txt. It also ends
with GUEST-STAGE-5-RETURN and lacks READY-4-RETURN, matching the frontend capture.
Investigate native screen reseeding against direct-target completion/resumed
output; do not attribute this solely to rendering or relax the marker gate.
The current candidate is restored to the latest NTW32, remains unaccepted,
and O:/winnt is unchanged. The second chain case was not reached in r51/r54.

## Completion-side resume fence investigation

Source inspection distinguishes the two directions. Native target completion
waits for native_request_completion in run16_frontend_scope_wait_native, but
launch_vdm returns immediately after BaseCheckForVDM and resource cleanup.
NTW32's independent sampler subsequently calls ntw32_presentation_begin, whose
seed writes the prior visible screen into the hidden Console. A resumed native
parent can therefore write before that seed; there is no acknowledged fence
on the DOS-to-native return side. This is a concrete missing ordering contract;
the exact failed-run interleaving still needs a controlled reproduction.

Unbuilt source WIP introduces native request protocol 4's zero-payload resume
request, returning no target/receipt handles and performing no launch. It reuses
authenticated request transport and the existing begin/end callbacks. It is
NOT production-wired or accepted: run16 still has no caller. Selection audit
found OpenNtBaseServiceSelectNativeWorker deliberately rejects a connection
with a DOS task/parent wait, so direct reuse at launch_vdm return is invalid.
Do not weaken that check or infer authority from an environment PID. Resolve
the completion-side authenticated attachment before enabling this request.
Current build-root executables remain protocol 3; O:/winnt remains S11.

## Completion fence candidate r55-r58

The preceding unbuilt description is superseded, not a delivered P. Protocol 4
resume follows successful DOS BaseCheckForVDM only. Wrong/uncollected receipts
cannot authorize native selection; actual Console membership merely routes
the authenticated endpoint. No new target is created. r55 six-EXE x86 build,
333 request-lifetime checks (zero failures/retained handles), scope and service
fixtures pass. Resume success/begin failure/end failure export no handles.
Logs: build/M0-T423/S12/resume-build-r55.log and
O:/winnt/Logs2/t423-s12-resume-lifetime-r55.txt.

r55 still fails READY-4-RETURN. r56 stops earlier at ENTER 4. r57 trace proves
first-created DOS launcher 2612 completed but resume returned 5023, whereas
the reused inner launcher succeeded. pending_creation is a retained cleanup
obligation, not proof the task remains incomplete. r58 checks it under the
exact completion condition. Service tests cover first-created/reused tasks,
wrong receipts and rejection before completion.

Both actual r58 chains GGGWDWGGGDWD and GGGDDWGGGWWD pass all enter/return,
visible interaction, group identities, results and empty-record gates.
Inputs/summary: build/M0-T423/S12/chain-resume-r58. Runtime prefix:
O:/winnt/Logs2/t423-s12-chain-r58; opt-in trace r58.txt. Trace-free regression,
fault/cleanup and publication remain open. O:/winnt stays accepted S11.

Read-only original-source review: OpenNT/base/mvdm/dos/command/cmdexec.c
blocks DOS input before creating the inheriting native child, waits for
reentry and decrements BaseSrv reentry count on return. nt_event.c flushes
output, returns unused hardware/BIOS keys and restores Console modes.
nt_fulsc.c DoFullScreenResume/copyConsoleToRegen imports current shared Console
contents/cursor, not a saved pre-child page. Original srvvdm.c owns task
completion/reentry, not frame transport. Our extra hidden/visible Console
boundary must preserve that ordering.

Trace-free Console regression r59 and Window regression r60 both pass all 17
cases using the protocol 4 candidate (prefixes t423-s12-console-r59 and
t423-s12-window-r60). Both harnesses terminate with exit zero. Window CAF
witnesses identify the visible NTCON window and its input owner; ordinary
noninteractive cases retain the harness's existing route selection.
The client-only protocol fixture r60 exits zero: final presentation success,
failure and EOF remain distinct; resume accepts a zero-handle success, forwards
access denied and rejects a contradictory target handle as invalid data.
Build: resume-client-build-r60.log. Runtime:
O:/winnt/Logs2/t423-s12-resume-client-r60.txt. These are supporting protocol
checks, not substitutes for actual broker authentication or full S12 closure.

## Retirement and lifecycle verification r61-r69

r61 removes the uncalled ConPTY launch entry and its pseudoconsole attribute
branch from native_console_launch.c/native_launch.h. Ordinary Console handle
inheritance remains; run16 and NTW32 are rebuilt in the retained x86 graph.
The previous two candidate EXEs remain in build/M0-T423/S12/pre-retirement-r61.
Build evidence is retirement-build-r61.log. Request-lifetime and client checks
pass; real frontend-loss with an independent native session also passes under
runtime prefix t423-s12-retirement-fault-r61. This is not publication.

r62 passes the five retained real-guest mouse cases. Its pressure continuation
does not run because the broker is still present. After that broker exits,
r63 pressure fails: guest WINDOW-MOUSE-FAIL, exit 2, observer posted=fail with
last-error 203. The last-error alone does not establish which operation failed.
Logs t423-s12-pressure-r63-burst.txt and companions remain authoritative failed
evidence. No pressure acceptance is claimed from the five mouse cases.

The old frontend lifetime fixture expected an empty ConPTY frontend to remain
alive. It now requires natural empty-frontend retirement after exact native
37 and nested DOS 7 results; unrelated native work must remain alive until its
own release and then return 53. The obsolete ConPTY-host fault case is removed
from this suite, not counted as an NTW32-worker failure pass. The wrapper keeps
normal/frontend/launcher/DOS-worker cases and optional independent-session
isolation, adding NTW32 to exact-candidate post-verdict cleanup only.

MSVC x86 /MT fixture build: build/M0-T423/S12/lifecycle-r64/build.log.
r64 isolated normal fails before the guest-ready marker; the phase label still
reads independent-session-start. Its cause is unresolved, not waived by reruns.
r65 normal and r66 three fault cases pass in Console. r67 passes all four in
actual private-desktop Window mode. r68 passes all four with an independent
native session, including its survival/result/retirement assertions. Runtime
prefixes are t423-s12-lifetime-r64 through r68 under O:/winnt/Logs2. Authored
NOIO guest probes and unchanged original guest media are distinguished.

Native-worker unexpected death remains a separate open gate: production
run16_frontend_scope_wait_native waits indefinitely for the target before
checking the worker's final presentation stream. Source review therefore
identifies a possible stranded-launcher path when NTW32 dies but its Console
client lives. Explicit Console-close management tests do not prove that case.
Add a real failure reproducer and repair the wait contract without introducing
target-tree termination. O:/winnt remains accepted S11; no S12 P or closure.

r69 repeats the r63 mouse failure with the same old input-r4 observer.
That executable predates the current pressure observer source; retained S11
publication scripts instead selected pressure-r1/observer.exe. Rebuilt current
source in build/M0-T423/S12/observer-r70 (x86 /MT; build.log). r70 also fails,
but before injection: frontend=0, burst-records=0, no located test Window.
Therefore an observer-version mismatch is not a demonstrated complete root
cause or a mouse fix. Preserve all three failures and inspect startup/window
creation before rerunning pressure. r70 observer is terminal; any surviving
exact candidate processes require explicit identity-checked cleanup before
another integration run. No change was made to the production mouse path.

## Unexpected native worker death r71-r74

Extended verify-ntw32-management.ps1 with WorkerLoss, mutually exclusive with
FrontendLoss. It pins the exact candidate NTW32 and its real attached CMD,
kills only that worker, requires CMD to remain alive, and requires launcher
failure 1067 before test cleanup. TwoSessions retains independent input/23
result assertions. This is distinct from acknowledged Console shutdown.

The pre-fix r71 case fails: Direct launcher did not return after management
close (10-second bound). Production wait_native waited only on the live target;
its worker/root checks were reached only after target exit. The original
cmdCreateProcess direct-result wait is retained, but the admitted standalone
worker-failure contract additionally requires observing the authenticated
worker and frontend process capabilities. The narrow run16-owned wait now
waits on all three, target first; worker loss returns 1067, frontend loss
returns ERROR_PIPE_NOT_CONNECTED. No target termination or new scheduler is
introduced. Existing scope cleanup releases the local handles on failure.

x86 incremental build worker-wait-build-r72.log passes. Previous candidate
run16 is retained in pre-worker-wait-r72. The scope fixture passes under
t423-s12-scope-r72. Real WorkerLoss plus TwoSessions passes under
t423-s12-worker-loss-r72: affected launcher returns 1067 while CMD remains
alive; independent CMD accepts ISOLATED-SESSION-OK and returns 23. Test cleanup
then releases only pinned test processes, not a product process-tree policy.
This repairs the evidenced hang but does not close the remaining pressure,
full regression, source retirement or eight-file publication gates.

r73 passes real FrontendLoss with TwoSessions on the changed candidate:
affected Console targets close, unrelated CMD remains interactive and returns
23. r74 passes normal nested lifecycle with native 37, DOS 7 and natural
frontend retirement. Runtime prefixes t423-s12-frontend-loss-r73 and
t423-s12-lifetime-r74 retain actual report/output assertions. Neither result
is a replacement for the pending complete current-candidate regression.

## Pressure and current-candidate regression r75-r77

With current observer-r70, r75 passes all three unchanged real-guest pressure
cases (1000 burst, 1000 retirement, 200 latency). Hook/guest hashes match the
published fixture copies. This does not erase r70: its timeout-live record
contains the original illegal-instruction panel CS:03f4 IP:20c3, OP:63 6f 63
6b 72. Mapping its stack against the exact candidate ntvdm.exe.map gives
illegal_op_int -> host_error -> ErrorDialogBox. It failed before a test Window
was available, not after mouse pressure delivery. Root cause of that startup
failure remains unproved; do not classify it as an immutable guest limitation.
The decoded panel and stack come from retained read-only timeout evidence,
without clicking the dialog or changing the guest.

r76 completes the full trace-free Console17 suite on the changed r72 run16
candidate, with actual output/result assertions. Logs use prefixes
t423-s12-pressure-r75 and t423-s12-console-r76. Window17 r77 is started on the
same candidate; its result is pending. NTCON README now describes its current
presenter/service ownership and explicitly labels retained libvterm material
as research, not a selectable production backend. No publication or P yet.

The link-ownership verifier's red control exposed a stale-cache gap: injected
frontend-terminal.lib no longer had a graph definition, so source traversal
ignored it. The verifier now rejects that retired archive explicitly on a
traversed production edge, even if a previous build cache still contains it.
Actual graph and all direct/archive/backend/helper leakage controls pass.
This is a stronger test, not restoration of a terminal provider. Production
source and binaries are unchanged by this verifier correction.

r77 completes Window17 with all seventeen actual output/result cases passing,
using observer-r70 and the same r72 candidate. Component minimization and
binary-import verification pass after loading the x86 VS tool environment;
the earlier invocation without dumpbin was unavailable, not a passing check.
NTMON r78 and inherited WOW headless frontiers r79 are the next current-candidate
runtime checks. Source retirement, the unexplained startup failure and final
coherent publication remain separate open gates.

r78 passes actual NTMON Console and Window title/F3/direct-zero result through
NTW32. r79 preserves all inherited headless WOW frontiers: WINMINE localized
main window (interaction not retested), SOL original out-of-memory modal,
WRITE original Write out-of-memory modal, with no character frontend. These
are frontier non-regression results, not SOL/WRITE functionality passes.
Logs use t423-s12-monitor-r78 and t423-s12-wow-r79. Governance and diff checks
pass; current candidate remains unpublished and the T/S remain open.

## Retired backend test cleanup r80

Removed nineteen obsolete ConPTY/VT research/test sources and entry scripts
from tests/observation after byte-identical recovery copies were verified in
build/M0-T423/S12/retired-tests-r80. This includes uncommitted research, not just
Git-recoverable baseline files. manifest.json records each SHA-256 and length;
total saved content is 134506 bytes. Earlier S9/S11 references describe their
historical revision, not runnable current acceptance. No original evidence or
guest/shared-library input was deleted. Original source snapshots for their
removed providers remain in retired-backend-r39.

Replacement coverage is split by contract: ntw32_console_state_test and its
script exercise actual Console cells/cursor/modes/members; ntw32_text_frame,
native_console_capture and common pixel tests exercise shared text production;
frontend request/lifetime fixtures and real management fault tests cover
completion and failure; real twelve-target/Console17/Window17/surviving-client
workloads cover handoff and continuity. Old VT parser and ConPTY resource-only
assertions are retired mechanisms, not renamed NTW32 passes. Final integration
and cleanup review remain required.

Remaining source references to deleted native backend headers/bodies are only
the ownership verifier's intentional rejection controls. Governance and diff
checks pass after removal. Current build inputs and runtime binaries do not
change in this cleanup.

Recoverable test-file hashes:

| File | SHA-256 |
| --- | --- |
| conpty_admission_test.c | 5E412D1B4281C3338644E1E70D4CA23A705BE2C886304945A4DD3398BE34DFC3 |
| conpty_born_worker_test.c | D3A284EDA01A621F8E242C816A2A23DFFAFA99D39F89516CAF3F8F160363A487 |
| conpty_branch_test.c | 66A57C9240AA1D88E25F1689B206108BC68A31831CDA56F0674D06144168270C |
| conpty_cursor_inheritance_test.c | C9375E178095307DCEC974F738F82BB66228CD67010E4DA0D781137D3E19022D |
| conpty_cursor_handoff_test.c | 9531E6887AAB4D2636E4C98CD6981C55C992D3AA0CE6E444BD3779D26467F245 |
| conpty_job_membership_test.c | 63250E3D47CF3211C6BE4F6FF1E427762F2AE1A6CC79FC7ADC13B42E69161540 |
| conpty_launch_test.c | 905363F72F89BCE8B85D86305BF0590A368CA51A4FBDFF8FF65AF02242D56475 |
| conpty_lifecycle_test.c | 059C81BFCA7C6B23FCACD5CB66D778748DE83EDE03ED02C1D45FA8EAFB1F0462 |
| conpty_owner_attach_test.c | B722C3B0E80A946DDB5E93E05B6854B0D1C76C6B8518898FF432304C44B02E87 |
| conpty_screen_contract_test.c | 7A8CA6D5341C327E53576A0A29A5F098AB93DA4F7485E89AD99C6C97796B9021 |
| verify-native-terminal.ps1 | EAC0CA03636788650576DE352086B4E0B34EFD428F95838388B3499665847AFB |
| verify-native-projection.ps1 | F604FDB1609B950AE2C4B9A5754D55A2060AF4B5614F40B5A73FB1118E36ECCA |
| verify-conpty-launch.ps1 | 09CF7B082DEF4B923D79E37E825B6832DF675BF06022DE17C563E7FC51113F64 |
| verify-conpty-cursor-inheritance.ps1 | 30189267AF317739C9746F57159192559199C5BAC224ADF6C825D5E396EFC7E4 |
| native_terminal_test.c | 7F7FB45DFF96E01472D720758995AA0D51F3C96ABA6208F0B17F4A9AAF7BB205 |
| native_projection_test.c | 253474B38AA337850C85F97E168645F858EF4B59DF57D8B790DF6BE49951F89E |
| native_conpty_observed.c | 314755C0C72FE246D5FB2FCA3A95DB8A5062CC5793907D358C480EB32AC5BC43 |
| conpty_wheel_probe.c | 2229C0504D2C3F2227DC7E6E53CF3161ACBA884121A15203D75E73EFCC3E9554 |
| conpty_terminal_candidate_test.c | 295ED814F439D86C87423E46EFD4C5E37128A4807D090E8BABA6FF1FAA1CF5F4 |

## Full-chain and final-link consistency r81-r82

r81 passes both twelve-target workloads on the r72 candidate, including real
interaction, separate frontend identities, nested results and natural empty
record retirement. Summary: build/M0-T423/S12/chain-r81; runtime prefix
t423-s12-chain-r81. This is actual chain evidence, not a topology-only fixture.

The six-EXE dry run then identifies ntvdm.exe older than its updated
opennt-base-bindings.lib input. r82 completes that required link and VdmTib
owner/storage validation; all six targets subsequently report no work to do.
Build: coherence-build-r82.log. Previous NTVDM is preserved in pre-coherence-r82
(SHA-256 D7C678BC359315166EEA851B55982F0CF4385471D9A9841AF3AD26B6399BA70A).
New candidate hash: A24EBE06DBFECB981F4B08EAA8C0BC46DE4B53EF81EEBBC2A0CF86F737D63A05.
The changed artifact requires affected regression again; r81 and the preceding
mouse/WOW/DOS passes do not silently certify this new link. No O:/winnt update
or S12 P is claimed. Startup-illegal-instruction causality remains open.

## Original handoff recheck and mouse deadline investigation r83-r86

Read-only OpenNT review confirms one shared Console, not saved independent
screens. cmdexec.c::cmdExec32 blocks DOS input before cmdCreateProcess starts
the native child, waits its direct result and updates BaseSrv re-entry count.
The execution thread can receive nested DOS commands while that child lives.
nt_event.c::nt_block_event_thread flushes output, returns unused hardware/BIOS
keys, flushes mouse events and restores modes. nt_resume_event_thread restores
DOS modes and, outside STREAM_IO, calls nt_fulsc.c::DoFullScreenResume and
copyConsoleToRegen to import current Console cells/cursor, not an old snapshot.
Original ntw32 WriteConsoleInputVDMW sets Append=FALSE; server/directio.c
prepends the returned keys. These are split-backend ordering requirements,
not permission to import a Console server or a new scheduler.

Original files under O:/repos.external/OpenNT/base/mvdm, SHA-256:
dos/command/cmdexec.c: 98A941095D47AE73EF9731D389A433203C7184B0A016B62E24B202BB89F829D0;
softpc.new/host/src/nt_event.c: 7F555A87BA029627D6811C0F7B96964BB8A96AF90C60C89012975A4AC5442076;
softpc.new/host/src/nt_fulsc.c: E7DC683BE45A04B63D783AE91263F55D50BF41AAC480D7BAC1A302F19205934C.
Source evidence does not certify every current binding.

Post-r82 pressure r83 passes burst1000 but fails retire1000 with stage2 and
10375ms burst-to-exit. Posting succeeds; the probe did not observe the complete
movement/down/up mask within 180 BIOS ticks. Raw reports: O:/winnt/Logs2/
t423-s12-pressure-r83-*. Original nt_mouse.c::MouseEoiHook retains a 10000us
deferred IRQ interval (SHA-256
A73C0572FA1E44326D26B2F4D47C15BFF768B031FCA16DCFD789A35E65E0E48B).
One thousand events make a roughly 9.9-second deadline a plausible timing
confounder, not proof of missing release or a test defect.

Controls use unchanged r82 product artifacts, observer-r70, private desktop,
S7MOUSE.dll, MVDM_OBSERVER_MOUSE_RETIRE=1 and the named burst count:

- r84: burst200, original WMS7.COM passes in 3484ms.
- r85: burst1000, WMSLOW.COM with MOUSE_WAIT_TICKS=720 passes in 7579ms.
  This is inside the original budget; it does not prove deadline causality.
- r86: burst1000, WMDIAG.COM with original deadline and MOUSE_DIAGNOSTICS=1
  passes in 7547ms, callback-mask=7.

Logs use t423-s12-retire-diagnostic-r84/r85/r86 in the same runtime log root.
The independently authored tests/observation/window-mouse.asm gains optional
diagnostic defines, while its default binary remains byte-identical:
A60EDF938A866810CC3BD3D92800E72A5BEB73369C12580A14F1BEA8BA337E6A.
Build: nasm -f bin with the above optional define and -o below
build/M0-T423/S12/mouse-deadline-r85. Diagnostic probes are staged only in
the build-root candidate tests directory. No original guest, shared library or
production mouse code changes. No O:/winnt publication.

Passing controls do not erase r83. Next capture the failing callback mask and
consumption timing before changing IRQ scheduling or acceptance budgets.
Startup r70 and final coherent regression remain open; no S12 P or closure.

## Real Console unread-input return coverage r87-r89

The source-first handoff audit finds existing frontend queue/prepend tests but
no focused test of NTW32's real Console drain followed by multi-batch return.
ntw32_presentation_test now supports --input-return on an observer-owned
private desktop. It detaches the inherited Console, allocates its own real
Console, explicitly opens CONIN$ and binds stdin, then writes
2*CONSOLE_IO_INPUT_CAPACITY+3 distinct key records. The production
ntw32_presentation_end/return_unused_input path must observe no other Console
members, drain the actual records, send three tail-first PREPEND_KEYS batches,
and leave the real input queue empty. The copied-protocol peer reconstructs
those batches and checks every key, repeat/down flag, count and order. This is
production sender plus real Console evidence; its peer is a test fixture,
not an integrated frontend/guest acceptance claim.

r87 initially fails because AllocConsole preserves inherited startup standard
handles; the test had used that stale stdin instead of its new CONIN$ handle.
After correcting only the fixture setup, r88 passes 667 checks with launcher
result zero. The original sixteen cases rerun as r89 and pass 303 checks.
No product source is changed by this test addition.

Build: MSVC x86 /MT via VsDevCmd, ninja -C
build/M0-T423/S1/restart-formal-x86 ntw32-presentation-test.exe.
Run: observer-r70/observer.exe <formal>/ntw32-presentation-test.exe Z:/
<log-prefix>-observer.txt --observation-timeout-ms 60000 <log-prefix>.txt
[--input-return], with MVDM_OBSERVER_PRIVATE_DESKTOP=1.
Runtime log prefixes in O:/winnt/Logs2 are t423-s12-input-return-r87,
t423-s12-input-return-r88, and t423-s12-presentation-r89 (without the switch).
The pre-existing signed/unsigned comparison warning at test line177 remains
visible; compilation/link succeeds. Raw failed r87 evidence is retained.

## Latest linked-artifact text regression r90-r91

Both Console17 r90 and Window17 r91 pass all seventeen output-gated cases
after the r82 NTVDM relink. Script: tools/audit/Verify-CommandExitStatus.ps1,
observer-r70/observer.exe, -PackageRoot Z:/, physical package
build/M0-T423/S12/p, -OrdinaryFrontend -ObservationTimeoutMs 60000 and authored
vertical-regression-r10/G7.COM. Private desktop is enabled; WINDOW_INPUT is
unset for r90 and 1 for r91; S34 diagnostic tracing is unset for both.
Logs and seventeen-row summaries are under O:/winnt/Logs2 with prefixes
t423-s12-console-r90 and t423-s12-window-r91. Each expected result equals the
actual result and the verifier's required output assertions pass.
NTVDM SHA-256 is A24EBE06DBFECB981F4B08EAA8C0BC46DE4B53EF81EEBBC2A0CF86F737D63A05.
These runs certify the named text routes on the current link, not the still
open startup fault, mouse-pressure cause, chain/fault closure or publication.

## Owner-prioritized architecture cleanup

Owner directs that mouse pressure be investigated last; architecture and
handoff must be cleaned first. The already-running r93 pressure invocation
terminates with Observer failed. No new pressure run follows this direction.
Preserve its logs and the test-only acknowledgment candidate for later review;
it is not a proven fix for r83. No production change/publication is implied.

Current call-site audit identifies these cleanup items:

| Surface | Observed state | Required disposition |
| --- | --- | --- |
| Native execution submission | run16 production calls the worker path; native_request_client.c still selects SubmitFrontendChannel through a boolean route. Only old fixtures use run16_native_request_submit/submit_receipt. | Migrate or retire obsolete fixtures, then delete frontend execution and its unconsumed service path. Preserve the distinct live worker-to-frontend presentation attachment. |
| Launch/source ownership | NTW32 execution and run16 GUI creation consume native_launch.h/native_console_launch.c physically under ntcon-exe. | Reconcile launcher/request source ownership and build edges. NTCON retains presentation/bootstrap only; no generic common library or implementation in interface/worker-base. |
| Result versus I/O completion | run16 waits the target, then reads native_request_completion from the request pipe. The separate receipt event is closed, not waited, by that production path. | Audit all consumers and remove redundant event state where the authenticated stream provides the barrier. Preserve failure cancellation and final-frame ordering. |
| DOS/native handoff | NTCON dos_pending blocks native reads until final capture/release; NTW32 begin seeds its actual Console; run16 DOS completion requests native resume. | Verify one stop/drain/publish/import/resume chain, retaining OpenNT task completion/re-entry. No new scheduler, independent saved screens or target suspension. |

Inspected: native_request_client.c/.h, native_console_launch.c, native_launch.h,
native_console_frontend.c, run16 frontend_scope.c/main.c, NTW32 execution.c/main.c,
NTSRV service_submit_channel/service_take_channel and wrappers, plus
frontend_bootstrap_test.c, frontend_request_client_test.c and
frontend_scope_lifetime_test.c. Findings are not completed migrations.
Retained stream/auth/exit-code coverage must move with its owner, not disappear
to make deletion easy. Full publication gates still apply.

## Worker-only execution cleanup r94-r98

The obsolete frontend execution route is removed from native_request_client,
Base RPC client/server, service declarations and service.idl. The only native
submission recipient is the admitted NTW32 worker. Frontend identity alone
cannot choose an execution recipient. The live RequestFrontend/FrontendRequest/
TakeFrontend presentation attachment is retained. No original DOS/WOW policy,
guest, shared library, rendering or mouse code changes in this cleanup.

Removing RPC methods changes operation numbering: service IDL major and shared
APP_PROTOCOL_VERSION advance together from 15 to 16, including both interface
specification references. r94 first build exposed retained versioned symbols;
r95 fixes them and builds all six x86 EXEs and six affected fixtures. Preserve
the failed build. Exact source/test backup is architecture-r94-before under
build/M0-T423/S12 (fourteen files).

Coverage follows the real owner:

- frontend-request-client-test retains target 37, rejection, version,
  contradictory/partial reply, delayed final-frame completion, completion
  failure/EOF and no-target resume, now with only worker submission.
- base-service-reservation --worker-channel replaces --frontend-channel:
  worker reservation, generation/object/type/pipe-creator rejection, occupied
  slot, separate presentation wake, bidirectional bytes, restricted context,
  delivered channel survival and pending sender/root rundown.
- monitor-rpc-test retains real RPC object/generation/type negatives and proves
  frontend identity without worker admission cannot submit or receive execution.
  Positive worker execution belongs to the ordinary CLI/NTW32 integration.
- frontend-bootstrap-test retains startup rejection, authentication, retirement
  barriers, creator capability release, distinct owners and broker loss. Its
  obsolete frontend-native creator/final-frame branches are removed. Real native
  creator-loss and final output remain in verify-frontend-lifetime and ordinary
  Console/Window regressions, not a frontend executor surrogate.

r95 passes request-client, scope-lifetime, native-worker and frontend-root.
The migrated worker-channel fixture initially fails its old IsEmpty assertion:
disconnecting an independent worker's RPC must not remove its live process.
r97 corrects the test to assert nonempty until that exact fixture worker dies
and its watch retires; production lifecycle is unchanged. r97 passes
worker-channel, bootstrap startup-rejections, bootstrap normal and monitor-rpc.
Each successful observer report has exited/0 and the matching PASS screen.
Logs: O:/winnt/Logs2/t423-s12-architecture-r95-* and r97-*.
Build logs: build/M0-T423/S12/architecture-r94-build.log through r97-build.log.
r96 linking was blocked by the suspended failed-fixture child; verify its exact
command before terminating it, then retry the same link. Raw failures remain.

Six EXEs are backed up at architecture-r98-package-before and updated only in
build/M0-T423/S12/p for regression. O:/winnt remains accepted S11. Protocol 16
does not imply closure of source ownership, redundant receipt, handoff/fault or
publication gates. Mouse pressure remains deferred by owner priority.

Post-cleanup regression: Console17 r98 and Window17 r99 each pass seventeen
output-gated cases; their -summary.json records contain seventeen matching
Expected/Actual pairs. verify-frontend-lifetime r100 passes normal, frontend,
launcher and worker loss in Window on this candidate. Logs use prefixes
t423-s12-console-r98, t423-s12-window-r99 and t423-s12-architecture-life-r100
under O:/winnt/Logs2; lifecycle records are also under
build/M0-T423/S12/architecture-lifecycle-r100. These are not full S12 acceptance.
The six EXE targets are current (Ninja no work). Link-ownership checks including
the new forbidden frontend-execution API guard, component minimization and
documentation governance pass. Against architecture-r94-before, the nine
production files total +39/-126, a net removal of 87 lines. Original mirrors
and overlays are unchanged by this edit; test/document lines are excluded.

## Shared worker presentation client r101-r102

NTVDM and NTW32 now link the same ntcon-worker-client.lib, implemented only
by src/ntcon-exe/worker_client.c. src/interface/worker_console_client.h declares
the common API and borrowed-handle client state, not a wire record or renderer.
worker-base remains worker lifecycle only. Both backends share request ordering,
version/generation/sequence validation, cancellation and disconnect handling,
video transaction chunking and ordinary Console input decoding. DOS relative
mouse decoding and original guest keyboard/screen state remain NTVDM-owned;
native Console capture, unread-input return and actual-member checks remain
NTW32-owned. Sharing transport does not make these backend states identical.

Exact pre-edit source/build-checker snapshot: shared-client-r101-before under
build/M0-T423/S12. The x86 build r101 succeeds for six EXEs and affected fixtures.
Private-desktop fixture reports t423-s12-shared-r101-* under O:/winnt/Logs2 show
DOS client normal result 73, broken-pipe result 0 and close result C000013A;
NTW32 presentation 303 checks/0 failures, native unread-input return 667/0,
and frontend text-storage handoff 32/0. The latter is production storage but
not a real guest run; the named-pipe fixtures do not prove product activation.

r102 rebuilds the six EXEs after final shared-client argument validation.
verify-console-input-batch builds with the same shared implementation and passes
capacity, ordering, relative-motion and keyboard-expansion cases. This is a
batch fixture, not the deferred real-guest mouse-pressure acceptance. Build log:
build/M0-T423/S12/shared-client-r102-build.log; runtime log:
O:/winnt/Logs2/t423-s12-shared-input-r102.log. Link ownership and documentation
checks pass. Candidate binaries alone are backed up at
shared-client-r102-package-before and refreshed in build/M0-T423/S12/p.
Full runtime regression remains required; O:/winnt is still accepted S11.

Post-sharing candidate passes Console17 r102 and Window17 r103: each summary
contains seventeen Expected/Actual matches, including actual output witnesses.
Component minimization passes against the new shared-library link. Twelve-chain
r104 stops at stage 4 W with IDENTITY-FAIL 1306, before DOS handoff. The chain
fixture had not been rebuilt for protocol 16; r105 rebuild compiles its source
and relinks both CUI/GUI fixtures. The first r105 invocation correctly refuses
the still-live r104 candidate broker. Exact Z:/ntw32.exe PID 36060 and Z:/ntsrv.exe
PID 32096 are identified and stopped after matching the isolated package hashes;
then r105 continues. Preserve the r104 failure; do not count it as acceptance.

r105 then passes both GGGWDWGGGDWD and GGGDDWGGGWWD, including original DOS
record identities, actual visible input/output, direct nested results, retained
outer stack, independent frontend groups and final retirement. Source sharing
does not waive the remaining S12 gates.

## Owner-expanded worker-base provenance audit r106

The new owner instruction supersedes lifecycle-only placement: extract all
project-added worker mechanisms with equivalent full contracts, including the
existing common frontend client, without relocating original mirror policy.
Inputs are the current main worktree, S11 965083eec and S10 f98825653 provenance,
and read-only OpenNT/base/mvdm/softpc.new/host/src/nt_event.c. NTW32 and the
copied frontend transport are project implementations, not OpenNT originals.

| Mechanism / provenance | Current source | Decision and target / reason |
| --- | --- | --- |
| Standalone connect/watch/failed-watch disconnect; project bootstrap | worker-base/connection.c; both entrypoints | Already shared, retain worker-base; Base RPC authentication stays with NTSRV. |
| Ordered KVM request/reply, version/generation/sequence/length checks; project copied transport | ntcon-exe/worker_client.c | Move implementation to worker-base/console_client.c and retire ntcon-worker-client.lib; both production workers link worker-base. |
| Overlapped wait/cancel/drain and sticky disconnect; project client | Same common client | Share unchanged; borrowed handles and caller lock remain explicit. |
| Frame chunking/serial and ordinary input decode; project client | Same common client | Share unchanged; DOS relative device decoding stays NTVDM-specific. |
| Per-client event initialization/disposal; project adaptation | ntvdm-exe/win32/console_client.c and ntw32-exe/presentation.c | Extract init/dispose; own only event, borrow endpoint handles. Existing endpoint owners still close their resources after callers join. |
| Activation message and acknowledgment; project adapter | Both presentation clients | Extract one-attempt packet/call. NTVDM BUSY retry and mouse retirement, NTW32 admission/seed rollback remain local; no scheduler in common code. |
| Atomic returned-key wire encoding/count validation; project replacement for unavailable Console transport | Both presentation clients | Extract one batch in worker-base. Preserve NTVDM raw partial/error result and NTW32 all-or-error policy. Native multi-batch reverse prepend stays local. |
| Original ReturnBiosBufferKeys/ReturnUnusedKeyEvents, nt_block_event_thread/nt_resume_event_thread/DoFullScreenResume | Original and mirrored nt_event.c | Remain in mirror. DIV-313 project activation hook calls existing adapter/common client; do not move or bypass original block/resume order. |
| Project PC input batching, relative mouse and text publication hooks | Mirror nt_event.c, nt_graph.c DIV-311/313/314/316/317/318 and NTVDM adapters | Audit despite mirror location. Guest BIOS/controller/VGA behavior has no NTW32 equivalent; retain minimal hooks and local adapter. No mirror edits in r106. |
| DOS/WOW record completion, command reentry, guest failure cleanup | Original command and Base VDM owners | Remain original. NTW32 actual process wait/result is not equivalent and stays execution.c. Shared byte transport does not replace completion semantics. |
| Native request byte transport | ntw32-exe/channel_io.c | Not the same failure contract as KVM: completed I/O wins over peer exit, PROCESS_ABORTED rather than PIPE_NOT_CONNECTED. Native-only mechanism; do not silently merge into strict frontend transport. |
| Native members, unread-input draining, Ctrl-C, capture/seed/empty retirement | ntw32-exe/main.c, console_state.c, presentation.c | Native-specific. Cannot drain a Console with surviving consumers or emulate it using DOS BIOS state. Existing common screen service remains NTCON-owned. |
| Guest memory/TEB/CCPU/graphics teardown versus native Console session close | NTVDM bootstrap/session and NTW32 membership | Different resources and original failure order; keep backend-specific, not a callback framework. |
| Copied cell/font wire versus producer state | NTVDM console_text.c and NTW32 text_frame.c | Shared ABI/client, distinct producers: actual VGA glyphs versus native Unicode/cell attributes. Do not reconstruct guest fonts from native text. |
| Authenticated capabilities, broker disconnect/watch, startup rollback | NTSRV Base client and run16 worker_launch.c | Existing common owner implementations, already used by both kinds. Do not duplicate into worker-base or move consumer responsibilities. |

The extraction changes no original source or wire version. Shared client state
has no global current worker, type-selected scheduler, task records or process
tree policy. Callers own locks and endpoint handles; init owns only an event;
dispose runs after in-flight operations stop. Server BUSY is retryable, broken
transport is sticky, and cancellation drains OVERLAPPED before stack release.
Activation success cannot substitute for task completion. NTW32 retains its
explicit final capture/input-return/barrier/release before completion response;
NTVDM retains original paint/BIOS return/mode restore before its hook.

r106 six EXEs and affected fixtures compile. DOS client normal/broken/close
results remain 73/0/C000013A; NTW32 pipe fixture is 312/0, unread-input return
676/0, frontend storage handoff 32/0. New assertions prove client disposal closes
only its event, preserves borrowed resources and is repeatable. Link-owner gate
now requires connection.c and console_client.c through worker-base in both
workers, rejects worker-base dependencies in the four consumer EXEs, and keeps
renderer-leak negative controls (including worker-base archive contamination).

Standalone batch fixture r106 initially failed because its single cl invocation
overwrote two same-basename console_client.obj files. r107 compiles each source
to a distinct build-local object, then links; all existing strict FIFO/capacity/
keyboard/relative-input assertions pass. No test assertion was weakened.
Console17 r107 and Window17 r108 each pass 17/17 on the extracted candidate.

Protocol audit finds a pre-existing whitelist omission in the retained
pre-sharing NTVDM client: CONSOLE_IO_KEYBOARD_LAYOUT returns KL_NAMELENGTH bytes
from console_frontend.c, but the worker rejects every payload for that op.
r109 red fixture reproduces exactly one failure out of 335 checks. The shared
whitelist now accepts that existing response operation; r109 green is 336/0,
including rejection and sticky failure for a BARRIER with unexpected payload.
No wire layout/version or original guest semantics changes. Earlier runtime
passes are supporting evidence, not substituted for final-candidate regression.

Final shared-client candidate evidence:

- r110: both twelve-target chains pass actual I/O, group separation, nested
  return, original DOS identities and retirement (worker-base-chains-r110).
- r111: Window normal/frontend/launcher/worker lifetime tests pass with
  ExpandedFaults, including an unrelated live session in every case.
- r112-r114: real NTW32 management close, frontend loss and worker death each
  pass TwoSessions; the unrelated session accepts input and returns 23. Worker
  death returns 1067 while its actual native target remains alive.
- r115/r116: final-candidate Console17/Window17 each pass 17/17 output-gated
  routes; each mode additionally passes native-surviving-client with result 37.
- r117: execution lifecycle 333/0, twelve completed and sixteen cancelled
  requests, target-survival and zero remaining handles; request-client tests
  retain successful/failed/EOF final acknowledgment and resume negatives;
  scope-lifetime tests retain attachment and reclamation assertions.
- Six EXE links and VDMREDIR x86 build pass; WOW32 Ninja reports no work.
  Component-minimization, shared-owner leakage guards, documentation and diff
  checks pass. The rebuilt VDMREDIR is not silently substituted into the tested
  candidate; final eight-file coherence/publication remains a separate gate.

Logs use O:/winnt/Logs2/t423-s12-worker-base-* and the explicit r115/r116 suite
prefixes; chain/lifetime evidence is under build/M0-T423/S12/worker-base-*.
Against shared-client-r101-before, the two local clients change +39/-237
(198 fewer local lines). The single common implementation is 168 lines and
its declaration header 34 lines; relocation is not counted as deleted logic.
This is scoped extraction accounting, not the entire S12 net diff. No mirror
file is changed by this extraction; pre-existing nt_graph.c edits are preserved.
Remaining S12 source cleanup, mouse-pressure, WOW/package coherence and delivery
gates stay open. No S12 commit/push, formal publication or closure is claimed.

### Native creation owner cleanup r118-r121

Reviewed native_console_launch.c against all callers before moving it. This is
project-added packet/resource materialization, not OpenNT scheduling. NTW32
owns native text creation; run16 also uses the same body for GUI creation with
no character-session capabilities. That GUI caller must remain; NTCON has no
creation caller. Split unchanged function bodies into ntw32-exe/launch_packet.c
and launch.c, moving shared declarations to interface/native_launch.h. Removed
the old NTCON source/header. The client archive includes only the packet codec;
run16 explicitly links local creation for GUI, and NTW32 links both. No native
request, guest, task-result, screen or input policy was changed.

Exact normalized comparisons of both extracted function groups with the
pre-move source pass. r120 x86 builds all six EXEs and affected fixtures. The
ownership gate requires zero NTCON sources in either worker and rejects native
creation in the frontend/client archive; run16 may contain only the two finite
native launch sources, not NTW32 execution or presentation. The new stale-source
negative control initially exposed a missing verifier rejection in r118; r120
rejects it without weakening existing controls. Component minimization passes
after loading the MSVC environment (the first shell lacked dumpbin, not a code
failure). Actual execution fixture: 333 checks, zero failures, twelve completed,
sixteen cancelled, target survival and zero remaining handles. Client fixtures
retain actual target 37, final-presentation success/error/EOF and resume negatives.

r121 assembles the current six EXEs plus rebuilt VDMREDIR and unchanged WOW32
under the existing build candidate only; each copied hash matches its formal
build input. Previous candidate retained in native-owner-r121-package-before.
Real-package regression is in progress, not a passing result or publication.
O:/winnt remains the accepted S11 set. Receipt cleanup and remaining S12 gates
are not silently waived by this owner-only refactor.

r121 Console regression stops at its first empty COMMAND case: observer records
timeout 0x53504354 before visible prompt/input. The private-desktop dialog says
CS:03f4 IP:200b OP:63 72 69 70 74. This resembles retained r70's CS:03f4 startup
failure but does not prove a common cause. NTVDM and WOW32 hashes equal the
pre-move candidate; no NTW32 was started in this failing case. Observer/suite
cleanup completed, confirmed by a fresh process query before further tests.
Single-variable r122 swaps only VDMREDIR back and passes empty COMMAND; r123
restores the newly built VDMREDIR and also passes. Thus the DLL is not established
as the cause. r124 repeats full Console/Window regression on the full new set;
these runs cannot erase r121 or prove the unresolved startup fault fixed.

r124 completes Console17 and Window17, both 17/17 on the new eight-file build
candidate. Summaries: O:/winnt/Logs2/t423-s12-console-r124-summary.json and
t423-s12-window-r124-summary.json, with per-case output/interaction reports.
Normal repository-config git diff --check returns zero; documentation,
component-minimization and link-owner negative controls pass. NTCON's map has
no run16_native_launch_start. A deliberately overridden autocrlf=false check
produced CR-as-whitespace noise; no formatting or repository setting was changed
in response. Final process query finds no live product test processes.

This closes native creation's physical owner relocation, not S12 acceptance.
Next required diagnosis is the retained pre-prompt fault: preserve r121/r70
and obtain guest/state evidence on recurrence before attributing it to CCPU,
guest, DLL or current refactoring. Receipt/export simplification must preserve
pre-launch export-failure rejection, not merely delete its event. Remaining
chain/pressure/WOW/coherent publication and commit/push gates remain explicit.

### Startup failure attribution r125-r126

The r121 build shell inherited VsDevCmd's environment into runtime. Fresh-shell
controls did not. With the unchanged r124 eight-file candidate, ten ordinary
empty-COMMAND runs pass under `t423-s12-startup-r125-01` through `-10`.
Loading the x86 VsDevCmd environment reproduces a pre-prompt fault under
`t423-s12-startup-vs-r126`: CS 03F4, IP 203B, opcode bytes 63 72 69 70 74.
This remains a failed runtime, not an acceptance pass.

The test-only observer now optionally reads the live guest's first MiB on
timeout, before cleanup. `MVDM_OBSERVER_GUEST_BASE_RVA=2d5824` is taken from
this exact candidate's map for Start_of_M_area; it is not a product constant.
The option is absent by default. It reads only the identified NTVDM child,
validates image RVA/address bounds, uses a new per-PID file, and never writes
guest memory. It is not an atomic all-thread snapshot. Observer r125 compiled
successfully; its use here is diagnostic, not a replacement acceptance observer.

The r126 report's `.guest-8456.bin` has 1,048,576 bytes. Read-only assertions
establish PSP 03F4, environment segment 049F, MCB owner 03F4 and size 02EA
paragraphs: environment range [049F0,07890). The INIT-resident EnvSiz word at
PSP:203C, linear 05F7C, lies inside that valid allocation and now reads 6972,
not 02EA. The fault address is also within the environment. The retained
COMMAND allocation is only 00A2 paragraphs; INIT is no longer owned by it.

COMMAND.COM SHA-256 is
`908a77ac617c2d741f0aa1b73f73973dcf29adc91f092e5bcb02173c8c732c43`, identical
to the already diagnosed [S35 guest](m0-t420-s35-xms-capability-progress.md#direct-environment-copy-witness).
Its binary still compares BX with [203C] after the second BOP 54:0F, and the
error route jumps to discarded Alloc_error at 1F8D. Original rdata.asm::EndInit
shrinks the resident allocation before those references. The prior direct
before/after-copy witness proves this source defect; the current snapshot,
same binary and environment-controlled reproduction match it. Current
CONFIG.NT already has DOS=HIGH,UMB, so high DOS is not a general cure.

Register this as the existing immutable-guest limitation under standing owner
authority, not a shared-client regression or a host workaround. Do not alter
guest, truncate product environment, inject /E:, or special-case allocation.
Future normal runtime tests use their ordinary shell environment rather than
accidentally inheriting build setup; the explicit large-environment failure
is retained separately. r70 has no equivalent memory witness and is not
independently attributed merely by resemblance. Other S12 gates remain open.

### Final owner review and WOW frontier check r127

The NTW32 completion receipt is deliberately retained. execution.c exports it
before io.begin/launch_request, rejecting a caller lacking duplication rights
before a target has side effects. Final presentation status still travels over
the completion channel; deleting the event merely because that status exists
would remove the pre-launch export check. DOS record completion is original
policy, not an equivalent event to merge into worker-base. No new common
scheduler or generic backend callbacks are introduced to unify these operations.

The remaining input read loops also keep backend-specific responsibilities:
NTVDM preserves Read/Peek and relative-device events with its guest API's
partial/error contract; NTW32 consumes a batch and writes actual Console input,
with sticky failure after ambiguous delivery. Both use the one common wire
transport and ordinary event decoder. Their difference is not duplicated
transport hidden behind different names. Server-side screen restoration and
barrier completion remain the single NTCON service implementation.

`verify-wow-headless-frontiers.ps1` with observer-r70, the retained publication
window reader, current Z: candidate, Logs2 prefix `t423-s12-shared-final-r127`
and PackageNetworkProfile preserves all three separate frontiers. WINMINE
reaches its localized main window; SOL and WRITE retain their original OOM
modals. No character frontend is created. These are headless non-regression
results, not interactive play or complete SOL/WRITE functionality. No production
source changed between r124's eight-file DOS regression and this check.

The expanded worker-base request has a provenance/disposition table above,
both production links, duplicate-client removal and affected normal/negative/
nesting/disconnect tests. It does not complete S12: literal DDWWDDWW coverage,
final mouse pressure, coherent O:/winnt publication and reviewed commit/push
are still outstanding. Unrelated worktree edits remain preserved.

r128 subsequently closes the final mouse-pressure row on the unchanged full
candidate: verify-window-mouse-pressure.ps1 with observer-r125 and Z:/tests
passes burst 1000, retire 1000 and latency 200. Every row retains the strict
WINDOW-MOUSE-PASS guest marker, zero result, exact posted count and
input-sink-acknowledged=yes assertion. Logs use
O:/winnt/Logs2/t423-s12-shared-pressure-r128-*; these are private-desktop tests,
not physical pointer clipping/foreground observations. The earlier failed
pressure runs remain evidence, not erased by this result. Final formal x86
Ninja reports no work for all six EXEs, VDMREDIR and the two shared-client/
execution fixtures; link ownership and minimization gates pass again.
Only the explicitly remaining chain/delivery gates above remain open, not
this now-completed pressure check.

### Continuous eight-target chain and final matrix r129-r130

Added verify-continuous-worker-chain.ps1 using the existing real chain/input
probe, extended only to accept the explicit eight-stage DDWWDDWW shape and a
read-only post-completion service snapshot. Twelve-stage checks are unchanged.
The new case asserts all sixteen ENTER/RETURN visible-input/output checkpoints,
one authenticated frontend, the same DOS and native worker IDs across both
native intervals, original DOS depths 1/2/3/4, parent task identity restoration,
direct target results and final empty records. Root DOS RETURN is recorded
before its final guest PAUSE/exit, so the empty-record assertion deliberately
runs separately after the root launcher exits, not against a premature sample.

Both t423-s12-continuous-r129 and -r130 pass. Final snapshot retains an EMPTY,
READY NTW32 with zero tasks/depth; its frontend retires naturally. Explicit
candidate-only cleanup of that independent idle worker is housekeeping, not
an assertion that frontend exit should kill it. r130 then re-runs both original
twelve-target chains with the rebuilt probe: nesting, separate frontend groups,
actual input/output, original DOS identities/depths, results and retirement all
pass. Evidence is build/M0-T423/S12/final-chains-r130 and corresponding Logs2
t423-s12-final-chains-r130-* reports. No production source changed in this step.

### Final publication and closure r131-r132

Before publication, exact source-owner review reconfirms the original DOS/WOW
execution boundaries, startup-only rollback, typed attachments, final-output
fence, real native membership and independent-session fault coverage. No source
or build graph selects Create/Resize/ClosePseudoConsole, a private helper or
the removed native renderer/parser. Both worker links use worker-base. The only
executable mirror change against admission is nt_graph.c +4/-0, the registered
DIV-314 configuration-only publication after original stream flush. No OpenNT-
host or shared-library code changes; no guest media changes. The original
mirror formatting is retained. All extracted code remains classified by the
r106 provenance table; an original scheduler is not relocated.

Current component READMEs are reconciled to actual ownership, rather than
retaining contradictory unfinished migration descriptions. Side-chat Queue/WOW
proposals are included under the owner's standing authorization; they do not
admit new implementation. Current status is reduced to the closure baseline;
its previous detailed text is retained under publication-backup-r131 and the
chronology above. T423 remains open; S13 is not automatically started here.

No product/test process was live before copying. Original COMMAND/MEM/EDIT,
CONFIG.NT/AUTOEXEC.NT/system.ini and WINMINE/SOL/WRITE hashes agree between
candidate and O:/winnt. The old seven-file runtime is backed up under
build/M0-T423/S12/publication-backup-r131, alongside publication-manifest.json
with exact previous/published hashes and formal candidate paths. All eight
candidate hashes match the formal x86 outputs (unchanged WOW32 reuses its
verified formal cache). Copying is whole-set, with rollback on copy/hash failure;
no guest/configuration/NTVDM.REG overwrite occurs.

Published O:/winnt checks:

- Verify-CommandExitStatus.ps1, observer-r70, ordinary frontend and existing
  vertical-regression-r10/G7.COM: r131 Console17 and Window17 each 17/17.
  The initial invocation omitted GuestFixturePath and was rejected before
  starting a product process; the corrected complete invocations are recorded.
- r132 supplemental native-interactive-return and native-surviving-client pass
  in both modes (results 1 and 37), with actual transcript/interaction checks.
- verify-wow-headless-frontiers.ps1, PackageNetworkProfile, observer-r70 and
  the retained S9 publication-window-reader: all three inherited frontiers
  preserved, separately. SOL/WRITE OOM remains incomplete application behavior.
- verify-window-mouse-pressure.ps1, observer-r125, the hash-selected S7MOUSE.dll
  and WMS7.COM under O:/winnt/tests: burst1000, retire1000 and latency200 pass
  strict guest/sink acknowledgment. No desktop focus/clipping claim.
- Final eight hashes still match the manifest; process query finds no run16,
  NTSRV, NTVDM, NTW32 or NTCON test processes. Documentation governance,
  relative links and git diff checks pass. Remote main was synchronized before
  delivery; no force push is used.

Runtime logs use t423-s12-published-{console,window}-r131 and
t423-s12-published-{extra, wow, pressure}-r132 prefixes (without spaces).
The production P2 is the reviewed complete S12 change, not an earlier partial
candidate. Completion means delivered NTW32 and retained behavior, not full
WOW functionality or repair of registered immutable-guest limits. Await owner
side testing; do not close T423 or start S13 on this record alone.
