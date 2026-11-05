# NTVDM VM frontend adapter

`vm_frontend.c` composes common/frontend-base directly for independent,
payload-scoped VM frontends. It does not depend on worker-base or NTCON.
NTSRV owns registration, association, downstream close and final retirement.
The adapter owns local registry entries and waits for serving threads before
releasing callback contexts. The containing worker's upstream role is separate.

The payload owner serializes registry calls. I/O callbacks run on a dedicated
channel thread and receive borrowed protocol buffers, never guest pointers.
An input readiness event and provider context must remain valid until retire
or destroy succeeds. A registration failure with a nonzero returned incarnation
retains an owned rollback entry; the owner must retire it before freeing its
context. A failure is not permission to abandon a live callback.

The origin capability is owned by its caller. Pass it to the existing BaseSrv
console-context/launch protocol; do not install it as the containing worker's
upstream context. VM handle reuse requires the full new returned identity.

`vm_host.c` serializes control operations and owns payload registries. NTVDM's
standalone worker creates this owner for DOS and closes it at the existing
worker exit authorization boundary, before releasing original host resources.
No frontend threads or downstream connections are created until registration.
I/O callbacks must not reenter registry control. The worker destroys the closed
owner only after control callers have stopped.

The unit fixture under `test/ntvdm-exe/frontend` uses real registry and
frontend-base pipe threads with a deterministic control provider. The separate
broker bootstrap integration fixture verifies actual RPC registration, distinct
contexts, reuse and upstream completion. These are complementary proofs;
neither establishes guest multitasking. Actual guest VM identity and the guest
I/O provider come from the separately queued guest bridge.
