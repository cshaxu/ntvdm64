# M0 T372 S4 P1 — Local completion source build

## Source owners

The source-defined local completion cohort is retained in its original owner
files:

- `mvdm-host/vdmredir/vrmslot.c` owns 16-bit mailslot identity, lifetime and
  per-PDB cleanup;
- `mvdm-host/vdmredir/vrnmpipe.c` owns named-pipe request queueing, worker
  lifecycle, cancellation and checked completion ordering;
- `mvdm-host/vdmredir/vrinit.c` owns completion-handler queueing and the
  original ICA interrupt transition.

The Redirector adapter is limited to the two external seams the source cannot
retain unchanged: session-owned host identities and bounded guest-memory
leases.  It does not interpret a `57:xx` selector or replace an owner body.

## Current source binding

`vrmslot.c` calls `mvdm_redirector_mailslot_publish/resolve/release` only at
the historical 16-bit-handle-to-native-record boundary.  The session host
identity map assigns and revokes that identity; it does not expose a host
pointer to a guest value.

`vrnmpipe.c` retains its original queue, `CreateThread` order,
`CancelIoEx` cleanup, completion handler and ICA notification.  Its one
modern lease seam prepares finite request/buffer copies before async I/O and
refuses a completion after the guest session memory has disappeared.

## Formal x86 source build and focused proof

The externally generated `build/M0-T372/S4/local-redir-x86` graph compiled
the original local owner units:

```text
vrnmpipe.c  vrputil.c  vrdisp.c  vrmisc.c  vrmslot.c
dos/command/cmdredir.c  opennt-host/netapi/netlib/ntstatus.c
```

It also compiled and ran `redirector-async-contract-fixture.exe` successfully.
The fixture proves:

- write requests snapshot guest bytes before asynchronous ownership begins;
- read completions copy only bounded bytes back through the session lease;
- byte-count/error return fields preserve the original fixed-width carrier;
- after `session_guest_memory_end`, a pending completion is rejected instead
  of publishing into stale guest memory;
- worker stop is observable before release.

The existing focused identity fixture was extended—not duplicated—to prove
the source-facing mailslot mapping contract:

- a 32-bit host identity remains resolvable from its original two-word form;
- `mvdm_redirector_mailslot_publish` allocates one nonzero 16-bit identity;
- resolve returns the original host-local record;
- release makes subsequent resolution fail.

Both x86 fixture processes exited with code zero.

## Accurate disposition

This P closes the local source compilation and bounded identity/lease proof.
It does not claim a dynamically loaded `VDMREDIR.dll`, a live host named-pipe
server, or NetBIOS/RAP behavior.  Those remain respectively the T372 S6
whole-package image and T372 S5 public-network owner work.
