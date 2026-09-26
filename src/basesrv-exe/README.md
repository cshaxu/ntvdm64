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

S2 also applies the owner-approved launcher/target failure pair at this finite
binding: an unfinished DOS command abandoned by its launcher is contained by
terminating its exact worker with ERROR_PROCESS_ABORTED. Original DOS records,
parent completion receipts and reservation/worker sequence select that target;
Console membership alone never authorizes termination. Original normal
completion, empty workers and WOW records remain excluded. RPC teardown may
precede process signalling, so pending-command disconnect and the exact
launcher's process-exit notification both enter the same guarded binding.
Callbacks are joined before context destruction. This is the standalone
failure contract, not a claim that OpenNT supplied recursive process-tree kill.
