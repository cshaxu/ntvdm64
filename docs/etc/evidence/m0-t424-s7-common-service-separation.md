# T424 S7 common mechanisms and service ownership

T424 S11 naming normalization: frontend labels/current source links now use NTCON.
This does not claim the new basename existed in the recorded historical package.
Exact earlier source, commands and product names remain in Git and sealed build
evidence; recorded hashes, dates, results and limitations are unchanged.

## Explicit native-control common client checkpoint

### Copied service queue removal checkpoint

SubmitNativeRequest and FinishNativeRequest now also use the explicit common
client view; their old facade bodies are deleted. Status/exception mapping,
partial attachment release and 0/1 completion validation retain their contracts.
The expanded substituted-RPC test passes 108 checks, zero failures and zero
handle delta (`common-native-command-r002.txt`).

NTSRV no longer contains QueueNativeChannel/TakeWorkerChannel, a native
control-pipe field, pipe-type/server-PID validation, pipe duplication or a
copied/pipe branch. Its private queue owns copied bytes, including zero-byte
resume. A short output buffer does not consume bytes, attachments or Direct
record ownership. Existing service locks, authorization and wake order remain.

`tests/adapter-basesrv/base_service_fixture.c` compiles the actual service unit
only for the existing reservation fixture and exposes its private copied
queue/take operations there. It adds no production API or alternate policy.
The fixture explicitly selects that object; products select the archive member.
The graph gate rejects its injection into each of six EXEs; product maps contain
no fixture queue/take symbols. Ownership checks pass in
`copied-service-link-ownership-r002.txt`. The first attempt retains an incorrect
PowerShell multiline argument expression, corrected without weakening checks.

The migrated native-command fixture passes in
`copied-service-fixtures-r001-native-command.txt`. Former control-pipe
type/payload/EOF assertions are mapped to absent/oversize payload rejection,
capacity non-consumption, copied payload immutability, authentication, single
pending command, no presentation wake, wait-only capabilities, delivered
resources surviving caller rundown, and undelivered cleanup on caller/root loss.
Common I/O transport tests retain exact/partial transfer and EOF checks; real
RPC fixtures cover role/process/generation/capability authorization.

The retained native-registry fixture fails at line 601 before its copied-queue
assertions (`copied-service-fixtures-r001-native-registry.txt`). Its launcher
already retained the initial root, then requests a second root capability;
unchanged RetainFrontendRoot forbids this rebinding. Keep this as non-pass,
not a reason to loosen production authorization. Its root replacement/registry
scenario still needs migration/review against current broker retirement. That
r001 suite stopped on failure; its later cases were not run.

All six x86 EXEs and the same-source fixture link in
`build-copied-service-fixture-r001.log`. All nine real RPC cases pass in
`copied-service-fixtures-r002-*.txt`; actual native output/direct exit 37 passes
in `copied-service-native-product-r001.txt`. The full gate/service split remain
open; this is not a delivered P and O:/winnt remains unchanged.

The project-added client exchanges formerly implemented in
`ntsrv-exe/opennt/source/base_rpc_client.c` are now selected from
`common/rpc/native_command.c`: GetNextNativeCommand copied delivery,
NativeStartupResult and CompleteWorkerChannel final-result reporting. These
are standalone RPC additions, not original OpenNT DOS/WOW algorithms. Their
existing public facade binds the caller-owned connection through an explicit,
borrowed four-field view. The shared implementation has no registry, callback,
process-global state, target wait, reconnect or lifecycle authority. No original
mirror is changed. The replaced client bodies, including the private redundant
GetNext wrapper, are deleted rather than retained as alternative providers.

| Logic | Source/current owner | Shared decision | Retained independent owner |
| --- | --- | --- | --- |
| Copied native command RPC, output clearing and partial attachment release | Project BaseClient adapter; now common/rpc/native_command | Common is the sole selected client provider | Authenticated connection lifetime and original DOS/WOW state remain BaseClient-local; NTSRV authorizes delivery |
| Native startup RPC and status/exception return | Project BaseClient adapter; now common/rpc/native_command | Share explicit connection view, borrow target/receipt inputs | Worker owns creation/rollback; NTSRV verifies request/process/capabilities |
| Native completion RPC and status/exception return | Project BaseClient adapter; now common/rpc/native_command | Return real error unchanged; do not hide failed completion | Worker owns I/O/real target result; broker owns record/receipt and run16 completion |

Incremental MSVC x86 /MT links for all six EXEs and selected RPC fixtures pass
in `build-common-native-rpc-r003.log`, using the recorded CCPU40 cache at
build/M0-T424/S2/r001. The first new test link omitted its common/RPC libraries;
the second graph mistakenly listed the SDK library as a Ninja file dependency.
Both failed attempts remain in r001/r002 build logs. The final graph uses the
existing RPC test link rule with an explicit common archive. No production
behavior or test assertion was relaxed to repair these build selections.

`tests/app/common_native_command_test.c` passes 61 checks, zero failures,
zero handle delta (`common-native-command-r001.txt`). It verifies borrowed
connection forwarding, invalid capacity/output arguments, disconnected state,
copied bytes, successful attachment ownership, ordinary errors and raised RPC
exceptions with partial attachment release, borrowed startup inputs, request-zero
completion, and unchanged error propagation. This is substituted-RPC unit
evidence, not authentication or end-to-end proof. The graph owner/reverse-edge
negative controls pass in `common-native-link-ownership-r001.txt`.

Real private-desktop RPC client/bootstrap/startup-rejection/monitor fixtures
pass in `common-native-rpc-fixtures-r001-*.txt`. The five additional real
startup-timeout/native-worker-failure/completed-worker-loss/workerless-grace/
workerless-cancel cases pass in `common-native-rpc-fixtures-r002-*.txt`:
all nine retained RPC fixture cases are passing at this source checkpoint.
Documentation governance and diff checks pass in
`common-native-governance-r001.txt` and `common-native-diff-r001.txt`.
Actual run16 native CMD emits
NATIVE-RPC-STARTED and returns broker direct exit 37 in
`common-native-rpc-product-r001.txt`. These checks use candidate protocol/RPC32;
the O:/winnt S6 protocol/RPC31 package is not replaced. Internal service pipe
test seams, remaining authenticated-client extraction, service split and the
full production gate remain open; this checkpoint is not S7 closure.

## Baseline and evidence boundary

Delivered baseline is S6 production P 21b576a9e, protocol/RPC31, eight products
at O:/winnt. S7 admission is cfcbb7138. This is an implementation ledger, not
a closure or publication claim. Source and raw reports live under
build/M0-T424/S7/r001; keep the validated x86/MT CCPU40 dependency cache.

The initial lexical audit records 134 function definitions in base_service.c,
14 interface inputs, snapshot call sites and hashes of the pinned local
OpenNT base/win32/server/srvvdm.c, srvinit.c and base/win32/client/vdm.c.
No service function body is byte-identical after newline normalization to a
function in those three inputs. This is not proof of independent authorship:
derived ordering, initialization and adapted blocks still require manual
provenance review. The existing path/banner is not original-source evidence.
Original BaseSrvVDMInit and BaseSrvCleanupVDMResources remain mirror calls;
the original srvvdm.c remains the DOS/WOW record/execution owner.

## Initial ownership decisions

| Mechanism/current location | Origin and consumers | Decision/target | Independent boundary retained |
| --- | --- | --- | --- |
| interface console_io/mouse/video, frontend/vdm protocol, version, IDL/ACF | Project copied wire declarations; launcher/service/workers/frontend/monitor | common/protocol; generated RPC stays build-only | No pointers, registry or policy implementation in declarations |
| run16/native_request_io.c and worker-base/console_client.c transfer primitive | Project byte-transfer mechanics with completed-first versus strict peer-first ordering | One common bounded pipe-transfer provider with explicit ordering; remove obsolete control messages/callers | Preserve cancellation drain, exact/partial lengths, disconnect and caller-owned resource lifetimes |
| run16/native_launch_packet.c and interface/native_launch.h | Project pack/unpack codec, launcher/service/native worker | common/codec/native_launch; one selected provider | Target CreateProcess/materialization remains executable-owned |
| interface/worker_console_client.h and worker-base/console_client.c | Project worker-only ordered frame/input client, both workers | Header joins implementation in worker-base | Strict peer-first contract differs from control transfer; do not collapse them |
| interface/native_request_client.h | Launcher-local submit/finish/resume client | run16-owned declaration with existing implementation | No Console/worker ownership; real result remains broker receipt |
| interface/frontend_bootstrap.h | Launcher-local broker-issued references and release | run16-owned frontend connection declaration | Handle release is not frontend retirement or direct launcher/frontend IPC |
| worker-base/connection.c | Project worker initialization/watch/failed cleanup; NTVDM/NTVWM | Keep shared worker-base production owner | Common must not reverse-depend on worker-base/EXEs |
| base_rpc_client.c and related client declarations | Shared authenticated RPC adaptation plus retained BaseClient-shaped seams | Audit each function before choosing common client versus source-shaped seam | Original BaseClient command semantics do not become generic common policy |
| repeated Console list allocation/growth | Project mechanics in NTCON, native worker, run16 and RPC root reporter | Common snapshot primitive after comparing full growth/error contracts | Anchor selection, root authentication, quiescence and parent resume remain owner-local |
| base_service.c registration/connection/frontend/native/management additions | Project finite bindings around original mirror owners | Provenance ledger then bounded NTSRV-private modules | Single registry/lock authority; no common policy or second registry |
| original mirror execution/completion/block-resume/cleanup | OpenNT/MVDM original owners, retained by original callers | Keep original-relative paths and minimize registered hooks | Never move original algorithm into common/worker-base and reverse-call it |

The worker Console client has explicit borrowed pipe/peer/cancel resources and
one owned event; caller locks cover full exchanges. Neutral control transport
borrows all resources and drains pending cancellation. Codec owns allocated
packed bytes; unpack yields borrowed views. These contracts do not merge with
frontend ownership, task receipt authority or native target materialization.

## Checklist

- [x] Initial source/hash/function/call inventory and concrete owner map.
- [x] Verify the two auxiliary control pipes still participate in production.
- [x] Replace NTSRV-NTVWM command/launch/final-I/O pipe messages with RPC; copied-command and actual startup/receipt checks below.
- [x] Replace NTSRV-NTCON bootstrap acknowledgement pipe with RPC; actual startup/authentication fixtures below.
- [x] Source/dependency sweep proves only service RPC and worker-frontend I/O message families remain; runtime regressions still required separately.
- [x] Share bounded pipe mechanics with explicit completion/death ordering; retain owner-specific protocol and resource contracts.
- [ ] S8 transferred: finish complete manual service block provenance and per-mirror accounting before service moves.
- [x] Move declarations and surviving neutral codec/client mechanisms, link finite common archives.
- [x] Extract reviewed native/frontend/worker/management client mechanisms without moving original DOS/WOW callback/state policy; focused and actual RPC evidence below.
- [x] Consolidate compatible Console snapshot mechanics and test growth/failure.
- [ ] S8 transferred: split NTSRV-private service state/modules without public private-state exposure.
- [ ] Remove replaced providers/wrappers/includes and verify one selected implementation.
- [ ] Build/test full production gate, recoverable publication, smoke and reviewed delivery.

No S7 behavior or runtime pass is established yet. O:/winnt remains S6. GUI
routing, frontend naming and monitor UNBOUND remain their subsequent owners.

## Communication audit and migration order

Source inspection confirms the owner's distinction is real, not merely two
names for one existing RPC path:

| Edge | Current production message path | S7 disposition |
| --- | --- | --- |
| NTSRV-native worker | SubmitNativeRequest creates ntsrv-native pipe; GetNextNativeCommand exports it; execution.c reads header/payload and writes launch reply/final completion | RPC copied command delivery and authenticated launch/final-I/O reports on the existing direct record |
| NTSRV-frontend | StartFrontend creates ntvdm-frontend-start pipe; frontend session_entry writes frontend_bootstrap_reply | RPC startup outcome authenticated against the exact pre-Resume admission |
| run16/monitor/workers/frontend-NTSRV | Existing ncalrpc service methods | Retain and consolidate selected common client/provider ownership |
| Both workers-frontend | console_channel.c creates the direct I/O pipe; worker-base/console_client.c exchanges frames/input/title/handoff | Retain one shared I/O protocol and worker client |

The native pipe carries three different facts: launch acceptance/failure,
actual target completion, and final I/O release/resource state. Migration
must retain their ordering without treating a final-I/O error as a target
exit code. Resume requests currently use request zero and a zero-byte pipe
header; their reply must remain distinct from direct task completion. Nested
direct execution cannot be serialized behind an outer target wait.

The frontend pipe currently authenticates the broker PID in NTCON while
RegisterFrontendLease authenticates the exact admitted child and inherited
capabilities in NTSRV. Its replacement must preserve this grant, report
failure even before root registration, and never accept an arbitrary process
or event nominated by a client. Startup RPC success is not Console restored.

Implement communication removal before final common transport ownership;
then split only the surviving project service mechanisms. Actual wire changes
require paired application/RPC revision and regenerated MIDL. This audit does
not establish implementation, build, publication or S7 completion.

## First implementation checkpoint (not a delivered P)

Frontend startup now reports through FrontendStartupResult RPC, authenticated
against the exact pre-Resume process and capability. The result event is owned
by the active NTSRV StartFrontend call, never exported; borrowed references are
cleared under the service lock before closing it. Successful reporting requires
the registered Console lease; a genuine failure can be reported before root
registration. Repeated, wrong-process, wrong-generation and wrong-capability
reports are rejected. The existing Console restoration RPC remains separate.
The broker waits for result/process/creator/deadline events, not polling.

Application protocol and IDL major advance together to 32, with regenerated
MIDL and explicit client/server interface references. The six executable x86
links and BaseSrv reservation/lifecycle fixture pass before common extraction;
new checks cover registration-before-success, authenticated pre-registration
failure, duplicate and stale reports. Raw logs: build-startup-rpc-final.log,
build-startup-rpc-tests.log and reservation-startup-rpc.log under the S7 root.

The new common pipe primitive replaces worker-client and transitional control
transfer bodies; frontend I/O uses its begin/finish/drain operations while
keeping input-ready notification policy local. Its full build/test verdict is
pending. Native control RPC migration and common/service reorganization remain
open. No candidate publication, full regression or S7 closure is claimed.

The subsequent common checkpoint links all six x86 EXEs and the selected
fixtures. common/control transfer passes 117 checks, zero failures, including
partial completion, both wait priorities, pre-existing peer death, cancellation
drain and invalid capacity/policy. Private-desktop presentation passes 415/0;
real input-return variant passes 677/0, both observer exit zero. These are
transport/presentation fixture proofs, not ordinary-product acceptance.
Logs are build-common-tests-final.log, common-transfer-test.log and
presentation-common-{fixture,input-fixture,observer,input-observer}.txt.

An older Console-client fixture lacked its already-required broker shutdown
provider. It now mocks a wait-only shutdown event and explicitly models the
broker instruction after root loss, instead of pretending the worker decides
retirement from root death. Its link passes; its full runtime variants remain
unverified. The generated graph initially placed the archive before an implicit
output separator; fixed selection now inserts it only among link inputs. Node24
automatic discovery was corrected by regenerating with the existing pinned
Node22 path. Retain failed logs as diagnostics, not passes.

## Common ownership migration checkpoint (not a delivered P)

Thirteen declaration/codec files have moved to their reviewed owners. Eight
copied protocol/IDL declarations now live in common/protocol; native packet
pack/unpack has one common-codec.lib provider. Bootstrap/request client API
declarations live with run16, and the local worker-client instance declaration
lives with worker-base. Process creation declarations were removed from the
common codec header and remain run16-owned. The original packet algorithm is
unchanged. The remaining interface/native_request_protocol.h is transitional
and still must be deleted by native-control RPC migration.

The reviewed build-only migration script and common-move-manifest.json retain
thirteen move hashes and fifty-two changed-path hashes. An initial attempt
rewrote some project include paths before detecting an unchanged ANSI mirror
that cannot round-trip through UTF-8. The guard stopped before writing that
mirror; a subsequent unchanged-text skip avoids transcoding it. The manifest
is not claimed to cover the pre-attempt state of those earlier include edits;
the retained pre-move-current.diff and Git baseline supply that comparison.
Git confirms no changes to mvdm, opennt-host or the shared KVM library.

Six x86 executable links and MIDL32 regeneration pass in
build-common-layout.log. The transfer fixture passes 117/0 and service
reservation/lifecycle fixture exits zero (common-layout-transfer.txt and
common-layout-reservation.txt). Link ownership now checks common provider
selection, rejects archive-hidden duplication and common reverse dependencies,
and retains frontend/target-creation/worker ownership negatives. The old
assertion rejected the existing run16_native_request_submit function by name;
HEAD confirms it already calls NTSRV. Its replacement rejects old RPC names
and direct launcher pipe operations instead. This is not a relaxation of the
frontend execution boundary.

Repeated Console PID snapshot mechanics are now consolidated in
common/console/members. Caller-specific read/capacity limits and all authority
decisions remain local: root reporting 32/4096/two reads, frontend anchor/join
16/4096/growth, native quiescence and parent resume 16/65536/growth. A zero-count
API failure with no error now deterministically fails rather than relying on a
stale LastError or accidentally processing a null snapshot. Allocation/growth
failure never exposes partial results; no timer, task record, ancestry, member
observer or retirement policy is added. New fault-injection tests exercise the
actual production implementation. The updated six x86 EXEs and selected
fixtures link successfully (build-common-snapshot.log). Snapshot fault
injection passes 66/0, ownership positives/negative mutations pass, and the
BaseSrv lifecycle fixture exits zero (common-snapshot-test.txt,
common-ownership-test.txt and common-snapshot-reservation.txt). These are
focused checks, not full ordinary-product acceptance.

Real private-desktop frame and input-return fixtures pass 415/0 and 677/0 in
snapshot-presentation-r002-{frame,input}.txt; both observer exits are zero.
The initial restricted-sandbox run failed at the named-pipe client open,
before any capture/snapshot operation, producing 60 failures. The same built
fixture passes with normal process permissions; no assertion or product ACL
was changed. Retain the initial snapshot-presentation-frame.txt and observer
log as a failed environment run, not a pass. Observers' own exit zero alone
does not prove their child fixture passed: the runner checks child assertions.

O:/winnt remains the accepted S6 protocol31 set. Native control RPC removal,
authenticated-client extraction, service-private split and the full production
gate remain open; no S7 publication, commit or closure is claimed.

The native request lifecycle fixture also passes after codec migration:
586 checks, zero failures, twelve completed cases, sixteen cancelled header
reads, handed-off target survival, broker-completion failure and resume
barriers, with remaining-handle delta zero. Its completion and quiescence
providers are fixture substitutions, not real-service acceptance. See
build-common-lifetime.log, common-native-lifetime.txt and its observer report.
Duplicate common-codec.lib spelling on that fixture link was removed; the
common selection pass supplies it once. Final regeneration/relink and ownership
checks pass (build-common-final.log and common-ownership-final.txt).
Governance and diff checks pass;
full Console17/Window17, WOW comparison and publication remain unexecuted.

The final native I/O result is now copied into the existing NTSRV direct
record by CompleteWorkerChannel RPC, before signaling the completion receipt.
FinishNativeRequest consumes that record without a second pipe read. The
record no longer owns a final-I/O pipe or worker handle; its startup-delivered
guard remains explicit. Real target completion and final-I/O errors remain
distinct, completion RPC failure still faults the worker, and result consumption
is exactly once. Startup command/reply piping remains an open removal row.
Six x86 EXEs and MIDL32 relink pass in build-native-completion-rpc-r002.log.
The earlier build failed because the added fixture lacked the protocol
header; the header was added, not the assertion weakened. Actual service-record
fixture exits zero (native-completion-reservation.txt), including invalid flags,
real exit 37 with final-I/O ERROR_WRITE_FAULT, and duplicate consumption rejection.
Private-desktop native lifecycle passes 606/0, zero handle delta, including a
real target exit 73 reported via the completion callback and no final pipe
response (common-native-lifetime-completion-rpc.txt and its observer report).
Completion callback and quiescence remain fixture substitutes in that test;
full real-service/product acceptance remains open.

The owner's final clarification places both protocol families and applicable
shared client/transport mechanisms in common: NTSRV control on RPC and direct
NTCON-worker I/O on named pipes. The briefly discussed I/O-to-RPC migration
is withdrawn. No I/O-to-RPC production change was made; worker-base may depend
on common, not the reverse. No S7 publication or delivery is claimed.

The identical local RPC binding sequence in project-added base_rpc_client.c
and ntmon/main.c is extracted to common/rpc/local_binding.c, with explicit
output state and no globals. Both previously composed ncalrpc, materialized
a binding and configured WINNT packet privacy with no authorization service.
The common provider releases partial strings/bindings on every failure and
returns the exact RPC status. BaseClient retains its existing ERROR_GEN_FAILURE
mapping at this boundary; NTMON retains the precise status/LastError behavior.
Endpoint/logon selection, service peer checks, version handshake, process
attachments and broker-loss policy remain unchanged and outside this provider.
This is project mechanism reuse, not an original OpenNT algorithm extraction.
Common has no reverse private/worker dependency. One common-rpc.lib provider
is selected by explicit link input for run16, NTVDM, NTVWM, NTCON and NTMON;
there are no copies embedded in private archives.

Build-common-rpc-binding.log records six x86 executable links and the new
fault-injection fixture. Common-rpc-binding-test.txt passes 42/0 with zero
live resources; the fixture includes the actual provider with mocked RPC APIs
to force all three partial-failure stages and verify auth parameters/ownership.
Common-rpc-ownership-r002.txt passes provider/consumer checks and reverse-
dependency negative controls, including common-rpc.lib. Common-rpc-reservation.txt
exits zero. These focused checks do not replace a real service version/auth
integration run or the ordinary Console/Window/WOW publication gates.

The superseded native_request_completion wire structure and its private
broker_native_completion_status validator are now deleted. Neither worker nor
service references that final pipe protocol. The startup header/reply remains
transitional and is still an open migration row, not a third accepted final
protocol. The client-only fixture now models final publication as a broker
record/event, not a delayed pipe response. Explicit permission holds that
publication until actual target exit has been verified: no Sleep is used to
pretend a final-paint barrier. Success, ERROR_WRITE_FAULT, ERROR_BROKEN_PIPE
after real target exit and worker-failure results remain asserted. Invalid
completion flags are covered by the actual service-record fixture rather
than the deleted message's obsolete version/flags validator.

Build-final-io-protocol-cleanup.log records successful x86/MIDL consumer relinks.
The private-desktop client fixture exits zero (final-io-client-observer.txt);
its runner verifies the child exit, not just observer success. This is a
broker substitute fixture, not real RPC/product acceptance. Service record
fixture exits zero (final-io-cleanup-reservation.txt). Link ownership and the
retired-final-pipe symbol absence checks pass (final-io-cleanup-ownership.txt).
No original mirror/shared-library change, package publication or S7 delivery.

### Native startup result RPC candidate checkpoint

NativeStartupResult now carries authenticated caller generation/request,
status and typed process/event attachments through the service RPC. NTSRV
checks the selected worker, current delivery, bound target PID and canonical
receipt object before publishing startup. SubmitNativeRequest owns a separate
startup event (not its overlapped transfer event), moves typed output resources
under the service lock, and clears that borrowed scope before disposing it.
Errors on the transitional command read also report explicit startup failure.
GetNext failure clears request/generation and attachments. No worker-I/O pipe
change or new process is involved.

The command header/payload still uses the transitional native control pipe;
the old reply DTO remains as local candidate/test scaffolding. Its production
reply transport and sender-namespace target/receipt export are removed. This
is not yet the final two-protocol closure: copied command delivery, deletion
of the scaffolding, actual cross-process startup authentication/rollback tests,
service source separation and the complete product gate remain open.

Build-native-startup-rpc.log and r002-r004 logs record regenerated MIDL32,
six x86 executable links and focused fixtures. Native-startup-next-command.txt
and native-startup-reservation.txt pass. The private-desktop native fixture's
startup RPC is substituted, while target creation/wait/stream denial are real:
common-native-lifetime-startup-rpc-r004.txt reports 703 checks, zero failures,
12 completions, 16 cancellations and zero remaining-handle delta; its observer
records actual child exit zero. These are not real RPC/product acceptance.

The first attempts are retained as failures. A single latest-generation mock
wrongly rejected concurrent cancelled deliveries; the fixture now verifies
each admitted generation/request and restricts stale deliveries to cancellation.
A separate one-handle difference localized to first CreatePipe use: the API
added three handles, and closing its two returned handles left one OS facility.
Warm that API before the unchanged zero-leak baseline, as for CreateProcess;
do not relax the request cleanup assertion. No S7 publication/P is claimed.

The subsequent startup-failure sweep found the no-serving-thread case still
completed its record without publishing startup. NTVWM now reports allocation/
thread-creation failure via the same NativeStartupResult RPC before completion;
on report failure it uses the existing broker-fault contract, not a fabricated
success. The execution path no longer uses native_request_reply; that DTO
remains only in transitional tests pending command-pipe removal.

Build-native-startup-rpc-r005/r006.log and build-native-startup-auth.log pass.
The actual product path (not a mock) starts native CMD, captures its output
marker and returns exit 37 through the broker: native-startup-product-r001/r002.txt
and native-startup-product-tracked.txt. The reproducible entrypoint is
tests/observation/verify-native-rpc-startup.ps1 with Observer=the selected cache's
observer.exe, PackageRoot=build/M0-T424/S2/r001 and a fresh ReportPath under
build/M0-T424/S7/r001. It asserts child result/output, not observer exit alone.
The selected cache supplies the six current candidate EXEs; this native-only
probe neither needs guest images nor proves coherent WOW package publication.

The real monitor-rpc-test fixture adds NativeStartupResult launcher-role and
wrong-generation rejection using typed process/event attachments. Its isolated
observer native-startup-auth.txt records child exit zero, also retaining its
version mismatch, restricted capability, root rundown and production inner
launcher negative checks. The r006 substituted-RPC lifetime fixture reports
705/0 and zero handle delta (checks vary with its existing drain observations).
No forced allocator/CreateThread-failure run is claimed; the new main-loop
failure ordering is source-reviewed and still needs focused injection coverage.
Copied command RPC delivery, service separation and full S7 gates remain open.

### Common I/O protocol client ownership

The final owner clarification makes common the carrier of both protocols and
their shared client mechanisms. Provenance review of all functions in the
existing project-added worker-base/console_client.c found only NTCON protocol
encoding, sequence/generation/reply validation, frame chunks, title publication,
input decoding and exact pipe exchange. It has no original mirror algorithm,
backend binding, frontend rendering, task registry or retirement policy.
Move that implementation/header to common/console/client.c/h with unchanged
function bodies and caller-owned lock/borrowed-handle contracts. Both workers
and the same fixtures now include the common header. Worker-base retains only
its worker-specific connection adaptation; backend operations remain local.

Common-console.lib selects members.obj and console_client.obj exactly once.
Worker-base.lib no longer embeds the client. The Ninja manifest, affected
fixture graphs and link-ownership verifier reflect the new owner. Actual
archive inspection (common-io-archive-members.txt) confirms worker-base has
only connection.obj and its four connection symbols, not old client members.
The graph verifier and its reverse-dependency/leakage negative controls pass
(common-io-link-ownership.txt). No original mirror or shared KVM source diff.

Build-common-io-client.log passes six x86 executable/fixture links. The real
native probe common-io-native-product.txt returns 37 and captures the marker.
Snapshot-presentation-common-io-client-frame/input.txt pass 415/0 and 677/0
using the actual common provider, with protocol peers/backend operations mocked.
The isolated Console API fixture normal-r002 asserts its source-defined close
callback output and exit 73; broken-pipe-r001 exits zero and asserts transport,
native errors and idle frontend loss. These observer reports begin
common-io-console- and remain under the S7 build root.

Initial direct Console-fixture launch failed because the shell supplied no
usable Console HWND. The first isolated runner then incorrectly expected zero
instead of the fixture's deliberate CntrlHandler ExitProcess(73); retain both
attempts as failures, not passes. The corrected runner requires the callback
marker as well as 73. The broken-pipe variant still requires zero. Neither is
real DOS guest-close proof. No S7 delivery or publication is claimed.

### Copied native command RPC checkpoint

GetNextNativeCommand now returns a bounded copied payload (maximum 1 MiB),
its length and the existing typed sender/execution/frontend attachments.
SubmitNativeRequest copies validated launch data into the existing service
queue under its lock; it no longer creates/connects/writes a native control
pipe. Startup acknowledgement and final result retain their authenticated RPC
and canonical direct-record contracts. The serving worker owns the received
heap payload and attachments after successful thread handoff; failed thread
creation leaves them with the caller for exactly-once disposal. Zero-length
delivery retains the existing I/O-resume operation, not a task or new scheduler.

The service checks output capacity before consuming the pending command or
creating its in-flight record. Copy and ownership transfer occur under the
same service lock. Sender generation is copied from that authenticated caller,
not reconstructed later by scanning completed delivery state. Shutdown still
precedes command delivery in the existing condition-variable wait loop.
No worker I/O pipe, original mirror, guest or shared KVM code is changed.

Build-copied-command.log records regenerated MIDL and successful six x86 EXE
links plus reservation/get-next fixtures. Ntvwm-next-command-test passes copied
bytes, recipient handle ownership, failure disposal and zero-length resume.
The tracked verify-native-rpc-startup.ps1 entrypoint against the current cache
produces copied-command-native-product.txt: actual CMD output contains
NATIVE-RPC-STARTED and the direct result is exit 37. Build-copied-command-auth.log
rebuilds the actual RPC negative fixture. Copied-command-basesrv-service-
reservation-test.txt and copied-command-monitor-rpc-test.txt record isolated
child exit zero for retained reservation/retirement and authenticated RPC
role/generation/capability/version negatives. This is not a DOS/Window/WOW
package gate or publication.

The old QueueNativeChannel/TakeWorkerChannel internal seam is temporarily
retained only for existing service fixtures. The execution lifetime fixture
still models command headers/blocked pipe reads and has not yet been migrated
or rerun against this checkpoint; its earlier 705/0 result does not certify
the new copied-payload implementation. Delete these obsolete seams/helpers
after replacement assertions cover the retained ownership, cancellation,
startup failure, completion failure, real target survival and final-I/O rules.
S7 remains open, uncommitted and unpublished; O:/winnt remains accepted S6.

### Copied-command lifetime fixture migration

The execution fixture now supplies recipient-owned copied bytes and typed
attachments, with per-generation startup observation events rather than a
command/reply pipe or numeric handle DTO. It retains 12 malformed commands,
oversized length rejection before body access, allocator-free invalid argument
checks, maximum packet bounds, real denied stream materialization and preflight
failure before target execution, real exit 73, held target survival with no
fabricated completion, resume begin/release/end ordering and broker failure
before reentry. Sixteen blocked header reads no longer exist in production;
their replacement starts 16 real serving threads blocked at I/O admission,
then verifies cancellation, one cleanup report per request and full drain.
The existing mock RPC/backend scope remains explicit, not real broker proof.

Build-copied-lifetime.log failed on the fixture's unlinked disposer and a
previous console-client-test.exe process locking its file. The failed run is
retained. The fixture disposes its own untransferred inputs locally; only the
exact confirmed cache test PID 18340 was stopped. Build-copied-lifetime-r002/
r003.log then link successfully. Common-native-lifetime-copied-command-r001/
r002.txt report 779/0 and 850/0 respectively, with zero remaining handle delta.
The latter adds actual target completion with final write/broken-pipe errors
and completion-RPC failure, preserving the real exit code and fatal reentry
contract. Physical desktop behavior is not claimed.

Removed production files: run16-exe/native_request_io.c,
interface/native_request_protocol.h and ntsrv-exe/transport/native_control.c/h.
Their graph/archive inputs are removed. Generic transfer fixtures call the
common mechanism with explicit priority; old native/bootstrap pipe models
remain isolated in legacy tests pending RPC-specific replacement. The legacy
reply validator exists only in that test, not a production archive. No S7
publication/closure is claimed; source separation, remaining fixture migration,
authenticated-client ownership and complete regression gates remain open.

Build-control-wrapper-removal.log exposed that the lifetime fixture previously
received codec linkage incidentally through its deleted transfer wrapper.
The graph now declares common-codec.lib explicitly for this fixture, without
unneeded client/service libraries. Build-control-wrapper-removal-r002.log
passes all six x86 EXE and selected fixture links. The direct common transfer
unit passes 117/0; the frontend link-ownership verifier passes its production
ownership and reverse/leakage negative controls. Control-wrapper-removal-
native.txt retains actual output/direct exit 37; common-native-lifetime-
wrapper-removal-r001.txt repeats 850/0 with zero handle delta after re-link.
Copied-lifetime-governance.txt passes the structural documentation gate.
These focused checks do not replace the still-open complete S7 package gate.

### RPC-specific launcher/bootstrap fixtures

Frontend-request-client-test now substitutes copied RPC submission/results,
not a native control pipe. It checks stripping of launcher authority slots,
cleared failure outputs and no target creation on RPC errors, actual target
exit 37, the final-I/O permission barrier after target exit, retained write/
broken-pipe errors, zero-length resume and cleared broker-failure result. Wire
version/contradictory/EOF/short-reply cases no longer refer to a product wire;
actual RPC role/version/capability validation is retained in monitor-rpc-test,
while partial/EOF/cancel behavior remains in common transfer assertions.
Neither fixture mock is represented as actual broker execution proof.

Broker-frontend-bootstrap-test's adversarial trusted sibling uses the actual
--session argument shape and FrontendStartupResult RPC. It tests failure
before registration, rejection of unregistered success and forged capability,
bounded ten-second no-report timeout and no launcher handle delta. Original
application/protocol mismatch assertions remain in monitor-rpc-test's actual
Connect calls; that fixture adds startup-report launcher-role/wrong-generation
rejection. Obsolete native/bootstrap pipe reply DTOs and version constants
have no callers and are removed from common/protocol/frontend_protocol.h.

Build-rpc-fixture-migration/r002/r003.log link six x86 EXEs and the selected
fixtures. The tracked entrypoint tests/observation/verify-s7-rpc-fixtures.ps1
takes Observer=cache observer.exe, PackageRoot=build/M0-T424/S2/r001,
ReportPrefix=fresh path under build/M0-T424/S7/r001, and selected Cases. It
requires actual child exit zero and assertion output on a private desktop.
Rpc-fixture-migration-r001 client/bootstrap pass; its startup-rejections fails
because the test peer returned artificial exit 83 instead of its actual RPC
failure. NTSRV correctly preserves failed child startup status. Peer now
returns that actual failure, as production does; unexpected acceptance reports
ERROR_INVALID_DATA and must fail the caller's exact ACCESS_DENIED assertion.
Rpc-fixture-migration-r002 startup-rejections, monitor-rpc and startup-timeout
pass. Rpc-fixture-migration-r003 native-worker-failure, native-completed-worker-
loss, workerless-grace and workerless-cancel pass. Each report retains actual
fixture child exit zero and assertion output. No production semantic workaround
or authentication relaxation was made. Full S7 regressions, internal service
channel seam removal and source/common client ownership separation remain
open; nothing is published or committed.

### Native registry fixture ownership migration

The --native-worker fixture authenticated its launcher against initial_root,
then attempted to bind that launcher to a different frontend and expected the
worker to rebind after root rundown. Those expectations contradict the existing
immutable launcher association and broker-owned root-loss shutdown. Production
authorization was not relaxed to accommodate this obsolete fixture scenario.

The fixture now uses one admitted root/capability for reservation, copied
delivery and presentation; its real suspended native target is separate.
Generation/capability/capacity, target snapshot, receipt, I/O failure and
no-fabricated-descendant assertions remain. Root disconnect checks the worker's
actual shutdown event, broker retirement, process survival until explicit close,
stale authority rejection, old-worker rebind rejection and cancelled GetNext
with empty outputs. Explicit Console-close acknowledgement and process rundown
remain checked. The obsolete Job-ownership comment is corrected. No production
source changed for this fixture migration.

Build-native-registry-r001.log failed on a locked fixture EXE. Three suspended
children from the prior failed fixture had their exact cache path/command lines
checked before disposal (3208, 26148, 8092). R002 linked. Migration-r001 reached
line 734, where the test expected ACCESS_DENIED but the worker's retained lost
native_root explicitly yields ERROR_PIPE_NOT_CONNECTED. The exact assertion
was corrected, not broadened; build-native-registry-r003.log links six x86 EXEs
and fixtures. A comma-separated -File Cases argument failed PowerShell validation
before execution; the actual -Command array invocation below passes both cases.

Reproduce with tests/observation/verify-s7-rpc-fixtures.ps1, selected cache
observer.exe, PackageRoot=build/M0-T424/S2/r001,
ReportPrefix=build/M0-T424/S7/r001/native-registry-migration-r002 and
Cases=@('native-registry','native-command'). Both actual children exit zero with
PASS assertions on the isolated desktop. Raw reports are
native-registry-migration-r002-native-registry.txt and
native-registry-migration-r002-native-command.txt plus console captures.
This resolves the prior fixture-policy mismatch only. Common client/service
organization and full S7 verification/publication remain open. O:/winnt remains
the accepted S6 package, not this uncommitted candidate.

### Explicit frontend-control common client checkpoint

Ten project-added RPC exchanges are extracted from
ntsrv-exe/opennt/source/base_rpc_client.c: AcquireFrontendRoot, FrontendUsage,
RetireWorkerlessFrontend, FrontendStateChanged, StartFrontend,
ReturnFrontendConsole, FrontendConsoleRestored, FrontendStartupResult,
WaitFrontendConsoleRestored and RetireFrontend. These are project transport
adapters, not original OpenNT/MVDM algorithms; service authentication and
retirement decisions remain in NTSRV. Original BaseClient callback/receipt
state remains local. Their 145 predecessor function lines now reside in
common/rpc/frontend_control.c, with explicit borrowed state. Ten six-line
facades (60 lines) retain caller compatibility but no duplicate RPC body.
Connection declarations move into common/rpc/connection.h for both clients.

Acquire/start failure closes partial attachments and clears outputs; success
transfers output ownership. Notification requires a real returned event.
Usage/retirement scalar outputs remain untouched on failure; status and RPC
exceptions are forwarded unchanged. Input capabilities are borrowed. Null
connection views fail INVALID_STATE. The restoration wait remains the existing
synchronous RPC; common chooses no deadline and restores no Console. No wire
change, third protocol, registry, timer or frontend ownership is introduced.

The build selects frontend_control.obj through common-rpc.lib, with generated
service.h dependencies and source hashes in its composition. Ownership checks
require three RPC providers and reject private/reverse/test-source leakage.
Actual run16/ntcon/ntvdm/ntvwm maps attribute the new implementation to
common-rpc:frontend_control.obj; NTMON selects only its needed binding.

Build-frontend-control-r001/r002.log pass six x86 products and selected fixtures.
Tests/app/common_frontend_control_test.c checks all ten exact generated RPC
calls, output ownership, returned errors, raised exceptions, partial release,
invalid state/outputs, unchanged failure scalars, missing event and borrowed
handle survival. Common-frontend-control-r001/r002.txt pass 149/0 with zero
handle delta; common-native-command-frontend-r001.txt retains 108/0, zero delta.
Substitute-RPC tests are not actual authentication proof.

Actual verify-s7-rpc-fixtures.ps1, selected cache observer.exe,
PackageRoot=build/M0-T424/S2/r001 and ReportPrefix under
build/M0-T424/S7/r001/frontend-control-rpc-r001 pass nine cases: client,
bootstrap, startup-rejections, startup-timeout, native-worker-failure,
native-completed-worker-loss, workerless-grace, workerless-cancel, monitor-rpc.
All require actual exit zero and PASS assertions. Separately,
verify-native-rpc-startup.ps1 produces frontend-control-native-product-r001.txt:
production CMD outputs NATIVE-RPC-STARTED and reports broker exit 37.
Frontend-control-ownership-r001.txt passes dependency/leakage controls.
Remaining client/NTSRV-private organization, provenance/mirror accounting and
full eight-file regression/publication remain open. O:/winnt stays accepted S6;
no S7 commit or closure is claimed.

### Management projection block provenance and split boundary

This manual pass reads the current base_service.c management block (lines
900-1182), service_win32record_depth, their callers and struct ownership, then
compares the reached original source rather than classifying by filenames.
Current base_service.c SHA256 is
cd8830306db4b868514c0b8a7087183be6c8dc87bffc6c2c85363d4d42c78cff.
Pinned OpenNT base/win32/server/srvvdm.c SHA256 remains
c1e2177c6c00679d85cfa475f620841f6736b0e56d8dbf790b71afe33e1ed80b;
the selected project mirror is
342d10e9610778df828e33ff4095e7fdb64a94e3bc899a2dbf43f02338814fbb.
These are source identity observations, not a completed all-mirror diff sweep.

| Function/block | Provenance and retained dependency | Target and reason |
| --- | --- | --- |
| service_copy_management_text | Project monitor label conversion using MultiByteToWideChar; no original execution mutation | NTSRV-private management.c, not a common codec policy |
| service_copy_management_image | Project label projection from original VDMINFO AppName/AppLen | Same private module; original VDMINFO owner stays unchanged |
| service_same_management_wait | Source-shaped derivation of original BaseSrvGetVDMExitCode low-bit handle tag comparison, pinned srvvdm.c line 1832 | Keep attributed private display comparator; do not move original exit query or receipt handling |
| service_clear_management_labels | Project sidecar allocation cleanup, not original DOSRecord destruction | Private management module; worker rundown remains the caller |
| service_find_management_label | Project sidecar lookup keyed by original record identity and tagged wait identity | Private module under existing service lock; not another task registry |
| service_prune_management_labels | Project stale display-label deletion after reading original DOSRecord chain | Private module; cannot delete or complete original records |
| service_capture_management_label | Project copied label storage; allocation failure deliberately cannot fail task delivery | Private module, preserve non-authoritative failure contract |
| service_capture_management_label_from_info | Project capture before original command info destruction | Private module; original GetNext body frees VDMINFO, pinned srvvdm.c line 441 |
| service_capture_initial_management_labels | Project display traversal of original DOSHead/records under BaseSrvDOSCriticalSection | Private module, invoked at existing worker claim boundary |
| service_find_management_watch_for_console | Project lookup of the single service-owned worker census | Private module; no second registry or PID discovery |
| service_capture_checked_management_label | Project display capture at existing CheckVDM seam using original parent-wait identity | Private module with existing caller/lock order, not task admission policy |
| service_copy_management_record | Project read-only DOS/WOW projection using original record state and selected original locks | Private module; original record dispatch/completion remain srvvdm.c |
| service_sort_management_records | Project deterministic snapshot presentation order | Private module; sorting must not reorder ownership lists |
| service_win32record_depth | Project count of unfinished Direct Win32Records | Private module; no Console members/Observed task inference |
| service_copy_win32record | Project projection of actual service-owned Direct records | Private module; no process creation/wait/receipt authority |
| OpenNtBaseServiceSnapshot | Project copied authenticated management DTO production | Private management module with unchanged public signature, service lock and capacity contract |

The selected original BaseSrvGetVDMExitCode additionally removes/completes
records; that algorithm is explicitly not part of the display extraction.
Original GetNextVDMCommand owns VDMINFO disposal and original DOS/WOW locks.
The service owns label allocations, registry state and lock. Management borrows
that state only during the caller's lock scope; Snapshot acquires the same lock
it currently acquires, with DOS/WOW lock nesting unchanged. Only clear-label,
initial-capture and checked-capture need a bounded service-private cross-module
declaration; lookup/conversion/pruning/sorting remain module-local. No private
state becomes a common/public protocol declaration. TerminateWorker is not
grouped into the display module: cooperative shutdown and close acknowledgement
remain lifecycle authority, despite proximity to Snapshot in the current file.

Disposition is reviewed but not yet physically moved. The complete service
block ledger and per-mirror accounting gate remain open before source splitting;
the initial 134-definition lexical scan alone does not satisfy them. No source,
wire, runtime package or test assertion changes in this manual checkpoint.

### Explicit worker-control common client checkpoint

Reviewed project additions: WorkerFrontendCapability, AcquireConsoleContext,
BindConsoleContext, RegisterNativeBackend, WorkerShutdownEvent,
WorkerStateChanged and take_frontend (Take/WaitFrontend). Their 111 predecessor
function lines are replaced by 42 facade lines; one implementation is in
common/rpc/worker_control.c. Both NTVDM/NTVWM reach it through the existing
source-shaped BaseClient public facade. Original callback/receipt mapping,
execution, waits and failure decisions remain adapter/worker-owned. No mirror,
wire contract, scheduler, process, I/O-pipe or retirement policy change.

All connection/input resources are borrowed per call. Output attachments are
caller-owned on success and released/cleared on error or exception. Event APIs
retain successful-null rejection; capability APIs retain provider-status
semantics. Take versus service-blocking Wait remains the caller's choice.
This is shared NTSRV control RPC, not a change to direct named-pipe I/O.

Build-worker-control-r001.log passes six x86 EXEs and selected fixtures.
Common-worker-control-r001.txt passes 164 checks, zero failures and handle delta
zero: all seven exchanges/eight generated RPC methods, explicit view forwarding,
two view identities, Take/Wait selection, invalid arguments/state, returned
errors, raised exceptions, partial cleanup, null notification and borrowed
resource survival. Retained frontend/native client tests pass 149/0 and 108/0,
both zero handle delta. These mocked providers do not prove authentication.

Actual verify-s7-rpc-fixtures.ps1 with the selected cache observer and PackageRoot
build/M0-T424/S2/r001, ReportPrefix under build/M0-T424/S7/r001/worker-control-rpc-r001,
passes eleven cases: client, bootstrap, startup-rejections, startup-timeout,
native-worker-failure, native-completed-worker-loss, workerless-grace,
workerless-cancel, monitor-rpc, native-command, native-registry. All require
actual exit zero and PASS assertions. Worker-control-native-product-r001.txt
independently records real CMD output and broker exit 37. Link maps for both
workers attribute shutdown/TakeFrontend to common-rpc:worker_control.obj;
worker-control-ownership-r001.txt passes graph leakage/ownership checks.

The retained normalized pre-extraction candidate snapshot is
build/M0-T424/S7/r001/base-rpc-client-before-worker-control.c (not a build input),
SHA256 886ef6279f2f1f349a9ca6f21470f9337a70bf12ad5cb2c4c62add645d4d7cc2.
Tests/component-integration/verify-common-worker-control.mjs compares the seven
bodies after only the explicit-state/null-view transformation and checks each
facade has no duplicate RPC/cleanup body. Fourteen altered-body/duplicate-facade
negative controls are rejected; worker-control-source-equivalence-r001.json
records PASS. This bounded equivalence check supplements manual provenance;
it does not certify the complete service or original mirrors.
Remaining client/service organization and full S7 gate stay open. No S7 P or
publication: O:/winnt remains the accepted S6 eight-file package.

After final composition-description/graph regeneration, build-worker-control-
r002.log triggers the required product re-links and passes the retained VdmTib
storage check. Evidence is therefore repeated against those final artifacts:
worker-control-rpc-r002 covers all eleven cases and
worker-control-native-product-r002.txt covers actual CMD output/direct exit 37.
Worker-control-ownership-r002.txt and worker-control-governance-r001.txt pass.
The earlier reports remain checkpoints, not replacements for these final-input
checks. No full-package regression or publication conclusion follows from them.

## Management RPC client ownership checkpoint

Question: does NTMON still own duplicated project RPC exchange bodies despite
the admitted common control-protocol carrier? Inspection found TaskSnapshot in
refresh and TerminateWorker in terminate_worker. These are project-added
connectionless management exchanges, not original BaseSrv DOS/WOW execution or
record algorithms. Existing endpoint authentication/version validation remains
NTSRV-owned; binding lifetime, reconnect, selection, confirmation and UI remain
NTMON-owned. No original source or mirror algorithm is moved.

The two exchange bodies now live in common/rpc/management.c with its explicit
borrowed binding/process view. There is no context registration or new client
registry. Failed snapshots free partial MIDL allocations and clear output;
successful allocations transfer to the caller. RPC status/exception values and
the selected PID are conveyed unchanged. NTMON keeps its existing three
hotkeys, kind labels, display and queued-confirmation invalidation. Old direct
exchange bodies are removed from the product caller. The actual management
fixture calls the same provider for its valid requests while keeping generated
RPC version-mismatch and missing-worker negatives independent.

Verification uses the validated x86 /MT cache build/M0-T424/S2/r001; raw
checkpoints stay under build/M0-T424/S7/r001. Graph-management-r001/r002 and
build-management-r001/r002 logs record six successful product links and the
retained VdmTib storage assertion. First build revealed a duplicate archive
argument and test signedness warning; both were corrected, not ignored.
Final common-management-r002.txt reports 72 checks, zero failures and zero
outstanding allocations (first checkpoint was 60/0). Substituted providers
cover peer/version identity, return errors, raised exceptions, partial result
release, empty snapshots, null views/arguments, borrowed process survival and
an alternate binding. They do not prove server authentication.

The actual verify-s7-rpc-fixtures.ps1 monitor-rpc case, selected observer/cache,
ReportPrefix build/M0-T424/S7/r001/management-rpc-r002, passes actual exit zero
and PASS assertions against the final linked package. NTMON's link map assigns
both methods to common-rpc:management.obj. Management ownership and governance
reports are retained separately. No change to wire version, original mirrors,
guest media, shared KVM library or worker I/O transport was made here.
Full provenance/service separation and complete S7 runtime/publication gates
remain open; O:/winnt remains S6 and this is not a delivered P.

## Owner-separated service work and S7 closure gate

Owner directs service source separation into the next S8 and closure of S7's
common/two-protocol work. No service function is claimed moved or completed.
The former S8-S10 sequence shifts to S9-S11; T remains open. Retained partial
manual provenance and current candidate implementation are handoff inputs.

The previous disposable inventory is stale (134 definitions and removed
interface paths). tools/audit/Audit-S7ServiceOwnership.mjs now inventories the
actual tree, pins the three original source inputs, records byte/newline-normalized
mirror comparisons and common/protocol identities without claiming automatic
source attribution. Run with build/M0-T424/S7/r001/service-ownership-current-r001.json
reports 137 lexical definitions and eight protocol files. The service SHA256
is cd8830306db4b868514c0b8a7087183be6c8dc87bffc6c2c85363d4d42c78cff.
srvinit.c is byte-exact; srvvdm.c and client/vdm.c retain substantive baseline
differences, not newline-only changes. This three-input comparison is explicitly
not the complete selected-mirror sweep or completed S8 ledger.

Closure build and eleven-case actual RPC regression use fresh closure-r001
reports. All eleven actual fixtures pass exit-zero and PASS assertions:
client, bootstrap, startup-rejections, startup-timeout, native-worker-failure,
native-completed-worker-loss, workerless-grace, workerless-cancel, monitor-rpc,
native-command and native-registry. Closure-governance-r001 also passes.
Complete product regression, coherent publication and commit/push remain
required before S7 can be called delivered.

## Final bounded S7 verification and publication

This final section supersedes candidate-only checkpoint conclusions above,
without deleting earlier failures. Service source separation remains S8;
no full 137-function provenance ledger or service-private split is claimed.

Inputs are frozen in build/M0-T424/S7/r001/release-source-manifest.json
(112 changed/current source, test and tool inputs). The eight x86 product
identities are in release-candidate-manifest.json and published-manifest.json.
The validated dependency cache remains build/M0-T424/S2/r001, including the
WOW32 sub-build. New graphs, MIDL32, six EXE links, VDMREDIR and WOW links
pass; the VdmTib storage check remains enabled. APP_PROTOCOL_VERSION and
service.idl major are both 32, with unchanged application 0.0.424.

Actual commands and terminal evidence under build/M0-T424/S7/r001:

- build-products.cmd / build-products-r001.log completes product-programs
  and the WOW dependency check; closure-rpc-r001 retains eleven actual RPC
  process fixtures, all actual exit-zero/PASS, not observer-timeout passes.
- run-matrices.ps1 / matrices-r001.log and the two t424-s7-*-summary.json
  reports pass Console17 and Window17, 17/17 each. Z: is removed in finally.
- run-retained.ps1 / retained-r002.log passes same outer CMD DOS/native/DOS
  relaunch and cooked exit 19, independent-session management/exit 23 and all
  four frontend/worker-loss retirement cases. closure-edit-return-r001.txt
  proves real modern EDIT menus, Ctrl+Q, CMD echo and DOS MEM return.
- closure-common-units-r001.txt records native RPC 108/0, frontend RPC149/0,
  worker RPC164/0, management72/0, snapshot66/0, binding42/0 and transfer117/0.
  Native/frontend/worker handle deltas are zero; management allocations zero.
  GetNext ownership/disposal passes in closure-common-units-r002.txt.
- verify-common-lifetime.ps1 -Run closure-r001 uses the real execution unit
  in an observer-owned Console: 850/0, 12 completed, 16 cancelled, target
  survival, remaining-handles=0. verify-common-presentation.ps1 passes 415/0
  frame and 677/0 input checks. Common I/O client normal/broken-pipe variants
  pass; close-hang closure-r002 proves the original-shaped close callback
  marker and the existing bounded forced-close code 0xc000013a.
- run-versions-wow.ps1 / versions-wow-r002.log proves all five actual
  incompatible-server variants reject both run16 and NTVDM with 1306:
  old app, wrong protocol, wrong reply app/protocol and legacy RPC interface.
  WOW observations preserve WINMINE's visible guest main window and SOL/WRITE's
  existing memory dialogs. Their observer timeouts are observation bounds,
  not usability or completion passes. No gameplay, foreground focus or RDP
  pointer acceptance is claimed.
- closure-worker-equivalence-r001.json rejects fourteen exchange mutations;
  closure-ownership-r001.txt checks selected common providers, no duplicate
  embeddings, no reverse dependency and no service test seam in any product.

Retained failed harness attempts: retained-r001 launched the next test before
the previous broker's ordinary grace ended and was refused by the isolation
guard. r002 waits that exact process handle before the next independent test.
Direct no-Console calls to Console-dependent fixtures are invalid invocations,
not passes; the observer-owned reruns above are authoritative. The close-hang
wrapper initially expected zero instead of its pre-existing forced-close code;
source inspection and r002 require the real callback marker plus exact code.
Version r001 used the known unsupported long runtime path and NTVDM returned
161 before RPC; r002 uses only approved Z: and retains every mismatch assertion.
No production delay, fallback, authorization or acceptance assertion was loosened.

Source review finds only worker-I/O pipe creation in NTCON; common owns its
exact/partial/cancel/drain transport. Native command/startup/final-I/O and
frontend startup control now use authenticated RPC. Local diagnostic file
WriteFile calls are not message protocols. Original DOS/WOW adapters and
service authority remain independent; NTMON presentation/kinds/three hotkeys
remain unchanged. No MVDM/opennt-host mirror, guest, configuration or shared
KVM library is changed by S7. The selected three-mirror comparison above is
not misrepresented as a complete historical mirror audit.

publish-candidate.ps1 / publication-r001.log verifies all eight x86 products,
backs up the exact accepted S6 package and available configuration under
accepted-s6-recovery, then publishes all eight matching hashes to O:/winnt.
No guest/configuration overwrite, alias removal or new executable occurs.
Publication rollback restores the prior coherent eight-file set on failure.
The service-private split transfers intact to S8 with the source inventory,
partial manual review and test baselines; T424 remains open.

Postpublication verify-frontend-relaunch.ps1 uses O:/winnt itself and passes
DOS MEM/EXIT -> native VER -> DOS MEM/EXIT -> outer cooked CMD exit 19
(postpublication-r001.log). The exact eight published hashes and frozen inputs
are rechecked before commit. Documentation governance, relative links and
diff checks form the final delivery gate; the containing P1 commit and push
are the repository delivery, not a claim that S8 or T424 is closed.
