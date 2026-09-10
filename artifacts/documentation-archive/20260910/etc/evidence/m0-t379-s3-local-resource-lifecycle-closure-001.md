# M0 T379 S3 — Original VDMREDIR local resource/lifecycle closure

## Question

Does the admitted local VDMREDIR cohort preserve original pipe/mailslot and
completion lifecycle ownership—including cancellation, stale guest memory and
session resource release—without placing a host pointer or HANDLE in a guest
or durable cross-thread record?

## Inputs

* Original `mvdm-host/vdmredir/vrnmpipe.c`, `vrmslot.c`, `vrinit.c` and their
  `VrTerminateNamedPipes` / `VrCancelPipeIo` / mailslot lifecycle bodies.
* Existing `adapter-mvdm-host-out/redir` handle, mailslot, guest-copy and
  async boundaries; session mapping manager and guest-location lease.
* Existing focused tests `redirector_async_contract_fixture.c` and
  `t290_s3_redirector_handle_fixture.c`.
* S2's formally linked original `VDMREDIR.dll`.

## Owner matrix

* `vrmslot.c` retains original mailslot allocation, lookup and deletion flow.
  The adapter publishes its private record through the active session's
  host-resource identity instance, exposes only a nonzero `WORD`, and removes
  the mapping on release.  A stale or zero word resolves to no native handle.
* `vrnmpipe.c` retains original request packing, original queue/completion
  order and `VrCancelPipeIo` / `VrTerminateNamedPipes` control surfaces.  The
  adapter snapshots write bytes or stages read bytes; it reacquires a checked
  guest lease only at completion.  `PrivateAsyncState` is private host-side
  request bookkeeping, never a guest-visible field or a worker-carried guest
  alias.
* `VrTerminateNamedPipes` asks the session-owned worker boundary to stop.  A
  stop request is explicit and observable; it does not tear down the host
  process.  Completion after guest-memory/session end fails rather than
  copying to a stale address.
* `vrinit.c` retains original initialization/error direction and the local
  service lifecycle.  Remote RAP/XACTSRV remains S2's separate unavailable
  owner and does not enter this local lifecycle claim.

## Procedure and verification

The existing x86 tests were regenerated and executed; no new fixture was
created:

```text
New-T324S2LocalRedirectorNinja.ps1 -Architecture x86 \
  -RepositoryRoot O:\repos.hobby\ntvdm64 \
  -BuildRoot O:\repos.hobby\ntvdm64\build\M0-T379\S3\local-async-x86
build\M0-T379\S3\local-async-x86\msvc-x86.cmd ninja -C \
  build\M0-T379\S3\local-async-x86
build\M0-T379\S3\local-async-x86\msvc-x86.cmd \
  build\M0-T379\S3\local-async-x86\bin\redirector-async-contract-fixture.exe

New-T290S3RedirectorHandleNinja.ps1 -Architecture x86 \
  -RepositoryRoot O:\repos.hobby\ntvdm64
build\M0-T290\S3\redirector-handle-x86\msvc-x86.cmd ninja -C \
  build\M0-T290\S3\redirector-handle-x86
build\M0-T290\S3\redirector-handle-x86\msvc-x86.cmd \
  build\M0-T290\S3\redirector-handle-x86\bin\t290-s3-redirector-handle-fixture.exe
```

Both returned zero (`ASYNC_PASS`, `HANDLE_PASS`).  The async proof covers
write snapshot, staged read completion, returned count/error words, rejected
completion after `session_guest_memory_end`, and worker stop state.  The
handle proof covers publish/resolve/release and confirms that resolving a
released word returns `NULL`.  S2's formal DLL target was then re-run and
reported `ninja: no work to do` (`DLL_PASS`).

## Observations

* Local guest memory is used only through synchronous bounded leases.
* Mailslot IDs and ordinary host handles are session-owned opaque identity,
  not pointer truncations.
* A stale session/memory region cannot receive an asynchronous completion.
* The original cancellation/termination entrypoints remain in the DLL source
  and export surface; this S does not claim a remote pipe server or network
  transport.

## Interpretation and confidence

High confidence for the selected local lifecycle cohort.  Its original source
flow, current bounded binding and focused x86 execution agree.  This closes
the prerequisite for packaging the DLL beside the parent in S4; it is not a
claim that every `57:xx` service, named-pipe peer or LANman transport runs.

## Follow-up

S4 packages the exact formally linked DLL with the current CPU40 parent and
the immutable `REDIR.EXE` workload, then proves normal loader and retained
failure directions without making the product observation yet.
