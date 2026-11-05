# T477 S3 NTVDM VM frontend composition

## Scope and evidence status

This implements the admitted [S3 packet](../../states/CURRENT.md) under the
[host sequence](m0-t477-host-frontend-plan.md). The following is the implementation
contract, not a claim that its tests or guest integration have passed.
The baseline is f47111476. No guest source or binary is changed or rebuilt.

## Audited existing owners

- `common/base_srv/frontend_client.c` sends explicit instance keys over the
  process's existing authenticated RPC connection. There is no new connection
  or ambient current-frontend variable.
- `ntsrv-exe/opennt/source/frontend_registry.c` authenticates embedded
  registration against a live DOS/native worker watch. Its close operation
  drains the instance's descendants; retirement reports FINAL_CLOSE only
  after that drain. Unregister refuses a live instance or downstream worker.
- `common/frontend-base/service.c` owns request delivery, channel replacement,
  disconnect acknowledgement and broker-loss observation. Its stop operation
  joins the pump and channel before freeing borrowed adapter state.
- `common/frontend-base/channel.c` owns the existing direct I/O pipe protocol,
  cancellation and request/reply transfer. It consumes the supplied worker
  handle on every start outcome.
- NTCON's `session_service.c` and `console_channel.c` compose these mechanics
  with native presentation. Their Console restore, lease-join and window
  operations must not be copied into an embedded VM adapter.

## NTVDM ownership

An executable-owned payload registry holds independent entries identified by
payload lifetime, VM handle and incarnation. Registration rejects a duplicate
live VM; reuse after completed retirement obtains a new incarnation and a new
broker key. Lookups compare the complete identity, never the VM handle alone.
The registry does not infer guest identity from CR3, focus, HWND or a PID.

`vm_host.c` is the worker-owned control boundary. It allocates payload lifetime
identities, serializes registry operations and rejects admission after terminal
close. Each payload owns its `vm_frontend.c` registry. I/O continues on each
entry's shared service/channel threads, outside the host control lock.
The existing standalone NTVDM creates this host only for DOS, with no endpoints
or threads until a host bridge explicitly begins a payload and registers a VM.
The future bridge must end that payload before releasing its copied I/O state.

The existing `ntvdm_worker_lifecycle_authorize_exit` seam closes/join frontends
after NTSRV authorizes whole-worker exit and before original `host_applClose`
releases resources. The object remains allocated until standalone cleanup,
so concurrent control callers can receive cancellation. A local teardown error
after whole-process exit authorization fails the process without freeing
resources under possibly live callbacks. No new mirror hook is required.

Each entry directly composes frontend-base service/channel and owns the
registration's event handles, broker observation handle and adapter context.
The guest-facing provider supplies copied I/O operations and input readiness;
it must never expose a borrowed guest memory pointer to a serving thread.
The future guest bridge supplies actual VM identities and guest I/O semantics.
Host fixtures use this same executable-owned composition; fixture success is
not acceptance of the queued guest driver.

The VM adapter validates wire version, authenticated attachment generation,
strictly increasing request sequence and operation range before calling the
provider. It stamps reply identity itself. The provider owns operation payload
validation and semantics; it cannot replace broker/channel identity.

Control and I/O remain separate. NTVDM's existing worker-base connection is
its one upstream backend role. An entry's frontend RPC calls and downstream
pipe do not replace that connection, change its active task, or invoke native
Console restoration. Worker-base gains no frontend-base dependency.

## Lifetime and failure ordering

1. Validate the payload/VM identity and callback contract before registration.
2. Register with NTSRV, retain its key/events, acquire broker/state observation,
   and start the shared service pump. Publish a usable entry only after start.
3. For each broker request, start one shared channel with the entry's adapter.
   NTSRV remains the authority for attach, handoff and disconnect.
4. VM/payload retirement requests close from NTSRV. Keep the entry and its
   callbacks alive while downstream carriers drain. Wait on existing events
   and thread completion, without polling, deadlines or reconnection.
5. After authorized retirement, join service/channel, unregister the instance,
   then release handles and adapter storage. A failed join retains storage.
6. On broker loss, join local serving work and release local resources. Do not
   reconnect, invent successful unregister, or terminate the containing worker
   from a per-VM callback; existing worker lifecycle owns process retirement.

Startup rollback follows the same close/unregister authority after successful
registration. Failure before registration has no broker object to retire.
An I/O fault requests closure of the affected instance, not sibling instances
or the containing worker's upstream frontend.

## Finite implementation and verification checklist

- Implement the payload registry and direct service/channel composition in
  `src/ntvdm-exe`, with a documented serialized registry control owner and
  separately running I/O callbacks. Select them in the real NTVDM build.
- Verify production-path registration, duplicate rejection, stale incarnation,
  handle reuse, independent 1+N entries and callback lifetime on teardown.
- Exercise actual NTSRV association and nested chains with host fixtures;
  assert upstream ownership is retained while descendants start and finish.
- Verify request/attach failures, channel cancellation, broker death and
  retirement with active callbacks; retain failures rather than fake success.
- Run cached builds, required regression gates and coherent publication;
  review source/dependency direction before commit and push.

No changes to mirror algorithms, guest VMM entrypoints or NTMON UI are part
of this checklist. Any newly discovered contract gap is recorded against
these boundaries rather than silently replaced with a second transport.
