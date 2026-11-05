# T477 S1 frontend-base extraction design

## Status and objective

Owner admitted and authorized this design. Both P deliveries are now complete;
[S1 evidence](../evidence/m0-t477-s1-frontend-base.md) records implementation,
verification and publication. [Current](../../states/CURRENT.md) is the sole
execution-state authority. This document retains the approved design contract.

Extract NTCON's reusable frontend connection and service mechanics into
common/frontend-base and reconnect the existing production NTCON. Preserve
the current copied wire protocol and all observable Console, Window, input,
handoff and failure behavior. NTVDM integration belongs to S3 and will call
frontend-base directly; worker-base never depends on frontend-base.

## Source audit and reuse decisions

Reviewed baseline: admission commit d1380d1a9 with ownership correction b4f564630.
The relevant implementation is project-owned composition; this extraction
imports no source and does not modify an original mirror.

| Existing source | Finding | Disposition |
| --- | --- | --- |
| src/ntcon-exe/console_channel.c | Pipe pair creation, authenticated attachment, exact-length OVERLAPPED transport, worker death/stop waits, request/reply loop and join are mixed with native surface/video transactions. | Extract transport/channel ownership into frontend-base/channel; retain native dispatch and publication adapter in NTCON. |
| src/ntcon-exe/session_service.c | One active channel, NTSRV request/disconnect acknowledgements and broker-loss handling are mixed with Console membership checks, restoration and parking. | Extract reusable service pump into frontend-base/service; keep NTCON-specific service bindings and presentation actions local. |
| src/ntcon-exe/frontend_session.c | Owns visible/hidden logical Console surfaces, Window thread, input receipt bookkeeping, keyboard/mouse routing, title and presentation barriers. | Retain in NTCON. Its process-wide Console control handler is a native NTCON binding, not shared frontend-instance state. |
| src/ntcon-exe/console_frontend.c and console_video.c | Dispatch validates generations/sequences and executes Console operations; frame transaction state is coupled to presentation. | Retain existing dispatcher and video implementation during S1. Call through an explicit NTCON adapter; do not duplicate protocol validation in the new base. |
| src/common/console/client.c | Existing worker-side I/O client. | Reuse unchanged; do not replace or move it merely to match frontend naming. |
| src/lib-win32/pipe/pipe_pair.c and pipe_transfer.c | Existing generic pipe and cancellation/drain primitives. | Reuse, without a second transport library. |
| src/common/base_srv/base_rpc_client_core.c | Process-level client plus frontend queries without an explicit frontend-instance argument. | Retain S1 production binding through narrow control callbacks. S2 owns instance-addressed protocol and service changes; no fake multi-frontend support in S1. |

These are extractions of available current owners, not newly authored
replacements for OpenNT algorithms. Before moving each body, preserve its
ordering, error returns, resource contract and relevant existing tests.

## Ownership and dependency contract

Production selection:

    NTCON -> common/frontend-base -> common/protocol + lib-win32/pipe
    NTCON control adapter -> existing common/base_srv
    NTCON presentation adapter -> existing NTCON session/Console/Window

Future S3 selection:

    NTVDM -> common/frontend-base
    NTVDM -> worker-base
    worker-base -X-> common/frontend-base

The common library has no NTCON private headers, frontend_session,
frontend_console, frontend_video, native Console surface, window controller,
VM registry or guest pointer. It creates no Console, Window, worker or task.
NTSRV alone chooses association, release and retirement. Callback injection is
a process-local typed C composition boundary, never a callback serialized in
RPC and never a generic plugin framework.

All stateful entrypoints take an explicit instance. Each service instance
owns at most one channel. Two isolated unit instances must coexist without
shared stop events, active-owner pointers or buffers. This proves local
instance isolation only; actual shared-process NTSRV registration remains S2.

## Proposed interface shape

The exact names may be adjusted during implementation without changing this
contract. Keep two opaque types, frontend_base_channel and frontend_base_service.

Channel operations:

- Prepare a channel from an authenticated worker reference and caller-owned
  adapter context. Consume that worker reference on every outcome, matching
  frontend_io_channel_start_request. Return an owned channel only on success.
- Bind the host adapter before exporting its pipe. Attach through the supplied
  control binding, propagate the returned generation to the adapter, then
  start the serving thread. Preserve current ordering and failure rollback.
- Obtain the borrowed completion-thread handle for service waits.
- Stop/join/dispose the channel. A failed join leaves the object and borrowed
  adapter state alive; elapsed time never authorizes freeing them.

The adapter receives a bounded copied request and a preallocated reply buffer.
It returns an explicit outcome: send reply, continue frame without reply, or
terminal transport failure. It also supplies input readiness/rearm and
connection preparation/final cleanup. Required callbacks are validated at
creation; callback context and borrowed readiness handles survive channel join.

The current snapshot restrictions, frame chunk validation, publication
rollback, present barrier and terminal-after-commit rule stay in the NTCON
dispatch adapter. In particular, non-final video chunks do not acquire replies,
and failed non-final chunks close the stream rather than queue an extra reply.
No new common generation/sequence counter competes with the existing dispatcher.

Service operations:

- Start with borrowed notification/state handles, explicit control and host
  operation bindings, and one instance context.
- Query and execute NTSRV-directed attach/disconnect/retire actions using the
  existing request and action meanings.
- Stop/join service processing, then join its channel; host resource destruction
  follows successful joins. Caller retains final presentation ownership.

Control bindings cover existing request query, attachment, disconnected
acknowledgement, retirement and exit-request operations. NTCON-specific
Console join probes and park/return operations remain host callbacks using
existing code. The base must not interpret PID ancestry or invent a lease.
S2 can replace the control context with instance-addressed bindings without
changing the channel's presentation contract.

Do not mechanically turn every existing frontend_session method into a public
callback. Use one operation-dispatch boundary and the small lifecycle/readiness
set needed by the actual common pump.

## Ordering, locking and error contracts

1. Query an authenticated NTSRV request; do not choose a worker locally.
2. Prepare endpoint and adapter, bind before attachment, export handles through
   the existing authenticated path, then serve requests.
3. Borrow packet buffers only for the synchronous dispatch. No adapter retains
   request pointers or waits for a future guest callback using those pointers.
4. NTCON retains its current I/O/presentation locks and transaction ownership.
   Base transport performs no blocking pipe wait while holding those locks.
   Preserve present-barrier waiting after dispatch releases the I/O lock.
5. On NTSRV release: stop admission, cancel/drain outstanding OVERLAPPED I/O,
   join the channel and detach borrowed frame state before acknowledging
   FrontendIoDisconnected. Only then can a new channel attach.
6. Pipe EOF is an I/O failure, not permission to pick a replacement worker,
   replay requests, report target success or restore the Console early.
7. Healthy-service fault handling preserves the existing one-shot participant
   exit request and service-directed cleanup; confirmed broker death permits
   the existing local cleanup path.
8. Native Console return/park must complete before its corresponding NTSRV
   acknowledgement. A failed return is never reported as restored.
9. Resource creation, activation, attachment and thread-start failures have
   explicit reverse cleanup; pipe ownership after thread start stays with
   that thread. No double-close of a stale pipe handle.
10. Preserve input readiness reset-before-peek ordering, tagged input returns,
    pending publication abort and final output ordering. No new runtime timeout,
    retry, scheduler or polling thread is introduced.

## Sequential P deliveries

| Part | Production outcome | Verification |
| --- | --- | --- |
| P1 | Extract channel transport/ownership into common/frontend-base/channel and reconnect NTCON through its local dispatch adapter. Remove the displaced transport loop; update real build graphs. | New common channel tests plus existing Console/video/channel-lifetime/client tests, actual NTCON regressions, coherent package build/publication and reviewed commit. |
| P2 | Extract reusable service pump into common/frontend-base/service; keep Console restore/membership and rendering in NTCON. Remove displaced pump, finalize ownership docs and S1 closure. | Service action/ordering/isolation tests, actual NTSRV/NTCON handoff and failure tests, full selected regression, dependency audit, coherent package build/publication and reviewed commit. |

Each P must leave NTCON using the extracted production code. No unused library,
parallel implementation or pure rename qualifies. Tests needed for a P land
with that P; P2 is not a deferred repair stage for P1 behavior.

## Verification matrix

New tests belong under test/common/frontend-base and link the production
archive without NTCON/worker-base. Proposed runner:
test/common/frontend-base/verify-frontend-base.ps1.

| Cases | Assertions |
| --- | --- |
| Partial transfers and fragmented requests | Exact-length framing, bounded allocation, intact payload and reply; zero-byte/broken transfer fails. |
| Cancel during pending read/write; worker exit | Kernel I/O drained before event/buffer/pipe release; exactly-once cleanup and deterministic error. |
| Failure at prepare/activate/attach/thread-start | Worker reference consumed once; no published live channel or leaked adapter binding after rollback. |
| Adapter reply/continue/fatal outcomes | No unsolicited reply for intermediate frame chunks; final result ordering unchanged; terminal failure closes endpoint. |
| Two simultaneous base instances | Independent buffers, readiness, stop and channel ownership; stopping A does not affect B. |
| Service release and reconnect | Channel joins before disconnect acknowledgement; no second channel while one remains; service alone grants the next attachment. |
| Broker loss and healthy-service I/O fault | No reconnect/replay; no false Console-restored acknowledgement; preserve one-shot exit request. |
| Failed join/restore and disposal | Borrowed state retained until safe; failed cleanup reported, not turned into success. |

Reuse test/integration/verify-frontend-transport-contracts.ps1 for its existing
Console/video/pointer/lifetime/client cases and applicable expanded backend
checks; these native fixtures are not real DOS/RPC acceptance. Also exercise
real DOS/native handoff, final screen/cursor/title, input return, Console/Window
transitions, normal/abnormal retirement, GUI and redirection using the selected
product regression runners. Freeze exact runner arguments and package identities
in each P's evidence after auditing their current prerequisites.

Build the existing native x64 frontend plus x86 common fixtures/worker
compatibility under their selected MSVC /MT islands. Use actual production
build graphs, not copied implementation in tests. All builds/tests run as owned
background processes; product P delivery follows the active execution gates.
Documentation-only design validation does not claim a runtime test pass.

## S1 exit and deferred scope

S1 closes only when NTCON actually uses both extracted mechanisms, old duplicates
are removed, common is independent of NTCON and worker-base, required tests and
publication pass, and delivery evidence is recorded. S2 owns multi-frontend
NTSRV identity; S3 owns NTVDM VM composition; S4 owns monitor integration.
The guest driver is a separate queued task.
