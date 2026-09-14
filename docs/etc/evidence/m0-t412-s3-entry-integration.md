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
