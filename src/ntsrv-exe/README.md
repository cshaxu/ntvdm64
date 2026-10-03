# ntsrv executable owner

This executable-owned component contains the standalone BaseSrv entry, its
versioned authenticated service transport and package-private BaseSrv bindings.
The mirrored `opennt-host/base/win32/server/srvvdm.c` remains the sole original
DOS/WOW record-policy owner. This component supplies transport and modern
resource/Console mechanics only; it must not replace that original strategy.

The private service implementation under `opennt/source` is separated into
`service_core.c` (connection/authentication/rundown), `worker_registry.c`
(reservation/admission/reuse/process watches), `frontend_registry.c`
(root identity/routes/Console takeover and return), `native_commands.c`
(direct native delivery and receipt), `lifecycle.c` (authoritative retirement
and shutdown), and `management.c` (read-only projection). `base_service.c`
retains the source-shaped DOS/WOW interface/resource adapters. These are
translation units of the same provider, not new components or registries.
`opennt/include/service_internal.h` is private to those units and the test-only
hook fixture. It exposes the same explicit service instance, unchanged recursive
service lock and finite cross-module helpers; it is not a public/wire ABI.
Original DOS/WOW records, locks and algorithms stay in `srvvdm.c`. The native
command queue is not a generalized DOS/WOW scheduler. Fixture hooks link the
production provider, rather than embedding another copy of the service.

The private service implementation under `opennt/source` is separated into
`service_core.c` (connection/authentication/rundown), `worker_registry.c`
(reservation/admission/reuse/process watches), `frontend_registry.c`
(root identity/routes/Console takeover and return), `native_commands.c`
(direct native delivery and receipt), `lifecycle.c` (authoritative retirement
and shutdown), and `management.c` (read-only projection). `base_service.c`
retains the source-shaped DOS/WOW interface/resource adapters. These are
translation units of the same provider, not new components or registries.
`opennt/include/service_internal.h` is private to those units and the test-only
hook fixture. It exposes the same explicit service instance, unchanged recursive
service lock and finite cross-module helpers; it is not a public/wire ABI.
Original DOS/WOW records, locks and algorithms stay in `srvvdm.c`. The native
command queue is not a generalized DOS/WOW scheduler. Fixture hooks link the
production provider, rather than embedding another copy of the service.

The S2 frontend association is independent of DOS/WOW scheduling. An original
pending command selects the worker; an authenticated inherited capability
selects the NTKVM frontend root. The service retains a pending resource attachment so
root/request rundown can cancel a waiting worker before the pipe exists.
It transfers pipe/event references once, never keyboard records or frames.
A cancelled undelivered attachment cannot be adopted by another root. After
delivery, the existing direct-channel/process-loss contract handles failure;
the original worker-exit cleanup retires its resource association.
For cancellation before worker Connect, no worker watch exists. A subsequent
successful client admission reclaims canceled routes whose retained process
handle is signalled; live cancellation markers remain until worker exit.
Service stop also drains the final idle residue. This is resource cleanup,
not an idle-worker timer or a new task scheduler.

Non-root launcher rundown never terminates handed-off DOS tasks or workers.
The authenticated NTKVM root defines interactive session lifetime. NTSRV
observes root loss and sends its authoritative worker shutdown instruction;
the worker executes its own original/native Console-close boundary, not a
broker process-tree kill. Pipe failure alone remains I/O failure.
Actual worker death fails unfinished requests with ERROR_PROCESS_ABORTED;
pre-handoff startup rollback remains separate.

The S4 candidate also centralizes exact sibling NTKVM/NTVDM/NTVWM creation
and all launcher Console takeover/restore acknowledgement through this service.
Independent DOS worker-exit completion preserves original DosSesId/PIF policy.
Self-owned native text close-on-exit follows final I/O acknowledgement and a
conservative backend resource check. Both use the same broker retirement and
sticky Console-return latch; borrowed sessions retain resident workers.
Only worker/frontend I/O travels directly, without the broker as a frame relay.
The current active packet/evidence, not this component description, determines
verification and publication status.

Original worker cleanup frees DOS records, including completed results not yet
collected by their parent. Before either orderly ExitVDM or process rundown
cleanup, this binding calls the original
exit-code owner for each completed parent wait and retains only the resulting
reply, keyed by its authenticated connection and exact receipt. Delivery
consumes this one pending reply; a newly admitted command clears it. This is
response-lifetime protection, not a second DOS record or completion policy.
