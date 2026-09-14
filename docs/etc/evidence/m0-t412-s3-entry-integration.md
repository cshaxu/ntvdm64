# T412 S3 entry integration

## Launcher process-support dependency

S2's classifier test supplied its own NtCurrentPeb, current TEB and process-heap
functions. That proved the original classification algorithm but left a real
run16 composition dependency unverified. The S3 test removes those definitions
and links the selected adapter-mvdm-host-out/win32/source/opennt_support_rtl.c.
No new process-support implementation is introduced.

The test checks that the provider initializes ImageBaseAddress from the actual
module. Native classification/path services remain selected through the formal
opennt-base-client and opennt-base-bindings archives. Link-map checks require
NtCurrentPeb, opennt_support_current_teb and RtlProcessHeap from support.obj,
not fixture.obj. The support TU is compiled with /Gy so unused unrelated
helpers are not pulled into this focused link. This is not yet the complete
run16 product link or an audit of every helper in that translation unit.

Verify-BrokerOriginalClassifier.mjs now writes under
build/M0-T412/S3/original-classifier and records the process-support selection.
With OPENNT_BROKER_OWNER_BUILD pointing to S2/formal-owners, the actual original
classifier plus real support pass MEM/COMMAND/EDIT DOS, WRITE Win16, absolute
and relative PE, DLL rejection, extension handling and missing-file error tests.
No guest program is executed or modified.

The three BaseInit suffix strings remain fixture-owned initialization, retaining
the original baseinit.c values from lines 270 onward and basedll.h declarations.
Run16 must compose that initialization rather than import the entire Base DLL
initialization shell. Other pending product dependencies include command-tail
handling, actual broker discovery, original launch calls and error propagation.
Removing test-owned process providers is not three-program acceptance and does
not permit deleting the old product entry before its replacement is verified.

## Actual launcher entry checkpoint

The subsequent `src/app/run16_entry.c` composition now supplies those three
suffix values and calls the same original classifier archive. It does not
implement a second image parser. Classification belongs to run16; BaseSrv
receives the classified request, and ntvdm remains the original-shaped worker.
Original BaseClient launch-update calls belong on the launcher side, with
original BaseSrv state updates on the service side, not a worker CLI policy.

The formal generator's explicit x86 `run16.exe` target links the original
classifier and finite path binding, actual process support and original RTL.
Its Console-subsystem CRT wWinMain entry supplies the unmodified command tail.
CommandLineToArgvW decodes the target only; CreateProcess receives a mutable
copy of the original tail. The full historical CreateProcess/CSR product shell
is an excluded dependency; this app-owned native launch uses the public API
under the approved CLI exception, not a copied replacement kernel algorithm.

Commands, from the repository root:

```text
powershell -NoProfile -ExecutionPolicy Bypass -File tools/build/New-T310OriginalSoftpcNinja.ps1 -Architecture x86 -BuildRoot O:/repos.hobby/ntvdm64/build/M0-T412/S3/product -NodeExecutable O:/.nvm/versions/node/v22.22.1/bin/node.exe -ParallelJobs 4
cmd /d /c build\M0-T412\S3\product\run-ninja-parallel.cmd run16.exe
node tools/audit/Verify-Run16Entry.mjs
```

Build artifact: build/M0-T412/S3/product/run16.exe, 157,184 bytes,
SHA-256 `7becc9c20ebeb1d6122393e5bee4c6ce579ff3af5741c3b5a5dc71c1232f1d52`.
The fixture builds its own native child beneath S3/entry-tests. Absolute and
relative quoted paths both preserve the exact UTF-16 command tail, including
spaces, empty arguments and backslashes; the child checks inherited directory
and environment, relays stdin to stderr and returns 23. Empty invocation returns
87, and a missing file returns 2. The map selects the original classifier and
excludes the old local VDM provider/machine shell. No CPU is executed in these
native-entry tests; this is not a CCPU or guest regression result.

MEM returns 50 with an explicit unfinished-dispatch diagnostic. This is a
temporary S3 implementation gap, not accepted DOS behavior or the final product
contract. Broker discovery/dispatch, worker creation/registration, DOS/WOW
delivery and original completion/rollback remain required. No staged or runtime
EXE was replaced, and neither S3 nor T412 is closed by this checkpoint.

## Server request-context binding

S3 removes the lifecycle fixture's constant OpenNtBaseServerRequestThread
provider. Original OpenNT public/sdk/inc/ntcsrsrv.h lines 175--176 define
CSR_SERVER_QUERYCLIENTTHREAD as a lookup of NT4's private TEB CsrClientThread
field. That field is not a usable standalone binding on modern Windows;
recreating the CSR thread runtime is excluded. The smallest retained-interface
binding is base_request.c in adapter-opennt-host/basesrv: a borrowed thread-local
original CSR_THREAD pointer and explicit bind/restore. srvvdm.c is unchanged.

This is a dispatch mechanic, not authentication or a replacement registry.
RPC dispatch must authenticate and obtain a registered live context before
binding, then restore it on every exit before freeing that context. No native
pointer enters wire records. Unbound lookup returns NULL, never a manufactured
default caller; original service entry must not run in that state.

`node tools/audit/Verify-BrokerOriginalLifecycle.mjs` passes with outputs under
S3/original-lifecycle, preserving S2 evidence. The map selects request.obj;
tests check empty initial TLS, nested restoration, independent enqueue-thread
binding and explicit teardown, together with all original lifecycle checks.
Process records and dispatch remain fixture-owned: this is not actual broker
command delivery or worker-death registration. Formal owner composition now
also includes this binding, without changing the deployed executable.
The formal `opennt-broker-owners` target in S3/product builds successfully;
rerunning the verifier with OPENNT_BROKER_OWNER_BUILD set to that directory
also passes and selects opennt-base-bindings:request.obj. Existing original
source compiler warnings remain; this is not a warning-free build claim.

## Remaining fixture-to-product dependencies

The actual lifecycle link map still used fixture-provided NtCurrentPeb,
current TEB and RtlProcessHeap. Those three definitions and their private
records are now removed. The test links existing opennt_support_rtl.c with
/Gy, checks real module/PID/heap values and asserts the three map owners.
With OPENNT_BROKER_OWNER_BUILD=S3/product, all original lifecycle tests pass,
including asynchronous retry and generation cleanup. No original service
algorithm or production support function was changed to obtain this result.

Console association remains explicitly fixture-local (value 1). Existing
process support initially uses stdout as ConsoleHandle; stdout can be a pipe
and is not cross-process Console identity. The test overrides that local field,
not a real Console registration. Product membership must use the admitted
Console query/association design. The result records this limitation and the
selected process-support source hash.

Remaining fixture providers are CsrClientCallServer and
CsrLockProcessByClientId/CsrUnlockProcess. Product counterparts require real
authenticated transport and registered-process lookup with pinned lifetime.
Original base/subsys/csr/server/process.c lines 943 onward acquires the process
lock and references the match; unlock dereferences and releases the lock.
A PID lookup returning an unpinned pointer cannot preserve that contract.
The full CSR runtime remains excluded. The nullable UserNotifyProcessCreate
symbol retains the already approved absent-hook disposition, not emulated
success. These tests do not yet establish a production broker or S3 closure.

## Registered-process lookup integration

S3 replaces the fixture's two Csr process lookup/unlock functions with
base_process.c in adapter-opennt-host/basesrv. The preceding map description
is the pre-integration checkpoint. Original srvvdm.c is unchanged. Existing
broker.c's test-only fixed-size array lacks process-object lifetime and the
original lock contract; it is not reused as a product provider.

Recovery ladder: original CSR process.c lookup/unlock were inspected, but the
TU requires excluded CSR root/thread/reference/deletion machinery. The finite
same-shaped binding retains lock-held service use over caller-owned original
CSR_PROCESS records. Its modern mechanics are a critical section, ListLink,
owned process references, monotonic sequence assignment and pins preventing
reentrant removal. It is not imported original code, an overlay, or task policy.
No CSR shell is recreated. Balanced lookup/unlock and quiescent destruction
are internal preconditions; the caller unbinds before releasing registry state.

Trusted registration duplicates a supplied process reference with local
query/duplication/synchronize rights and derives PID from GetProcessId. It
rejects duplicate records/PIDs and exhausted sequences; removal closes its
owned reference. Authentication and worker-role authorization must precede
this API. It is not a remote duplication service or launch reservation, and
does not enumerate unrelated processes or trust wire PID values.

The actual map requires the Csr functions from registry.obj. The formal
S3/product opennt-broker-owners target builds successfully. The lifecycle test
uses the real binding and covers original DOS/WOW operations, unknown PID,
duplicate registration, pinned removal, nonempty destruction, missing record,
invalid process reference, re-registration and sequence exhaustion. Run with
OPENNT_BROKER_OWNER_BUILD pointing to S3/product. Real cross-process removal
races, death-watch cleanup, authenticated worker registration and resource
receipts remain open integration gates. This local test cannot prove them.

## RPC attachment registration and rights correction

The first registry version requested query/duplicate/synchronize rights while
RPC peer fixtures supply only query-limited/synchronize. The local full-access
process fixture had hidden that mismatch. Registration now duplicates with
DUPLICATE_SAME_ACCESS and never widens permissions. A local test closes the
limited input, verifies the retained reference supports query/wait, and checks
that using it as a DuplicateHandle source process fails with ACCESS_DENIED.
Original remote resource duplication still requires the admitted receipt
binding, not implicit permission acquisition by this registration mechanic.

resource_registration.c is a test-only bridge that connects the actual RPC
receiver to the formal registry library. Registration follows real peer
authentication. After RpcServerUnregisterIf waits for pending calls, the test
queries the retained process object, removes the record and destroys the empty
registry. The incoming RPC attachment has ended but the owned reference remains.
Map assertions require opennt-base-bindings:registry.obj, not a test provider.

The nine existing resource cases pass with this integration: accepted calls
record registration and retained-after-call drain, while wrong scope/session/
peer cases do not register. Original local lifecycle tests also pass. The
scope/session negatives remain controlled expectations rather than genuine
other-user runs. This test does not authorize launcher registration of another
worker, implement launch reservations or deliver VDM commands. Those product
integration and death-race gates remain open; no deployment changed.

## Registry concurrent removal and real process exit

With the formal S3/product libraries, the lifecycle fixture now starts a
removal thread while the main thread holds CsrLockProcessByClientId. An event
establishes the remover has started; its completion must remain unsignaled
during the held-lock interval, while the pinned process reference remains
queryable. After CsrUnlockProcess, removal completes and clears the owned handle.
This tests the cross-thread lock lifetime, not just same-thread pin rejection.

The fixture also launches its own build-local executable in a hidden suspended
child mode, registers the actual process, resumes it, and verifies normal exit
zero. After closing the creator's handles, the registry still identifies and
waits on the exited process object. Explicit removal then makes lookup fail.
Only that owned child is involved; no deployed product or guest runs. Both new
checks and the existing full original lifecycle test pass. They prove local
registry lifetime across thread contention and real process exit, not an
implemented broker death watcher or automatic BaseSrv cleanup dispatch.

Original srvvdm.c resource review reconfirms the next integration constraint:
BaseSrvCreatePairWaitHandles and BaseSrvDupStandardHandles must receive their
resource-delivery result synchronously at the original NtDuplicateObject call.
Deferring acquisition to GetNextVDMCommand would move failure after original
record publication. The admitted recipient receipt/revocation binding remains
required; this registry verification does not replace that remaining work.

## Receiver-local stream and wait receipts

The S3 resource binding now has vdm_receipt.c/.h in broker. Original BaseSrv
retains task policy; unavailable CSR target-local HANDLE delivery is replaced
only by same-access local retention and numeric generation-scoped IDs. The
existing broker record fixtures do not provide resource ownership and are not
reused as a task provider. No original mirror or overlay was changed.

Authentication and typed OS file/event attachments precede receipt acceptance.
The caller owns serialization and prevents revoke during borrowed-reference use.
Console handles are excluded. IDs are issued only after duplication succeeds,
never wrap, and are not reused in the same generation. Repeat revocation is
harmless; drain closes remaining owned handles. Fresh generation assignment,
role authorization and synchronization belong to the surrounding endpoint,
not this storage routine. Initialization requires fresh or drained storage.

Verify-BrokerResourceAttachment.mjs compiles the production receipt TU with
MSVC x86 /MT. Its nine real-process cases now perform I/O through retained file
and event receipts, preserving file position and read-only access. They check
wrong-generation refusal, revoke/resolve failure, repeat revoke and final drain.
Startup checks reject invalid input and ID exhaustion without publishing an ID.
No guest/CCPU execution is involved. Receipt retention is not final wire
acknowledgement: product receipt IDs, aliases, lost-response rollback, concurrent
receiver serialization and original NtDuplicateObject/NtClose binding are still
required before command dispatch and three-program acceptance.

## Real receipt acknowledgement and remote revocation

The resource fixture now returns two fixed-width receipt IDs alongside its
existing OS attachment. A separate Revoke RPC checks packet privacy, actual
peer-process identity, the accepted peer and generation before invoking the
production receipt store. No sender-local HANDLE number is used as authority.
The receiver stays listening after Transfer and stops only after successful
revocation empties its receipts. The relay follows the same protocol downstream.

All nine existing cases pass: direct/relay success and read-only failure,
unavailable downstream, and identity negatives. For accepted callers the
transcript records wrong-generation rejection, two successful revocations of
the same file receipt and revocation of the event receipt. Existing file data,
position, rights and returned-object lifetime checks remain enabled. Thus the
test no longer relies only on process-exit drain to release delivered resources.
The original BaseSrvCloseStandardHandles may close stdout/stderr aliases twice;
this supports the required idempotent remote-revoke contract without editing
that original function.

This IDL is still a fixture protocol, not the final three-program command ABI.
Response loss before receipt IDs arrive, abrupt peer death, product concurrency
and source-shaped duplicate/close call-site integration are not proved here.
No VDM task was dispatched and no deployed executable changed. The complete
S3 product gate remains open despite successful resource-protocol evidence.

## Original resource-call binding and synchronous rollback

The formal BaseSrv library now binds NtDuplicateObject/NtClose via its private
base_server.h declarations to base_resource.c. srvvdm.c remains unchanged.
The wrapper preserves original arguments, synchronous status and cleanup calls.
A trusted thread-bound resource implementation receives both operations. A
missing bound operation fails; absent transport permits only current-process
native duplication and local close. Remote native duplication is refused,
not silently used as an alternative to authenticated attachment delivery.

This is the smallest same-shaped binding below original algorithms, not a
replacement resource/task policy or a CSR import. The binding TU alone defines
OPENNT_BASE_NATIVE_RESOURCES to call native APIs without remap recursion.
Dispatch must authenticate before binding, keep the local context alive, and
restore it on every exit. Native pointers remain process-local. The final
callback must distinguish admitted receipts from local token/event handles;
it cannot serialize or trust arbitrary source HANDLE values.

The formal S3/product owner build succeeds. The lifecycle map selects both
resource wrappers from resources.obj. A test invokes the original
BaseSrvCreatePairWaitHandles with delivery returning ACCESS_DENIED: it observes
one duplicate attempt followed by the original close callback, verifies the
event is no longer valid, and receives the original failure. The rest of the
original lifecycle suite passes unbound through the local-only native route.
A foreign process-handle test requires NOT_SUPPORTED rather than native fallback.
This verifies the source call site and failure ordering, not successful remote
delivery through a finished product callback. Connecting authenticated receipts,
partial-stream rollback and lost-reply cleanup remain required S3 work.

## Partial standard-stream delivery constraint

The actual original BaseSrvDupStandardHandles is now tested with a controlled
first-duplicate success and second-duplicate ACCESS_DENIED. It calls duplication
twice, performs no close, and leaves StdIn in destination form while StdOut and
StdErr retain source form. The fixture uses distinct numeric markers only;
it creates no remote resources and does not claim a measured OS handle leak.
The original source is unchanged and the formal lifecycle suite passes.

Caller review confirms BaseSrvCheckDOS's ready branch immediately leaves on
duplication failure; its busy branch closes the wait pair and frees the record.
BaseSrvUpdateDOSEntry likewise closes the recorded wait pair on stream failure.
These calls cannot be assumed to revoke every already acknowledged stream.
This is a transport-integration constraint, not a license to rewrite original
service policy or interpret every field as a destination receipt after failure.

The pending resource binding must track the exact successful acquisitions of
the current request independently of those mixed fields, revoke only those
pending receipts on failure, and commit their ownership only at the original
successful publication boundary. Original explicit closes still revoke their
identified receipts idempotently. The production transaction/receipt wiring
has not been implemented or accepted by this test; it is the next required
integration step before the current callback can safely deliver real streams.

## Original successful stream identity and close sequence

The formal lifecycle test now also executes original BaseSrvDupStandardHandles
and BaseSrvCloseStandardHandles for distinct streams and shared stdout/stderr.
Its callback assigns distinct test-only destination markers, with no OS handles
or autonomous implementation of the original helper. Distinct streams cause
three duplication calls. Shared stdout/stderr causes two and preserves the
same destination marker for both output fields. Every duplicate uses the
original OBJ_INHERIT / DUPLICATE_SAME_ACCESS shape.

Original cleanup issues three DUPLICATE_CLOSE_SOURCE calls even for shared
stdout/stderr, then zeroes all fields; repeating cleanup issues no further
calls. The test checks argument shape, exact marker sequence and field clearing.
It passes with the same formal original-owner libraries and complete lifecycle
suite. Together with the preceding partial-failure test this fixes the required
receipt identity/cleanup contract: preserve output alias identity, tolerate its
repeated revocation, and separately journal only completed acquisitions on
failure. It does not prove remote standard-stream dispatch or handle leak
freedom, and does not change the original source or deployed executable.

## Pending acquisition journal

vdm_delivery.c/.h implements only the cleanup bookkeeping required by the
source-proven mixed-field failure. Original CSR native-handle ownership cannot
serve as cross-process receipt bookkeeping; original BaseSrv remains the
algorithm owner. Existing receipt storage owns received resources, not sender
acquisitions. The journal therefore reserves memory before delivery, stores
validated generation/receipt acknowledgements, and permits ownership commit
only when every tracked acquisition is known. No original mirror is modified.

Rollback visits the known acquisitions and retains failed revoke attempts;
unknown replies stay pending rather than being treated as no delivery. Explicit
forget is allowed only after definitive non-delivery or confirmed recipient
rundown. All local recipient contexts must outlive pending work; the owner
serializes operations, and callbacks must not reenter the journal. Commit only
releases bookkeeping and adds no fallible remote operation after publication.

`node tools/audit/Verify-VdmDeliveryJournal.mjs` builds x86 /MT below
build/M0-T412/S3/delivery-journal and passes partial-acquisition rollback,
unknown-reply retention, revoke failure/retry, successful commit and sender
event survival. It uses production receipt storage and real local event
references; the failed revoke result is controlled, not a real broken RPC.
This is not final sender dispatch wiring, loss reconciliation or rundown proof.
Those remain mandatory before the source callback can publish real commands.

## Original partial delivery with actual received file ownership

The formal lifecycle fixture connects original BaseSrvDupStandardHandles through
base_resource to production receipt storage and the delivery journal. It reserves
bookkeeping, retains a real build-local file, and acknowledges that receipt;
the second acquisition is explicitly rejected before delivery. After the original
function returns ACCESS_DENIED, journal rollback removes the received reference
without using mixed stream fields. The sender file remains writable. Only the
fixture's temporary file is closed/deleted afterward. Original source is unchanged.

This exposed a receipt attribute mismatch: original standard-stream duplication
requests OBJ_INHERIT, but waiting-event duplication requests no inheritance.
Same-access retention now follows these admitted roles instead of always FALSE.
Actual handle flags verify inheritable streams and non-inheritable wait events.
The formal original lifecycle and local journal tests pass; the existing nine
RPC scenarios are the cross-process regression for this change.

This is concrete local source-to-receipt rollback, not merely numeric markers.
The callback still uses an in-process receiver and controlled second failure;
real RPC source dispatch, reply-loss recovery, commit placement and three-program
execution remain required. No deployed executable changed and S3 stays open.

## Formal transport archive integration

The x86 formal generator now builds `broker-transport.lib` alongside the
original Base owner archives. It contains the existing rpc_security,
vdm_receipt and vdm_delivery production units, using native SDK declarations
and /MT rather than the imported OpenNT forced-header island. Source hashes,
object membership and pending-product disposition are in the graph manifest.
There is no new runtime policy or mirror change in this build integration.

Regenerated build/M0-T412/S3/product with New-T310OriginalSoftpcNinja.ps1,
Architecture x86, Node v22.22.1 and ParallelJobs 4; built the explicit
opennt-broker-owners target. The initial sandbox Ninja invocation produced no
compiler progress and was stopped by its verified process identity; its handle
then reported exit 1. The elevated retry completed all four compile/archive
steps with exit zero. Archive SHA-256:
`3e381499094c55b24ea0d7b38daced83494ec8d610b41325174df1cd16e69530`.

Verify-VdmDeliveryJournal.mjs, Verify-BrokerOriginalLifecycle.mjs (formal
OPENNT_BROKER_OWNER_BUILD), and Verify-BrokerResourceAttachment.mjs all pass
after linking this archive instead of compiling private copies of those
production units. Each checks the linked map for its actual archive providers.
The lifecycle run preserves original command/capture/wait/rollback checks; all
nine RPC cases retain their previous limits, including controlled scope
negatives and integrity upgraded by the runtime to packet privacy. Outputs
remain in the corresponding S3 build test roots. No guest run or deployment
was performed. Product command dispatch and the three executable links remain
incomplete; this archive is not evidence of their acceptance.

## Original-service dispatch binding

Inspection of the existing base_vdm_record confirmed that its fixed sixteen
slots and DOS-only subset omit StartupInfo/Desktop/Title/Reserved fields of
the restored Base messages. It is not selected as the new command transport.
The eleven recovered APIs now have one finite local OpenNtBaseDispatch entry
in adapter-opennt-host; the lifecycle fixture's duplicate handler switch is
removed. No original service body or queue/capacity policy changed.

Recovery ladder: the mapping owner is pinned OpenNT
base/win32/server/srvinit.c, BaseServerApiDispatchTable (file SHA-256
`f53d4ca6f7d3eee8945d94fc237f73b892fb0f509da8a47256043180b48bfc70`).
Its full initializer and CSR dispatcher require the excluded CSR runtime and
unrelated Base services. The selected smaller binding preserves those eleven
API/handler pairs and the original basemsg.h native structure sizes. There is
no mirror intrusion or invented replacement service algorithm. New boundary
checks reject unbound callers, unknown operations and incorrect native lengths;
the trusted local thread supplies ClientId. Wire decoding, payload/resource
validation, serialization and authenticated context lifetime remain mandatory
caller responsibilities, not functions of this native-pointer API.

Regenerated S3/product and built opennt-broker-owners: x86 dispatch object and
formal binding archive link pass. Verify-BrokerOriginalLifecycle.mjs with the
formal owner build passes all existing original lifecycle checks and verifies
the dispatch map provider. New negatives prove unbound, short, long and
unadmitted requests do not consume original first-VDM state; the next valid
request succeeds and replaces a forged message ClientId with the bound local
identity. This is local dispatch acceptance, not authenticated RPC command
delivery, a complete basesrv executable or guest execution. S3 remains open.

## Copied command buffer spans

Original basemsg.h CheckVDM/GetNextVDMCommand and srvvdm.c capacity/copy
paths carry eight variable buffers, plus separate STARTUPINFO and native
resources. A present output buffer with capacity is not an incoming byte
string; a required-length response does not prove that many bytes were copied.
The vdm_payload fragment preserves presence, original length and copied byte
count independently for command/application/PIF/directory/environment/desktop/
title/reserved fields.

Recovery disposition: original client/server copying and capacity policy stay
unchanged. CSR capture pointers require the excluded shared CSR address
translation and cannot be sent to another process. This is the admitted finite
copied-byte binding, not a replacement queue/provider or original algorithm.
Fixed-width little-endian fields on the selected x86 graph contain no native
pointer or resource. Presence does not authorize memory access: the eventual
operation decoder must validate semantic lengths and materialize local buffers.

Regenerated S3/product and built opennt-broker-owners with the vdm_payload
member in broker-transport.lib. Verify-VdmPayload.mjs passes with map-provider
and PE/x86 checks, all eight copied fields, present/empty distinctions,
short-output nonmutation, every truncated size, overlap/noncanonical offsets
and overflow rejection. Outputs are below build/M0-T412/S3/payload.
StartupInfo, resource IDs, protocol envelope, native-message translation and
actual RPC command integration remain missing; this fragment is not a complete
command protocol and its test is not three-program or guest acceptance.

## Startup numeric binding

Original client vdm.c BaseCheckVDM's Copy startupinfo block copies only
dwX/dwY/dwXSize/dwYSize/dwXCountChars/dwYCountChars/dwFillAttribute/dwFlags/
wShowWindow and constructs cb. Strings and standard streams are separate Base
message fields. GetNextVDMCommand subsequently rebinds string pointers to its
caller's buffers. No native STARTUPINFO pointer, reserved pointer or HANDLE
belongs in this fragment.

The original capture algorithm is retained. Reusing its native structure as
wire bytes requires the excluded CSR address/handle environment. The finite
base_startup adapter selects exactly the nine original scalars plus presence
in forty fixed-width bytes. Decode constructs local cb and zeroes unselected
members; invalid presence, show-width overflow and nonempty absent forms fail
without modifying output. NULL StartupInfo remains absent. This is transport
binding, not a new startup policy or imported original algorithm.

Regenerated S3/product and built opennt-broker-owners. The formal lifecycle
test now encodes/decodes actual BaseCheckVDM startup captures before original
server dispatch and restores the captured pointer before original cleanup.
Verify-BrokerOriginalLifecycle.mjs passes with formal map-provider checks,
all nine scalar values, absence, poisoned native members and malformed-form
negatives, plus the original lifecycle/capture/rollback suite. This is local
marshalling evidence, not RPC command delivery or three-program acceptance.
Full envelope, resource identities and endpoint composition remain open;
no runtime executable was deployed.

## Receipt role validation before native materialization

Reviewing the pending command-resource decoder found that receipt entries
retained a native reference but discarded the accepted stream/wait role.
Generation/ID lookup alone could therefore materialize an event for a stream
field. The finite receipt binding now retains that role and requires the
operation decoder's expected role at lookup. Wrong families, and distinct
parent/worker wait roles, fail with ACCESS_DENIED and a NULL output. Invalid
expected roles fail INVALID_PARAMETER. The three standard streams share one
family, preserving original BaseSrvDupStandardHandles stdout/stderr aliases
without widening retained access rights. The expected role is not taken from
an untrusted receipt claim. Kernel object-type validation remains the typed
authenticated attachment endpoint's prerequisite.

This corrects the modern unavailable-CSR resource binding, not original BaseSrv
policy. Regenerated S3/product and rebuilt the formal transport archive.
Verify-VdmDeliveryJournal.mjs and formal Verify-BrokerOriginalLifecycle.mjs
pass. Verify-BrokerResourceAttachment.mjs passes all nine real-process cases;
each positive receiver transcript must prove event-as-stream and
stream-as-event rejection plus stdout receipt resolution as stderr. Existing
shared position, read-only denial, remote revoke and ownership checks remain.
The journal fixture also checks parent/worker role mismatch and invalid role.
All outputs remain under the existing S3 build roots. Actual command RPC
decoding, resource callback integration and three-program execution are still
open; no deployment or guest acceptance is claimed.

## CheckVDM native payload binding

base_payload now maps all eight BASE_CHECKVDM_MSG variable fields to the
existing pointer-free payload fragment and back. Original basemsg.h supplies
the field identities and USHORT/ULONG widths; original BaseCheckVDM and
BaseSrv command copying retain ownership and policy. The excluded CSR shared
address translation is replaced only by borrowed pointers into checked copied
bytes. No mirror change, new queue, capture allocator or service policy is
introduced. The decoder checks all spans and native widths before mutation;
CheckVDM input requires copied bytes, not GetNext's capacity-only form.
Operation-specific string validation and authentication still precede service
dispatch in the eventual endpoint. This helper is not a full hostile-input
RPC decoder.

Formal S3/product owner build passes. Verify-BrokerOriginalLifecycle.mjs now
copies real BaseCheckVDM captures through the native payload and startup
bindings before dispatching original BaseSrv, then restores original capture
pointers before cleanup. The formal payload object is verified in the map.
All original lifecycle/capture/failure tests pass, plus explicit rejection of
a 65,536-byte command length and a capacity-only CheckVDM input with unchanged
destination fields. Test-owned payload storage is released after synchronous
dispatch; only original server-owned command copies survive. Outputs remain
under S3/original-lifecycle. Full command envelope, receive/reply translation,
resource identities and actual RPC service integration remain unfinished.
No three-program acceptance or deployment is claimed.

## PIF reply lengths are not write counts

Return-path review found a wrong codec assumption: copied bytes were required
not to exceed original length. Original BaseSrvFillPifInfo first writes NUL
to requested PIF/title/current-directory buffers, then reports source lengths
which may be zero. Even its error path can clear strings before returning.
Generic success-only or returned-length-only copying would lose writes.
The original function is unchanged. The codec now separates copied bytes from
returned lengths; canonical spans, overflow and presence checks remain.
Native CheckVDM input still enforces exact byte/length equality and widths.
Future reply decoding must validate bytes against independently retained
destination capacities, never against returned required length alone.

Formal owners/transport rebuilt under S3/product. Verify-VdmPayload and formal
Verify-BrokerOriginalLifecycle pass. The latter calls original BaseSrvFillPifInfo
with empty source info and sentinel-filled PIF/title/directory buffers: returned
lengths are zero, first bytes become NUL, second bytes are unchanged. The codec
now carries these three single-byte writes with zero reported lengths. Existing
CheckVDM negatives and lifecycle tests pass. Full GetNext request/reply binding
remains incomplete; this is a source-proven transport correction, not real RPC
command execution or three-program deployment.

## GetNext response buffer application

OpenNtBaseApplyGetPayload now binds a validated copied reply to the original
client's saved native pointers/capacities. It checks all eight spans, matching
presence, actual copied-byte capacity and USHORT length widths before writing
any destination. Only copied buffers and returned length fields change; no
status, resource, startup or wait/consumption policy is introduced. Native
storage and nonoverlapping payload remain caller-owned for this synchronous
operation. This finite binding replaces only unavailable CSR response-address
translation; original GetNextVDMCommand remains responsible for its subsequent
source-defined user-buffer copy, error and retry behavior.

Formal S3/product owner build and Verify-BrokerOriginalLifecycle pass with the
new provider in the map. The original PIF output fixture now applies its encoded
three terminator writes to separate destination buffers: zero required lengths
are preserved, first bytes change and trailing sentinels do not. A zero-capacity
late title field rejects the whole application without changing earlier PIF/
directory bytes or message fields. Existing source and lifecycle tests pass.
This proves local reply application, not full reply construction, RPC exchange,
worker command execution or deployment. Those S3 requirements remain open.

## Complete local GetNext buffer roundtrip

The finite native payload binding now encodes GetNext pointer presence and
capacities without reading uninitialized capture buffers. Preparation checks
all spans and width/sum overflow, then allocates a single zeroed block for
native output buffers and the eventual reply header/data. All required storage
is acquired before original command consumption; finishing writes only the
returned length metadata, without an additional allocation. Reply data covers
the saved initialized capacities independently of returned lengths, preserving
partial/PIF writes without exposing server heap contents. Original capture,
selection, status and retry algorithms remain unchanged. This is an explicit
finite replacement for unavailable CSR shared capture-address translation,
not a new command provider or generic capture runtime.

Formal S3/product owners build passes. The lifecycle fixture now routes every
original BaseClient GetNext call through prepare, original BaseSrv dispatch,
finish, restored client capture pointers/capacities, apply and release. Startup
and native resource exchange are not thereby transported. Formal map checks
select all four new functions from the same binding archive. All original
lifecycle tests pass, including capacity failure and wait/retry; focused checks
prove overflowing aggregate capacity is rejected before mutation, initial
buffer bytes are zero, required length can exceed copied capacity in the reply,
live context reuse is rejected and release is repeat-safe. Heap-allocation
failure is returned explicitly but was not fault-injected in this step.
Outputs remain in S3/original-lifecycle. Full operation envelope, authenticated
RPC command exchange, resource callbacks and three-program execution remain
required; no deployment or guest acceptance is claimed.

## CheckVDM text termination boundary

Before admitting network data to original string consumers, the native
CheckVDM payload decoder now requires a command and NUL termination of every
present input field. Present zero-length buffers are rejected instead of being
materialized as aliases to unrelated payload data. Environment input accepts
one NUL for the empty environment; otherwise its final two bytes must be NUL.
This follows original OpenNT base/win32/client/vdm.c BaseCreateVDMEnvironment:
pDst starts at pNewEnv, the entry loop is skipped for empty input and the final
single UNICODE_NULL is included in the converted length (external source lines
3089-3181). BaseCheckVDM explicitly terminates the other captured strings.
No text conversion, shell parsing or program-selection policy is added, and
no mirror code changes. This is validation at the unavailable CSR boundary,
not permission to trust a complete unauthenticated message.

Formal S3/product build and original lifecycle suite pass. Focused checks
accept single-NUL empty environment and reject unterminated command text,
nonempty environment lacking its double terminator, present zero-byte command
and absent command. Malformed-text checks preserve the destination message.
Each negative is isolated from other malformed fields. Original BaseCheckVDM
captures and complete local GetNext buffer roundtrips still pass. Evidence
remains local/native; operation/resource authentication and actual RPC command
dispatch are pending. No deployment changed and S3 remains active.

## Versioned envelope and original API mapping

The S1 version/operation/request/generation contract now has a 32-byte fixed-
width envelope in broker/vdm_message: exact lengths, eleven operations,
nonzero request ID, generation, direction and reply status. Validation uses
a separately trusted generation and expected direction, leaving output
unchanged on failure. This replaces unavailable CSR metadata only. It does
not authenticate peers, validate operation bodies, correlate a reply to a
saved request or authorize replay. No native resource or claimed sender
identity is transported. Original service policy remains unchanged; the old
wire/user-key and fixed-slot DOS record providers are not selected.

The native binding maps standalone operations to the eleven original Base API
numbers. Original BaseClient lifecycle calls now use this mapping before
source dispatch; CSR numeric APIs are not the wire namespace. Formal S3/product
owners/transport build passes. Verify-VdmPayload passes all eleven envelope
operations and version/length/direction/generation/request-ID/status negatives.
Formal Verify-BrokerOriginalLifecycle passes with map-provider checks. Empty
test bodies prove header validation only, not semantic validity of a command.
Body bindings, actual RPC exchange, resource callbacks and three product entry
points remain unfinished; no deployment changed.

## Original service reached through authenticated RPC

Verify-BrokerResourceAttachment now links the formal original server archive,
dispatch/registry bindings and process support into its test server. Link-map
checks require BaseSrvIsFirstVDM from srvvdm.obj and the production operation
dispatcher, rather than a test implementation of service policy. The RPC
attachment is checked against the authenticated runtime caller PID and retained
registered process before dispatch. This operation needs no remote thread ID;
the bridge does not invent one from the server RPC thread.

The nine-case suite passes. Each positive server receives an invalid generation
request first (rejected without source dispatch), followed by two valid queries
that return the original first-VDM flag as 1 then 0. Both relay and downstream
servers are checked independently. Existing receipt revocation, read-only
rights, scope/session mismatch and wrong-peer checks continue to pass. Generated
logs and maps are in build/M0-T412/S3/rpc-resource.

This proves actual RPC reaches an original service with authenticated registered
context. The endpoint and serialization lock remain fixture composition, not
the final basesrv executable. Full command/resource exchange and three-product
DOS execution are still pending. No deployed executable changed.

## Launch/update/command scalar fragments

The formal binding archive now includes base_values.c. Its operation-selected
24/16/28-byte fragments carry all non-buffer numeric CheckVDM, UpdateVDMEntry
and GetNextVDMCommand fields from original basemsg.h. They deliberately exclude
Console, process, stream and wait HANDLEs and StartupInfo pointers. No original
service body is changed. Validation checks the exact fragment size and every
USHORT/WORD/BOOLEAN width before changing any native field; original service
policy remains responsible for interpreting state, flags and task IDs.

The formal owner build and original lifecycle suite pass. Distinct numeric
values verify field order independently of the encoder/decoder roundtrip.
Every truncated size and each narrowing overflow are rejected without native
message mutation. Actual original client requests and service responses also
roundtrip these scalars while whole-message comparison protects unrelated
pointer/resource fields. Map checks require the formal values.obj provider.
Only inherited private-header anonymous-union warnings remain for the new unit.

These are still fragments, not a complete authenticated command exchange.
Envelope/body combination, native attachment resolution, product entry points
and real DOS execution remain S3 obligations. No deployed executable changed.
## Composed CheckVDM copied request

base_command now combines the versioned envelope, CheckVDM scalar fragment,
startup fragment and eight copied buffers. The 96-byte fixed prefix is followed
by the canonical buffer payload. It contains no process, Console, stream or
wait HANDLE. The decoder validates the entire request before publishing native
scalar/pointer fields, startup storage and request ID; locally bound resource
fields remain unchanged. Borrowed storage lasts through synchronous dispatch.

The formal owner build and original lifecycle suite pass using this composition
for actual original BaseCheckVDM requests, replacing fixture-only assembly of
the separate fragments. Every truncated request, a wrong registered generation
and invalid startup presence fail without changing any native output. Existing
source-owned DOS/WOW admission, command ownership, resource rollback and waits
remain passing. Maps select the formal command.obj provider. This is copied
request integration, not authenticated resource delivery or DOS execution.
Full RPC transaction and three-product entry/deployment gates remain open.

## First DOS admission and original abort over RPC

The authenticated resource RPC suite now submits a copied CheckVDM request
through the production command decoder and dispatcher to original srvvdm.c.
Its bounded no-Console/no-worker case returns the original nonzero DosSesId and
VDM_NOT_PRESENT without a parent wait handle. The fixture verifies the original
record's command survives erasing the decoded request buffer, then calls
original UpdateVDMEntry/UPDATE_VDM_UNDO_CREATION with VDM_PARTIALLY_CREATED and
checks that the record is gone. No test-owned task table or cleanup algorithm
substitutes for those original paths.

All nine RPC cases pass, including independent relay/downstream positive
instances. This advances real transport evidence from first-VDM flag queries
to actual command ownership and partial-launch rollback. The generated logs
contain ORIGINAL-CHECK copied=1 no-worker=1 abort=0. The fixture deliberately
uses no guest, no worker and no standard streams for this admission; its
immediate abort is a test, not the product launch flow. Existing authenticated
file/event attachment tests remain separate from source-site resource delivery.

Original BaseSrvCheckDOS allocates/copies on the absent-record path. The parent
event and standard-stream delivery belong to later update/reuse paths, not
this first admission. Actual run16 worker creation/registration, resource
callbacks, full command return and three executable deployment are not complete.

## Formal basesrv executable connection composition

The formal graph now builds basesrv.exe from app/basesrv_entry.c, native MIDL
service stubs, original BaseSrv and finite service/registry/request bindings.
Its manifest records entry/IDL hashes and selected libraries. The product map
selects original BaseSrvIsFirstVDM and service.obj; no fixture implementation
is linked. This is an explicit build-only target, not a publication migration.

Verify-BasesrvProduct passes against that actual x86 executable: authenticated
connection, duplicate PID registration refusal, wrong-generation query and
disconnect refusal, first-VDM 1 then 0, explicit disconnect and reconnect with
a newer generation while preserving the service-global flag. A second owned
basesrv returns RPC_S_DUPLICATE_ENDPOINT (1740), leaving the first responsive.
All outputs are under build/M0-T412/S3/basesrv-product. Readiness is established
by successful RPC, not the diagnostic listening line.

The native connection holds an original registry record and restores bounded
request/registry thread bindings around source dispatch. RPC context rundown
owns dropped registrations; abnormal-loss/concurrent-rundown tests remain open.
No commands/resources are yet admitted by this product interface, so registration
teardown is not presented as task cleanup. First-VDM uses original policy and
the service lock required by S1, not a replacement flag. The existing original
BaseClient/BaseSrv lifecycle suite also passes with the updated formal archive.

The verifier terminates only its owned server after explicit client disconnect.
Idle/empty shutdown, all source command/resource paths, launcher integration,
worker composition and three-program DOS execution remain unfinished. The
deployed ntvdm32.exe is unchanged; basesrv.exe is not ready for user deployment.

## Independent worker composition audit

At e33bad7e7 the existing product still selects app/entry.c and
launch_declaration.c. The original worker entry is present:
mvdm-host/softpc.new/obj.vdm/ntvdm.c runs TimerInit, CpuEnvInit, nls_init and
host_main. The formal graph renames only its main symbol to
mvdm_softpc_original_entry, which machine_shell invokes through
adapter-mvdm-host-out/softpc/mvdm_softpc_execution.c. A renamed ntvdm32 binary
would therefore retain the unwanted outer launcher/local-service composition;
it would not create the independent worker.

The required cuts are source-specific, not a whole adapter-directory deletion:

| Current owner/call | Independent-worker disposition |
| --- | --- |
| app/entry.c: consume_options, resolve_requested_command, requested_image and native/shell launch | Exclude from worker entry. run16 owns target classification and launch through original BaseClient. |
| launch_declaration.c: prepare_softpc_arguments | Remove private argv injection from worker. run16 supplies the original BaseGetVdmConfigInfo command line to CreateProcess; OS command line and CRT argv then describe the same invocation. |
| entry.c: mvdm_base_vdm_environment_prepare/restore | Do not project and restore environment inside worker. Original parent-side BaseCreateVDMEnvironment supplies the child environment at process creation. |
| entry.c: app_launch_declaration_bind/publish | Exclude local command publication and command-exhaustion policy. Worker binds authenticated original BaseClient transport before entering the original worker. |
| selected base_vdm_client/local/broker.c | Exclude these three local provider units from final worker link; select original client vdm.c and real CsrClientCallServer transport instead. |
| machine_shell and mvdm_softpc_execution | Retain required session activation, thread binding, guest-memory lease and original entry/cleanup ordering. No evidence here authorizes removing these mechanics or changing CCPU. |

The same BaseSrv adapter source list also contains mvdm_command_guest_state,
mvdm_command_native_child and image-classification bindings. Their BOP/reentry
consumers prevent indiscriminate removal with the three obsolete local service
units; their final call selection remains the S4 source-route gate.

The immediate dependency is a complete original BaseClient transport, including
GetNextVDMCommand's buffers, returned resources and wait/retry result. The formal
basesrv endpoint currently supplies only registration/first-VDM. Creating an
ntvdm link against local fallback or success stubs would hide this dependency
and violate S3. This audit changes no worker behavior or published artifact and
does not establish worker build, DOS execution or task closure.

## Composed GetNext request and reply

base_command now composes both sides of GetNextVDMCommand. Source inspection of
client/vdm.c shows StartupInfo is allocated as an output capture; CodePage,
creation flags, drive and BAT state are output fields, and iTask is input only
for ASKING_FOR_PIF. The request encoder therefore reads state/exit code, the
conditional PIF task, buffer capacities and startup presence only. It does not
serialize output scratch bytes. Server decode rejects noncanonical output-only
values and materializes cleared native output storage.

Prepare reserves the copied payload and complete reply allocation before
original dispatch. Finish copies into this reserved reply without allocating
after source command consumption. Apply validates generation, request ID,
direction, widths, startup and all saved capacities before writes. Original
NTSTATUS is retained in the response, including short-buffer failure; original
BaseClient still owns the subsequent copy/error/wait/retry behavior. Resource
fields are neither serialized nor replaced by this codec.

Formal build and original lifecycle tests pass with actual original client
GetNext calls through the composed path, including PIF, environment/capacity,
startup, waits and retry-cleared exit code. Focused tests encode a deliberately
unreadable output StartupInfo pointer without dereferencing it, suppress poisoned
output-only scalars, preserve short encode output, reject every truncated
request and wrong generation, and verify repeat-safe release. Reply generation
and request-ID mismatches are rejected in the actual lifecycle path. Maps select
the formal command.obj functions. Real RPC GetNext resource transfer and the
independent worker remain pending; deployment is unchanged.

## Check/Update input direction review

Original BaseCheckVDM initializes VDMState to FALSE and receives iTask from the
server. Its request encoder now sends neither caller scratch value; decoder
requires both canonical zero values before original dispatch. This corrects
the earlier direction-neutral scalar-fragment composition without changing
original source. Valid input code page, drive, binary type and flags remain.

The 48-byte UpdateVDMEntry request now combines envelope and original scalar
inputs. Task ID remains input to the original record lookup for every entry;
creation state is input only for UPDATE_VDM_UNDO_CREATION. Process/wait/Console
handles stay in the separate authenticated binding. The three original entry
indexes are validated before dispatch; no replacement update/undo policy is
introduced. An explicit original task input is required, including direct
hooked-CtrlC messages; no uninitialized task value is invented or normalized.

Formal compilation, nine-case RPC regression and original lifecycle pass. Actual original
BaseUpdateVDMEntry calls now use the composed request. Focused tests verify
poisoned Check output fields are omitted, Update entry-specific field values,
every truncated Update length, request-ID publication and unchanged native
resource fields. Existing command/capacity/startup/wait/cleanup cases remain
passing. Source resource callbacks and complete product command admission are
still unfinished; this is not three-program acceptance.

## Receiver ownership publication

Original client GetNext waits on WaitObjectForVDM without closing it between
retries; original ExitVDM finally closes the returned wait handle. Standard
streams likewise become original consumer-owned handles after command delivery.
Receipt resolve alone cannot implement that handoff: a later drain would close
a handle already owned/closed by the original consumer. Duplicating on every
lookup would also break stable wait identity and leak references.

broker_vdm_receipt_take now removes the validated generation/role receipt
without closing or duplicating its native reference. Resolve remains borrowed;
take explicitly transfers ownership. Aliases must be grouped before taking one
ID once, and the caller must finish validation/publication and serialize access.
This does not change original service policy or authorize indeterminate replay.

The formal transport archive and Verify-VdmDeliveryJournal pass. Real event
tests reject wrong role/generation, preserve the same handle on take, reject
repeat take and retain validity after revoke/drain. Real pipe tests preserve
stdout/stderr alias identity and inheritance; after sender and receipt references
are gone the taken writer still works, and only its final close produces
ERROR_BROKEN_PIPE on the reader. Existing partial/unknown/retry/commit tests pass.
This is native ownership evidence, not a guest run or completed source callback.
Integration into authenticated recipient endpoints and original close paths
remains required before enabling product command delivery.
