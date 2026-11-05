# T477 S2 frontend instances design

## Admission and scope

Owner admits S2 and requests a design. Baseline: 6caaf5d78. This is a design
checkpoint, not implemented multi-frontend behavior. [Current](../../states/CURRENT.md)
is the sole active packet; [the sequence](m0-t477-host-frontend-plan.md) retains
S3 NTVDM composition and S4 management presentation. No source import is needed.

S2 makes NTSRV and its shared control clients capable of handling several
independent frontend instances hosted by one authenticated process. NTCON
becomes the single-instance production consumer. Real NTVDM VM enumeration,
VM existence checks and frontend-base adapters belong to S3; guest VxD and
asynchronous guest execution remain separate work.

## What is coupled today

A host control connection is an authenticated RPC participant record. It is
not the direct input/output pipe between a frontend and worker. A frontend
instance is one logical I/O destination with one active downstream worker.
These are currently conflated by storage and lookup, not by a C++ inheritance
relationship or by the wire combining screen output with RPC.

| Current owner | Evidence and implication |
| --- | --- |
| src/ntsrv-exe/opennt/include/service_internal.h, OPENNT_BASE_CONNECTION | Stores process identity together with frontend_capability, frontend_state_changed, frontend_closing, frontend_close_draining and frontend_io_route. There is one set per connection. |
| Same header, OPENNT_FRONTEND_ROUTE | root points to OPENNT_BASE_CONNECTION; frontend associations use process/root generations in connections and worker watches. |
| src/ntsrv-exe/opennt/source/frontend_registry.c | RegisterFrontendRoot rejects fVDM and a second frontend_capability, and authenticates the exact broker-created NTCON plus inherited unnamed capability. Worker I/O release waits for both endpoint acknowledgements. |
| src/common/protocol/service.idl | FrontendRequest, AttachFrontendRequest, FrontendIoDisconnected and retirement/state operations identify the caller by connection, process and generation; no separate frontend-instance selector. |
| src/common/base_srv/base_rpc_client_core.c | Uses the process client and calls these operations without an instance argument. |
| src/ntsrv-exe/opennt/source/lifecycle.c | RequestParticipantExit distinguishes frontend/worker using the participant record; process exit and frontend close cannot simply be reused unchanged for embedded instances. |
| service_core.c, worker_registry.c, native_commands.c, management.c | Rundown, associations and projected management keys also reference connection/root generations. Updating only RegisterFrontendRoot would leave these assumptions intact. |

Today NTCON has one process connection and one frontend; this works. A future
NTVDM has an upstream backend association plus VM-A and VM-B frontend instances.
Those two frontend instances need separate pending work, active channel and
retirement state. Creating extra fake process connections or selecting a
process-global current frontend would hide the ambiguity instead of resolving it.

## Proposed ownership

    NTSRV authenticated host connection (process identity and lifetime)
      + frontend instance A -> one active downstream worker route
      + frontend instance B -> one active downstream worker route

The same NTVDM host may separately remain a backend on its upstream chain.
These roles coexist; registering a frontend must not change its backend role.
S1 frontend-base instances execute in the host; the NTSRV instance records
are service-side authorization/lifecycle state, not copies of the I/O pump.

Introduce a service-private frontend-instance record owned by its authenticated
host connection. Move the existing per-frontend channel/request/release/closing
and notification state into it. Native Console lease/return/join state remains
a native-kind attachment, not mandatory VM fields. Host process identity,
RPC failure, execution participant state and host exit observation stay on the
connection. Startup reservations remain launch transactions until registration
binds their exact process/capability to a native instance.

Routes point to an instance, which points to its owning connection. Migrate
retained root references, pending route associations, worker watches and
management lookup together. Use the existing service lock, wait/notification
mechanisms and rundown ordering. Do not introduce a second task list, scheduler,
transport implementation, worker process or generic registry framework.

## Identity and authorization

- The broker issues an opaque frontend key (fixed-width ID plus generation).
  It is scoped to the authenticated broker lifetime; old RPC contexts fail
  after service restart. Never treat an ID alone as a capability.
- A frontend also has kind NativeConsole or PayloadVm. For PayloadVm, the
  host supplies a payload incarnation, local VM key and VM incarnation as
  opaque metadata. Raw guest pointers/handle values are not host pointers
  or globally trusted identities. Reusing a local VM handle creates a new key.
- Owner lookup validates RPC peer process/generation, key ownership and live
  registration. Cross-host control and stale callbacks are rejected without
  modifying another instance. Exhausted identity counters fail registration
  instead of wrapping into a live/stale key.
- Preserve exact NTCON startup admission and unnamed capability object checks.
  Embedded registration is a separate authorized path for an already admitted
  worker host; do not merely remove the existing fVDM rejection. NTSRV validates
  host admission and scoped metadata; actual VM existence is the host adapter's
  S3 responsibility. No arbitrary launcher can register a VM frontend.
- Reuse object-backed capability transfer for downstream association. A caller
  needs a broker-authorized association/capability, not a guessed frontend key,
  VM ID, PID, HWND or fabricated event. Never default an invalid selector to
  the host's upstream frontend or first registered instance.

Each instance owns notification/state/retirement objects and its own active
route. The broker keeps authenticated host handles alive while callbacks and
waiters borrow them. In-flight operations pin instance lifetime; close revokes
admission immediately but frees storage only after routes, callbacks and waiters
are drained. Re-resolve under the service lock after every wait, as today.

## Control RPC versus direct I/O

Control RPC means requests to NTSRV such as: which worker should this frontend
attach, has its pipe been attached, has release finished, and what retirement
action is authorized. Keyboard/mouse data and text/video frames continue over
the direct worker/frontend pipe using existing common console and lib-win32 pipe.

Add explicit instance selection to frontend-facing control operations and
return it from registration. An illustrative client call becomes:

    FrontendRequest(frontend_context, &request, &worker)

The context contains the key and borrowed process client reference; it is not
a new RPC connection, mutable global selector or second reconnect policy.
S1 frontend-base already accepts callback contexts: NTCON passes its registered
instance context through those callbacks. Future NTVDM supplies one per VM.

Audit the entire family: registration/lease, work query, attach, disconnected
ack, state event, retirement, fault/close, capability retention/request and
worker attachment delivery. Attach must bind both the instance and its pending
request, so a valid request from A cannot be attached through B. Keep worker
process death observation, and return sufficient instance identity/lifetime
notification that instance loss is not confused with host death. Reuse pipe EOF
and existing release signaling; add an instance-revocation event only where the
existing wait set cannot represent idle instance retirement safely.

Native Console startup, membership query and return acknowledgements remain
native-only operations. PayloadVm must not be required to impersonate an HWND
or claim Console restoration. Instance fault requests are distinct from
process-level RequestParticipantExit; the latter retains whole-process meaning.

Keep APP_VERSION 0.0.477 throughout T477. At the first wire-changing P, advance
protocol 65 to 66 consistently in version.h and IDL and build the complete peer
set. Reject old peers; do not support two parallel frontend state machines.
The ordinary NTCON command line and user-visible behavior stay unchanged.

## Handoff and teardown invariants

Reuse existing service_authorize_worker_io, release flags, dual endpoint
acknowledgements and admission ordering, parameterized by the instance.
One instance still has at most one active worker. A's release cannot clear B's
route or wake B as if it had a pending attach. New ownership begins only after
the existing old-worker and old-frontend release acknowledgements.

An explicitly selected VM frontend starts/joins its own downstream task chain.
The caller's execution identity and selected frontend identity are separate:
launching through A must not implicitly pause or release the host's upstream
chain. Use the existing launch/capability and BaseSrv/native task owners, with
explicit root selection; do not add a scheduler or synthesize task completion.
S2 tests this through authenticated host fixtures; actual guest cmdExec32
execution/pause policy is outside this design checkpoint and guest-driver scope.

| Event | Required disposition |
| --- | --- |
| Downstream worker releases/completes | Drain/release its instance route using existing ordering; sibling instances unchanged. |
| Frontend A unregisters or faults | Revoke A's admission, invoke existing chain close/drain rules for A, then retire A. Host and sibling B continue. |
| Host process actually exits | Revoke all its frontend instances and process/backend role; wake affected waiters and clean descendants using existing failure/rundown rules. Do not report successful task completion. |
| Payload incarnation ends while host lives | Retire only instances bound to that incarnation via explicit host notification; actual payload notification comes in S3. |
| Late attach/ack after close or key reuse | Reject stale identity/request; release only resources owned by the failed call. |
| Native Console return/park | Preserve existing native callbacks and acknowledgement order. |
| Embedded final close | Destroy/revoke that frontend instance after its downstream drain; never interpret it as permission to terminate its host worker. |

Use registered containment/association edges for nested cleanup, not PID ancestry
or Console membership as VM identity. Validate new edges against self/cyclic
association. Mark affected instances closing under lock before notifications;
do not synchronously hold the service lock while waiting for descendant exit.
Two frontend closures in either order must converge without deadlock.

## Proposed production P sequence

| Part | Complete bounded outcome | Required proof |
| --- | --- | --- |
| P1 | Separate service-private frontend state from the host connection and reconnect all existing native NTCON paths to one instance; retain current wire behavior. | Source/reference sweep, unchanged native startup/return/park, full existing regression; no residual frontend-as-process ownership assumptions in migrated paths. |
| P2 | Add authenticated instance registration and explicit control client/RPC context, scoped fault/retirement and host rundown. Switch NTCON to the explicit instance path; no alternate state machine. | Real NTSRV fixture with two registered instances in one host; request/attach isolation, cross-host and stale rejection, instance close versus host death; protocol negatives. |
| P3 | Complete explicit capability selection through existing launch/task-chain admission and nested association cleanup; close S2. | Independent downstream chains on the same host, upstream backend unaffected, siblings survive either close order, nested host loss and failure rollback; full package regression and dependency audit. |

Each production P includes builds, owning tests, coherent publication to this
worktree assets/release and O:/winnt/system32, hash checks and a local commit.
No push. This design checkpoint changes documents only and does not rebuild or
republish unchanged products. Implementation begins on owner direction.

## Acceptance and retained boundaries

Place private registry/lifecycle units under test/ntsrv-exe and shared client
units under test/common/base_srv. Add a real RPC/host fixture under test/integration
using the production service and shared frontend-base, with local presentation
callbacks only where guest/Console rendering is irrelevant. Do not count local
mock RPC callbacks as proof of authentication or lifecycle authority.

Required cases: duplicate registration; two live instances; wrong peer/key;
stale process, payload/VM generation and request; A release while B transfers;
late notification; close during registration/attach; independent completion in
both orders; instance failure with live host; host death with pending waits;
resource failure rollback; nested descendant cleanup; protocol mismatch;
NTCON borrowed Console/Window/return behavior and existing Full gates.
Prove exact resource ownership and no handle growth after repeated retirement.

NTMON's existing native rows and close targeting must not regress when internal
keys change; adding VM rows/display is S4. NTVDM source does not acquire the
frontend-base dependency until S3. Worker-base never acquires it. No mirror,
guest media, driver or other repository change is part of S2 design.

## Admission gate maintenance

The initial gate rejected S2 because committed Status is an explicit S1
intermission and complete-task history ends at T475. The bounded correction
recognizes the committed statement that T477 S1 is closed and T477 remains
open, admitting only T477 S2. Self-tests reject same/skipped S, another T,
a closed T and a non-intermission baseline. The existing linked prior-S closure
and packet-field requirements remain enforced. This is governance tooling,
not a product or protocol implementation.

Admission validation on 2026-11-05: owned background
Verify-DocumentationGovernance.ps1 -SelfTest exited 0; self-tests and full
relative-link/document verification passed. Evidence:
build/t477-s2/design-governance-r2.process and matching logs.
git diff --check passed. No product source, release asset, installed runtime
or guest byte was changed; this checkpoint is a local design/admission commit,
not a production P delivery. No push.
