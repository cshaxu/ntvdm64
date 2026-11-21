# Frontend connection base

Project-owned frontend connection mechanisms extracted from NTCON. The channel
owns pipe framing, authenticated attachment sequencing, serving-thread lifetime,
readiness notification and cancellation/drain. The executable supplies explicit
process-local callbacks for admission binding and operation dispatch.

NTCON owns its native Console, Window, input receipt and
presentation state. NTSRV remains the lifecycle authority. This library does not
select workers, reconnect, relay service I/O, or depend on executable-private
code or worker-base. Future NTVDM frontend instances compose this library
directly, independently of their worker-base backend mechanisms.

The channel consumes its worker reference on every start outcome. Adapter
context and the input-readiness handle remain borrowed until stop joins.
The serving thread drains pending I/O, finishes adapter transactions, closes
its pipe and wakes peer readiness before exit. Failed joins retain ownership.

Tests under test/frontend-base link the production archive with real
pipes, threads and events. Their local attachment binding does not establish
RPC authentication or real guest acceptance; those use integration gates.

The service instance owns its pump thread, stop event and at most one channel.
Notification, retirement, broker-process and state-change handles are borrowed
from the host adapter. Service actions, Console membership, restoration and
parking use explicit callbacks; NTSRV still authorizes every association.
A disconnected acknowledgement follows successful channel join/disposal.
Unexpected EOF requests participant exit once, then waits for service-directed
cleanup; failed control/restoration does not authorize retries or success.
Return preparation freezes request admission until final service retirement.
Stop joins service first, then channel; failed joins retain borrowed state.

NTCON retains native presentation creation/destruction and its BaseSrv control
binding. Its service wrapper has no duplicate request/pipe-fault pump.
NTVDM supplies VM-specific adapters directly, without worker-base including
or linking frontend-base. Multi-instance RPC addressing is owned by common
and the product adapters;
the two-instance unit fixture alone does not establish real RPC acceptance.

Frame validation, chunk assembly and staged/committed serials belong to
`src/compat/console_video_buffer.c`, not frontend-base. Adapters retain
presentation and acknowledgement policy; accepting a frame is not proof
of guest display.
