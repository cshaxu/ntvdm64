# adapter-opennt-host

BaseClient's base_config binding supplies caller-owned immutable package paths
to original BaseGetVdmConfigInfo. Run16 must validate files and convert paths to
ANSI without loss before initialization and own binding lifetime on its thread.
The original 256-byte configuration envelope is retained and rejects oversized
inputs. No registry access, image classification, process launch or arbitrary
command parser lives here. The worker-path-end helper replaces only the original
system32 pathname assumption; these configuration records never cross RPC.

The BaseClient capture-memory binding is declaration-only in base_capture.h:
the process supplies CsrPortHeap as a local heap, initializes it before calls
and destroys it only after requests drain. All three memory routines now come
from original opennt-host/base/ntdll/csrutil.c, replacing test-authored bodies.
Original pointer-offset tables contain local addresses and never form the RPC
wire format. Counts/spans are supplied by the original bounded callers, not
remote peers; the future transport validates copied message fields separately.

This component is the package-private historical host-interface adapter for
`opennt-host`. It has one named subfamily for each accepted non-MVDM OpenNT
owner package that needs a private historical binding. BaseSrv/client VDM is
the first such package, not the component's permanent scope.

The current BaseSrv/client subfamily may preserve only the exact calling shapes
reached by its owning `opennt-host` package:

- client CSR capture, message dispatch and wait/retry transport;
- server CSR request/reply, process/thread lookup and bounded registration;
- event-pair, duplicated-handle and cleanup operations required by the
  original BaseSrv record lifecycle; and
- the original WOWEXEC registration notification shape when its WOW owner is
  explicitly admitted.

It must not receive BOP, DOS, WOW guest-frame, SoftPC/Bochs or generic MVDM
semantics. `mvdm-host` does not include or call this component directly.
Every exported binding must retain an OpenNT source spelling/ABI/failure row
in the shared host tracker and identify its `opennt-host` caller.

T412 S2 admits the finite BaseSrv/BaseClient subfamily described in the
[consolidated design](../../docs/etc/evidence/m0-t412-s1-design-closure.md).
`basesrv/include/base_client.h` now retains the four reached declarations from
OpenNT `public/sdk/inc/ntcsrdll.h`, without importing its CSR runtime:

| Binding | Original caller and contract | Current implementation status |
| --- | --- | --- |
| CsrAllocateCaptureBuffer | Original GetNextVDMCommand; allocate bounded capture or return NULL. | Test transport only; product binding pending. |
| CsrAllocateMessagePointer | Original GetNextVDMCommand; source-shaped capture allocation with aligned length and returned pointer. | Test transport only; product binding pending. |
| CsrFreeCaptureBuffer | Original GetNextVDMCommand; release captured storage at original cleanup sites. | Test transport only; product binding pending. |
| CsrClientCallServer | Original GetNextVDMCommand; dispatch captured request and preserve transport/service result distinction. | Focused test routes to actual original server; authenticated product transport pending. |

The test implementations in `tests/broker/original_server_lifecycle.c` are
not product providers. No command-selection, waiting/retry or completion policy
may move here from the original client/server bodies.

The same four declarations also serve the restored original ExitVDM,
SetVDMCurrentDirectories, GetVDMCurrentDirectories, CmdBatNotification and
RegisterWowExec cohort. Their service opcodes dispatch to the corresponding
original BaseSrv routines; no per-opcode policy implementation is added here.
BaseUpdateVDMEntry is now another original client caller of that same dispatch
binding; its signature is declared for the forthcoming launcher composition.

T412 S2 adds `basesrv/include/base_classifier.h` and
`basesrv/source/base_classifier_path.c` for original BaseClient classification.
The header retains original nturtl/ntmmapi structures and status constants,
provides a private GetBinaryTypeW symbol and selects native NtOpenFile instead
of the DOS-path DEM adapter. This is a run16 client dependency, not broker
classification policy. The only new executable path binding calls native
RtlDosPathNameToNtPathName_U with its optional relative result omitted and
clears the original relative record, selecting the original full-path branch.
Native relative output writes 16 bytes and can return a nonempty name without
a directory handle, unlike original NT4's 12-byte borrowed-directory contract.
No parser, handle table, queue or current-directory policy is added. Original
curdir.c depends on native PEB/current-directory ownership and is not imported.
The original body therefore composes at the finite-binding recovery rung;
no mirror-body intrusion or new classifier algorithm is required. Focused
positive/negative proof and remaining limits are recorded in the
[S2 evidence](../../docs/etc/evidence/m0-t412-s2-original-lifecycle.md).

Original BaseCheckVDM now consumes the existing capture/dispatch declarations.
Its private PPEB spelling binds only the existing support carrier; standard
streams and startup flags are public-API fields, not host PEB layout guesses.
`basesrv/source/base_client_process.c` implements only CONSOLE_HANDLE's modern
capability query via GetConsoleMode, preserving LastError. The original
conroute.h low-two-bit pseudo-handle test cannot identify modern Console
handles. Command/stream routing policy stays in the original function; no
native handle enters a broker record. The header maps historical swprintf to
the CRT's _swprintf signature, avoiding ISO's extra count parameter. The
original hotkey-to-reserved-string branch is tested with an exact result.

`basesrv/include/base_server.h` is the finite native declaration binding for
the unchanged srvvdm.c owner. Its declaration shapes were audited through the
S1 compile probe and now belong to this package, not a tests/ include. They
cover event/token/loader/duplication APIs, source-shaped CSR record types and
the request-thread accessor. OpenNtBaseServerRequestThread must return the
currently authenticated bound request context. `base_request.c` now supplies
the borrowed thread-local binding used by the original lifecycle test; the
fixture still supplies its process records, while actual product dispatch and
registration are pending. Original ntcsrsrv.h reads NT4's private TEB
CsrClientThread field, which cannot be used as a modern standalone TEB slot.
The finite TLS substitution preserves lookup shape without a CSR runtime.
Trusted dispatch must authenticate first, bind its local record, and restore
the previous binding on every exit before releasing the record. The binder
does not authenticate, register, retain, or validate an arbitrary peer record.
`OpenNtBaseRetainRegisteredProcess` reuses the same CsrLockProcessByClientId /
CsrUnlockProcess binding to check the trusted PID/generation and retain a
non-inheritable same-access process handle before releasing the registry lock.
This is a finite lifetime binding for external helper I/O, not a second
registry or an authorization endpoint. A retained handle survives unregister;
the caller must repeat registration-generation validation before accepting
results. The original lifecycle suite verifies wrong/old generation rejection,
zero retained registry pins and missing registration after removal despite a
still-live retained handle. OpenNtBaseServiceRetainPeer now selects this binding
under the service lock after matching the authenticated connection PID and
generation. The formal First/Disconnect peer checks use it and immediately
close their temporary same-access handle. Its returned handle never extends
the RPC context lifetime. Verify-BasesrvProduct checks the selected service
and registry providers in the product map and passes live RPC generation,
duplicate, reconnect and process-exit rundown cases. Console-query result
revalidation/consumption and worker-candidate selection remain pending.

`OpenNtBaseRetainRegisteredProcess` reuses the same CsrLockProcessByClientId /
CsrUnlockProcess binding to check the trusted PID/generation and retain a
non-inheritable same-access process handle before releasing the registry lock.
This is a finite lifetime binding for external helper I/O, not a second
registry or an authorization endpoint. A retained handle survives unregister;
the caller must repeat registration-generation validation before accepting
results. The original lifecycle suite verifies wrong/old generation rejection,
zero retained registry pins and missing registration after removal despite a
still-live retained handle. OpenNtBaseServiceRetainPeer now selects this binding
under the service lock after matching the authenticated connection PID and
generation. The formal First/Disconnect peer checks use it and immediately
close their temporary same-access handle. Its returned handle never extends
the RPC context lifetime. Verify-BasesrvProduct checks the selected service
and registry providers in the product map and passes live RPC generation,
duplicate, reconnect and process-exit rundown cases. Console-query result
revalidation/consumption and worker-candidate selection remain pending.

The Check reply in `base_command.c` is a forty-byte versioned response:
original NTSTATUS plus iTask and VDMState, correlated to request/generation.
BaseCheckVDM in the original client checks NT_SUCCESS before consuming iTask;
failure replies therefore omit undefined output scalars and publish only the
status. Successful application changes only those two scalars and status,
leaving locally bound wait resources and all input buffers untouched. This is
a finite replacement of CSR copying, not a BaseSrv algorithm. The original
lifecycle fixture now roundtrips every reached Check reply and checks all
truncated lengths, wrong request/generation and unchanged resource fields.
Actual RPC command/resource composition remains unfinished. Update's reply
uses only the thirty-two-byte status envelope: original BaseUpdateVDMEntry
consumes no output scalar, but does consume WaitObjectForParent after success.
That handle must be separately bound locally. The reply codec never reads or
changes it. The original lifecycle fixture roundtrips reached Update replies,
preserves every native field, and rejects truncated and miscorrelated replies.

The Check reply in `base_command.c` is a forty-byte versioned response:
original NTSTATUS plus iTask and VDMState, correlated to request/generation.
BaseCheckVDM in the original client checks NT_SUCCESS before consuming iTask;
failure replies therefore omit undefined output scalars and publish only the
status. Successful application changes only those two scalars and status,
leaving locally bound wait resources and all input buffers untouched. This is
a finite replacement of CSR copying, not a BaseSrv algorithm. The original
lifecycle fixture now roundtrips every reached Check reply and checks all
truncated lengths, wrong request/generation and unchanged resource fields.
Actual RPC command/resource composition remains unfinished. Update's reply
uses only the thirty-two-byte status envelope: original BaseUpdateVDMEntry
consumes no output scalar, but does consume WaitObjectForParent after success.
That handle must be separately bound locally. The reply codec never reads or
changes it. The original lifecycle fixture roundtrips reached Update replies,
preserves every native field, and rejects truncated and miscorrelated replies.

Formal service connections now own the existing broker_vdm_receipts container,
initialized with their registered generation. Typed AttachFile/AttachPipe RPC
entries authenticate the process/context and call the same stream-only binder;
wait roles are refused. Revoke is repeat-safe, and disconnect/rundown drains
remaining references before freeing connection storage. No new receipt table
or original resource policy is introduced. Product RPC tests transfer a real
file and anonymous pipe, reject wrong generation/wait role, and prove revoke
and disconnect close the broker's last pipe writer. Command/worker binding of
these receipts and ambiguous-reply rollback remain unimplemented.

`base_stream.h/.c` implement the standard-stream slice of the existing native
resource callback. A trusted synchronous scope pins source/target processes and
the source receipt generation. Source-shaped local carrier values are receipt
IDs, never passed to NtDuplicateObject as native handles. The callback resolves
the source stream, delegates acknowledged target delivery, or delegates target
revocation for CLOSE_SOURCE. It admits only the exact original stream flags,
keeps output unchanged on failure and does not rewrite the ordered original
copy/alias/close loops. There is no fallback native remote duplication. Other
resource operations are explicitly unsupported by this slice. The original
lifecycle test selects streams.obj from the formal binding archive and drives
original BaseSrvDupStandardHandles/CloseStandardHandles: one delivery for an
stdout/stderr alias, distinct destination receipt, actual pipe byte transfer,
two repeat-safe closes, wrong source generation and target rejection. A second
delivery failure also passes through this translator and the existing journal:
source-shaped mixed fields remain, target rollback succeeds and the retained
sender stream still transfers pipe data. Target
delivery in this test is local; product worker RPC/journal integration remains
open. A separate formal-broker test now calls the same original stream loops
and adapter with the real basesrv process as receiver: one typed pipe RPC
delivery for a shared stdout/stderr, two original revoke calls and final pipe
closure. The fixture obtains the receiver handle from its driver's exact
owned process PID; this is not product worker registration or task execution.
This finite missing CSR transport is newly authored, not original policy.

`base_wait.h/.c` provide the separate original wait-pair resource scope. The
original BaseSrvCreatePairWaitHandles still creates its notification event and
owns failure cleanup; the callback retains the local event identity, delegates
authenticated/journalled target delivery and returns only the target receipt
carrier. Original BaseSrvClosePairWaitHandles closes the native server event,
revokes the recipient receipt and zeros its record. Native close is confined
to that scope's local event. Later close scopes must initialize this identity
from the original record; the scope is not another durable event registry.
The original lifecycle suite verifies shared signal state, non-inheritance,
original close and synchronous local close after failed target delivery.
The formal binding archive supplies waits.obj. Target reception is local in
this test; product event RPC, journal/unknown-reply and other native resource
operations remain outside this slice and are not claimed complete.

`base_wait.h/.c` provide the separate original wait-pair resource scope. The
original BaseSrvCreatePairWaitHandles still creates its notification event and
owns failure cleanup; the callback retains the local event identity, delegates
authenticated/journalled target delivery and returns only the target receipt
carrier. Original BaseSrvClosePairWaitHandles closes the native server event,
revokes the recipient receipt and zeros its record. Native close is confined
to that scope's local event. Later close scopes must initialize this identity
from the original record; the scope is not another durable event registry.
The original lifecycle suite verifies shared signal state, non-inheritance,
original close and synchronous local close after failed target delivery.
The formal binding archive supplies waits.obj. Target reception is local in
this test; product event RPC, journal/unknown-reply and other native resource
operations remain outside this slice and are not claimed complete.

`base_process.h`/`base_process.c` supply scoped registered-process lookup with
the original CsrLockProcessByClientId/CsrUnlockProcess shape. Original CSR
process.c depends on CsrRootProcess and CSR reference/deletion machinery;
that full runtime is excluded. The finite binding owns duplicated process
references and borrows caller-owned CSR_PROCESS storage until removal. The
registry retains the incoming process rights with DUPLICATE_SAME_ACCESS rather
than acquiring broader rights. Its lock remains held across original service use; pins reject reentrant
removal, duplicate registration fails, sequence allocation never wraps, and
nonempty destruction fails. Registration derives PID from the process object,
not wire fields. Authentication and worker-role authorization must precede it.
No system enumeration, task-selection state or automatic process termination
is introduced. All service calls bind the same registry; unbind/destruction
requires quiescent users and balanced unlocks. This is a registered modern
mechanic, not an imported CSR implementation or an already connected broker.
The source/lifetime proof and pending process-race gates are in the S3 entry
evidence. Product resource transfer must preserve the admitted attachment and
receipt protocol; this local registration API is not remote duplication policy.

`base_resource.h`/`base_resource.c` bind original BaseSrv NtDuplicateObject and
NtClose calls without editing srvvdm.c. A trusted per-dispatch binding routes
them synchronously to the resource transport; missing operations fail instead
of falling through. Without a binding only verified current-process native
duplication is allowed; cross-process duplication returns NOT_SUPPORTED.
Native local close remains available for original tokens/events. The resource
TU alone disables the declaration remap to reach native APIs without recursion.
The callback context is private local state, never wire data; it must outlive
the synchronous call, and dispatch restores the previous binding on exit.
The original failure branch is tested with a rejected wait-event delivery;
actual authenticated receipt callbacks remain to be connected before product
dispatch. This is not permission to forward arbitrary native handle numbers.

`base_dispatch.h`/`base_dispatch.c` supply the finite local dispatch entry for
the eleven recovered Base VDM APIs. Original srvinit.c's
BaseServerApiDispatchTable supplies the API-to-handler mapping; composing its
full initializer would require the excluded CSR server and unrelated Base
services. This adapter binds only that unavailable routing boundary, with exact
native payload sizes from basemsg.h. Original srvvdm.c retains every service
body. A missing local caller, wrong size or other API fails before dispatch;
ClientId is taken from the bound trusted thread, never the message. The caller
must already authenticate, validate copied variable buffers/resources and bind
the live registered context. This API neither decodes untrusted wire bytes nor
validates arbitrary native pointers. Broker composition must serialize source
service access and maintain context lifetime. Formal lifecycle tests now use
this entry instead of a fixture-owned handler switch; real RPC decoding remains
pending. This finite source-shaped adapter replaces routing only, not CSR.
OpenNtBaseVdmOperation/OpenNtBaseDispatchOperation explicitly translate between
the eleven standalone operation IDs and original Base API numbers. No other
Base API is exposed; neither the original CSR numeric namespace nor native
message layout becomes the wire protocol. Original BaseClient lifecycle calls in the
fixture now pass through this mapping. Authentication and full message-body
validation remain prerequisites of product endpoint use.

`base_service.h`/`base_service.c` compose a process-local original BaseSrv
instance and opaque registered connections for the formal basesrv entry. The
unavailable CSR connection shell is replaced with existing process-registry
and request-thread bindings; original first-VDM policy stays in srvvdm.c.
Registration retains the authenticated process, rejects duplicate PIDs, assigns
the registry generation and releases it on disconnect. A service lock serializes
dispatch. This current entry admits no commands/resources, so rundown only
removes registration; original task cleanup must be wired before command admission.
RPC guarantees connection lifetime across calls and invokes rundown after loss.
Rundown invariant failure is fail-fast, not silently leaked or reported success;
that exceptional path remains untested. Only one service instance per process
is supported, consistent with original globals. Product connection tests use
this binding, not the earlier resource fixture bridge.

`base_command.h`/`base_command.c` compose CheckVDM's copied request from the
versioned envelope, values, startup and eight buffers. The decoder validates
the entire message into local temporary storage before publishing borrowed
payload/startup pointers or the request ID. Resource fields remain locally
bound and untouched; authentication and attachment resolution are required
outside this codec. Original client lifecycle tests use this same production
composition. GetNext now composes request/reply envelopes too: input carries
only source-defined state/exit code, PIF task when relevant, capacities and
startup presence, never uninitialized output fields. Prepare allocates cleared
payload and complete reply storage before source consumption. Finish allocates
nothing; apply validates generation/request ID, scalar widths, startup and all
buffer capacities before publication. Original client code retains its error,
wait and retry policy. Native resources remain separately bound. This is not
yet a complete RPC request/resource transaction. Check request encoding omits
output-only iTask and fixes VDMState to the original FALSE input. Update request
encoding preserves task/binary/entry and includes creation state only for undo;
other entries never read that unused scratch field. Its decoder checks the three
original entry indexes and leaves native resource fields untouched. Original
BaseUpdateVDMEntry calls use this composed request in lifecycle verification.

`base_values.h`/`base_values.c` bind original basemsg.h's CheckVDM,
UpdateVDMEntry and GetNextVDMCommand numeric fields. Exact-size copied fragments
replace unavailable CSR address-space transport, not original service policy.
All USHORT/WORD/BOOLEAN width checks precede writes; resources, pointers and
startup storage remain untouched. Formal owner archives select this binding;
the original lifecycle suite checks distinct field values, full-message
preservation, truncation and each narrowing overflow. Product command transport
must still combine these fragments with authenticated resources and buffers.

`base_startup.h`/`base_startup.c` bind only original BaseCheckVDM's nine numeric
STARTUPINFO fields to the broker's fixed-width startup fragment. Original
vdm.c selects these fields explicitly and transmits strings and standard
streams separately; copying the entire native structure would carry pointers
and unselected fields. Decode reconstructs cb locally and leaves all native
pointer/handle members zero. Original GetNextVDMCommand later rebinds the
desktop/title/reserved strings; resources follow their independent protocol.
Presence distinguishes absent StartupInfo; invalid presence/show-width or
nonempty absent form fails without modifying output. The original lifecycle
fixture passes with this binding around real BaseCheckVDM requests. Full RPC
integration remains open; this is not a general STARTUPINFO replacement.

`base_payload.h`/`base_payload.c` bind the eight original BASE_CHECKVDM_MSG
variable fields to the broker copied-buffer fragment. Decoding validates the
complete spans, requires actual input bytes rather than advertised output
capacity, and checks the four USHORT lengths before changing native fields.
It requires the command and NUL termination of present nonempty text fields.
Environment termination follows BaseCreateVDMEnvironment: one NUL is accepted
for empty input, otherwise a double NUL. A present zero-byte input is rejected;
it must not become a pointer alias to unrelated payload bytes. These checks
do not parse shell syntax, alter text/encoding or select programs.
Only those fields change; returned pointers borrow the mutable payload until
the original synchronous service copies it. This CheckVDM decoder adds no
allocator, command selection or resource policy. Full-operation validation,
including semantic string validation and authentication, is still required
before dispatch. The original lifecycle fixture now sends its real BaseCheckVDM
captures through this binding and restores original capture pointers before
cleanup. This is a native marshalling component, not the full RPC decoder.
The GetNext response application binds to saved native request pointers and
capacities. It validates all eight spans, presence and original length widths
before any destination write, then copies bytes and publishes returned lengths.
Copied bytes are bounded by saved capacity, not the returned required length.
Payload/destinations must not overlap and must remain owned for the call.
GetNext preparation validates capacity-only spans and reserves one zeroed
allocation for all native output buffers and the complete reply before source
dispatch. Integer overflow and allocation failure occur before command
consumption. Reply construction only updates length metadata in that allocation;
the initialized full-capacity bytes preserve partial writes and PIF terminators
without serializing uninitialized memory. The per-call context is private and
must be fresh, with borrowed pointers discarded/restored before release.
Repeat release is harmless. This replaces unavailable CSR shared-capture
storage, not source status/wait/retry/consumption policy. The original lifecycle
fixture uses the complete local buffer roundtrip. RPC integration is pending.

The header supplies no CSR runtime, command policy or success stubs. Keeping
original CSR declaration records does not admit their historical transport.

`base_interactive.h`/`base_interactive.c` bind the single private predicate
below original winsrv `_UserTestTokenForInteractive`. The original kernel
TestForInteractiveUser compares against grpwinstaList's first windowstation
user, an unavailable global NT4 service contract. The admitted standalone
scope is captured from the broker's real process token on a visible process
windowstation; an immutable caller-owned scope is bound per dispatch thread
and restored/unbound afterward. Only equal AuthenticationId succeeds; absent,
invalid and mismatched scope fails closed. No wire LUID may initialize it.
Original token inspection and BaseSrv SYSTEM impersonation decisions stay in
their original owners. App must capture/bind this scope and install the linked
original helper before serving shared-WOW requests; product wiring is pending.
