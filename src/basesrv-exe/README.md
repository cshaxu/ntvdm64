# basesrv

This executable-owned component contains the standalone BaseSrv entry, its
versioned authenticated service transport and package-private BaseSrv bindings.
The mirrored `opennt-host/base/win32/server/srvvdm.c` remains the sole original
DOS/WOW record-policy owner. This component supplies transport and modern
resource/Console mechanics only; it must not replace that original strategy.

The S2 frontend association is independent of DOS/WOW scheduling. An original
pending command selects the worker; an authenticated inherited capability
selects the root run16. The service retains a pending resource attachment so
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
The authenticated root process defines interactive session lifetime: its death
is observed by the associated DOS worker and dispatched to original VDM close,
not to a broker process-tree kill. Pipe failure alone remains I/O failure.
Actual worker death fails unfinished requests with ERROR_PROCESS_ABORTED;
pre-handoff startup rollback remains separate.

Original worker cleanup frees DOS records, including completed results not yet
collected by their parent. Before either orderly ExitVDM or process rundown
cleanup, this binding calls the original
exit-code owner for each completed parent wait and retains only the resulting
reply, keyed by its authenticated connection and exact receipt. Delivery
consumes this one pending reply; a newly admitted command clears it. This is
response-lifetime protection, not a second DOS record or completion policy.
