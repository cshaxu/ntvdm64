# S14 P161: synchronous command and BASE boundaries

Owner approved all architecture-compliant simplifications, specifically removal
of the artificial COMMAND event/threadpool handoff. This completes disposition
of C0156 and C0619; C0621 was delivered in P159.

## Ownership and history

C0156: C calls the B-owned BASE entry. B validates/selects all eight original
request shapes, maintains the local wait/observation semantics and returns the
result. D owns the authenticated connection, wire/receipt translation and error
state. Its immutable nine-operation transport table is registered at connection
setup and cleared during joined-caller teardown. The nine synchronous callbacks
are explicit approved dependency inversion, not nine relocated implementations.
P152 reply failure was a fixture missing package identity; the subsequent
reconnect failure used the core entry instead of execution registration. See
[saved BASE evidence](s14-base-dispatch.md). No new transport or callback queue.

C0619: history before P150 (13c699a47) used synchronous COMMAND launch. P150
added the request event, threadpool dispatch and completion event for layering,
not for a separate asynchronous responsibility. B now owns the typed borrowed
request and synchronous start interface. F binds its existing product launch
operation before guest execution and unbinds after callers stop. The callback
returns the existing error/created/process result in the same stack request.
Preparation, standard streams and error propagation are unchanged. Deleted:
MVDM_COMMAND_CHILD, command_child handoff pointer, per-request completion event,
post_command_child, threadpool forwarding and its extra wait. Actual exit and
WOW cross-thread protocols remain. The existing one-worker lifetime is retained;
registration/unregistration must not race active callers.

C0621 remains the P159 B-owned BOP ingress and synchronous F submission callback;
no asynchronous success-before-admission or second scheduler was introduced.

## Counts and evidence

Known unapproved direct outward symbol dependencies: B 0, C 0. Final pending
chain dispositions: C0156/C0619 2 -> 0; C0621 stays resolved. Reviewed outward
runtime callbacks across these three chains: B-to-D 9, B-to-F 2, C-to-outer 0.
Callbacks are not reported as absent. Named DLL imports retain their approved
architecture treatment. Non-error mirror delta: 0 points, 0 lines; no original
guest source or binary changed or rebuilt.

Final package fe6f7652-80de-422a-9c5c-5f95ae3e0208. Cached x86, x64 and reference
product builds passed. The test link initially hit a transient executable lock;
retry passed. Both release destinations contain the same eleven images.
Evidence directory: build/t483-s14-callback-closure.

- BASE dispatch: 98 assertions, all eight same-thread routes, malformed lengths,
  disconnected transport and unbind. Transport is mocked; not eight real RPCs.
- Request launch: 71 assertions, per-request streams/handles and distinct exits.
- VM frontend/lifecycle: 15 cases including concurrent synchronous COMMAND calls,
  blocked first callback, independent second callback and rejection after unbind.
- Actual package COMMAND: modern EDIT screen/Ctrl+Q, CMD echo, DOS MEM and launcher
  completion passed (command-return.log).
- Actual Win3.1: two concurrent DOS VMs, distinct identities/generations, 320
  callback identity/stack/register/flags checks; native 54:08 AH=0/AH=1 and normal
  Windows exit passed. Guest input and 54:0A are not covered by this probe.
  Original installation bytes verified unchanged; private test copy cleaned.

This closes these dependency dispositions, not the queued repository-wide
redundant queue/state-machine audit. No Full regression claimed for this batch.
