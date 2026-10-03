# Common cross-component mechanisms

T424 S7 owns this bounded static-library family. Both admitted protocol
families and their selected shared mechanisms are production-wired; the
S7 evidence records the runtime/publication gate. NTSRV's service-private
source separation belongs to S8, not to this neutral library.

Common is the carrier of both admitted protocol families and their applicable
shared client/transport mechanisms: NTSRV control uses RPC; direct
NTCON-worker I/O uses named pipes. Their underlying transport differs, not
their common source ownership. They do not depend on or relay through each
other. Endpoint authentication policy, execution, rendering and lifecycle
authority stay with their executable owner. Worker-base may depend on common;
common cannot depend on worker-base or executable-private implementations.

`protocol/` owns copied cross-component declarations, shared version identity,
service IDL/ACF and frontend I/O formats. Generated RPC outputs stay in build/.
It has no implementation or local client instance state.

`codec/native_launch` owns the existing bounded packet pack/unpack algorithm,
including lengths and borrowed string views. It does not create processes,
authenticate clients or duplicate capabilities. Those remain endpoint-owned.
common-codec.lib is its single selected provider; private archives do not
embed a second copy. Local target materialization is separately declared in
run16-exe/native_launch.h. I/O-client instance state is declared in
common/console/client.h, not a wire protocol header.

`console/client` owns the existing shared NTCON named-pipe protocol client:
ordered requests/replies, frame chunks, title publication, input decoding and
atomic key-return encoding. It retains borrowed pipe/peer/cancel handles and
owns only its local operation event. Callers keep their locks across exchanges
and frame transactions. Activation is one attempt; backend retries, binding,
guest execution, renderer state and lifecycle decisions remain caller-owned.
It depends only on common protocol/pipe mechanics and Win32, not worker-base.
Common-console.lib is the sole selected provider, shared by NTVDM/NTVWM and
their fixtures; private worker archives do not embed another copy.

`rpc/local_binding` owns the identical project-added BaseClient/NTMON
ncalrpc binding/auth-parameter/rollback mechanism. It takes explicit output
state, retains no global instance, returns precise RPC errors and releases
all partial bindings/strings on failure. A successful binding is caller-owned.
Endpoint selection, logon scope, application/version checks, peer validation
and broker-loss policy stay with the caller. common-rpc.lib is its sole provider.

`rpc/native_command` owns the project-added GetNextNativeCommand,
NativeStartupResult, CompleteWorkerChannel, SubmitNativeRequest and
FinishNativeRequest RPC client exchanges extracted
from the BaseClient adapter. An explicit connection view borrows the binding,
authenticated context, process capability and generation for one call. Its
owner prevents disconnect during calls. The common client has no process-global
state, record registry, command callback or broker-death policy. Failed copied
delivery closes partial output attachments and clears outputs; successful
attachments belong to the caller. Startup/completion input handles are borrowed.
RPC status/exception codes are returned unchanged, including failed completion;
the worker remains responsible for its failure decision. Original DOS/WOW
BaseClient state and the existing public facade remain in their original owner.
Submit borrows its frontend/payload and gives successful target/receipt
attachments to the caller; ordinary or exceptional failure releases partial
attachments. Finish retains the original scalar-output/status contract and
rejects a target-completed value outside 0/1. Neither call owns a target wait.

`rpc/frontend_control` owns the project-added acquire/start, usage and
workerless-retirement request, state-event acquisition, return/restored,
startup-result, restoration wait and frontend-retirement RPC exchanges. It
uses the same borrowed `rpc/connection.h` view as the native-command client;
it does not own Console attachment, timers, service decisions or local event
wait sets. Successful returned handles are caller-owned; failed acquire/start
and event acquisition release partial attachments. Usage and retirement scalar
outputs remain unchanged on failure, as in the prior public facade. Status and
RPC exceptions are returned unchanged. NTSRV still decides retirement; a common
client call only conveys the authenticated request. The BaseClient facade
constructs a view and delegates, without a duplicate transport body.

`rpc/worker_control` carries the project-added worker frontend capability,
Console-context acquire/bind, native-backend registration, shutdown/state event
acquisition and Take/WaitFrontend exchanges. Both workers reach this provider
through the retained BaseClient facade; no duplicate exchange body remains.
It borrows the same explicit connection view and input handles. Successful
output attachments belong to the caller; failed calls release partial handles
and clear attachment outputs. Shutdown/state event APIs reject a successful
null event; capability APIs retain their previous provider-status contract.
The caller selects existing Take versus blocking Wait, owns its wait sets,
and decides failed-call handling. Registration authentication, root association,
shutdown policy and original DOS/WOW execution stay outside common.

`rpc/management` carries the existing project-added TaskSnapshot and
TerminateWorker client exchanges. Its explicit view borrows a binding and
process capability; these connectionless calls retain server authentication
and version checks without creating BaseClient registration. Partial failed
snapshot allocations are freed; successful copied records belong to the caller.
NTMON retains reconnect, selection, confirmation, UI and resource lifetime.
NTSRV remains snapshot and termination authority; common never enumerates tasks.

`console/members` shares allocation, bounded capacity growth, current-Console
sampling and release only. The caller explicitly supplies its initial/maximum
capacity and API-read ceiling. A failure exposes neither partial data nor an
allocation. Growth retries are immediate size negotiation, not timer polling.
Root identity reporting retains two reads and 4096 PIDs; frontend anchor/join
checks retain 4096, native quiescence/launcher resume retain 65536. Authentication,
member liveness, join acceptance, resume and quiescence remain local policies.
Single-member launcher cleanup remains its local stack-buffer query.

`transport/pipe_transfer` is project-added byte transport, not a third message
protocol. It is shared by the worker frontend client, frontend channel and
transport fixtures. The native command/startup control pipes and their reply
DTOs are removed; these operations use authenticated NTSRV RPC.

- Pipe, peer, cancellation event, operation event and buffer are borrowed.
  The caller serializes access and releases them after operations are drained.
- A stack operation is local state, never wire data. Begin reports immediate
  completion or pending; finish checks the actual transferred length. Cancel
  drains pending I/O before returning, including cancellation/completion races.
- Exact transfer loops over partial transfers with checked buffer capacity.
  Completion-first and peer-death-first are explicit policies. A frontend may
  use begin/finish/drain around its own input-ready wait without exporting that
  business state into common.
- No registry, scheduling, lifecycle decision, frame interpretation, rendering
  or reverse dependency on worker-base/EXE-private code belongs here.

Builds select neutral archives as explicit link inputs to their consumers,
never as objects embedded in EXE-private or worker-base archives. Common has
no dependency on worker-base or any executable-private implementation.
Objects, generated RPC files and fixtures remain under build/.
