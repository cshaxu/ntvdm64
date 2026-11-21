# Worker Base

Project-added worker mechanisms reused by NTVDM and NTVWM only. The current
connection.c/h owns their shared connection, explicit executable-owned loss
notification and disconnect. Owners register only after callback resources
exist and stop/join before releasing them. Transport never kills a process;
worker-base does not select DOS/WOW/native cleanup. Both production entries
connect; the nonresident GUI launcher has no CUI cleanup watcher.
The NTVWM-only GetNextNativeCommand wrapper belongs to `ntvwm-exe`, not this
shared library; NTVDM retains its original BaseSrv-shaped GetNextVDMCommand.
The S21 audit records this shared boundary and the owner-local mechanisms that
must not be merged: [worker-base audit](../../docs/etc/evidence/m0-t423-s21-worker-base-audit.md).
The service still owns authentication; the worker owns its heap/backend state.

Copied-state publication now belongs to `src/compat/publication.c`
and its B-owned interface. Worker-base does not own that implementation.
NTVDM owns VGA extraction; NTVWM owns hidden Console acquisition. The two
products compose the B publisher directly, without a frontend-base dependency.

Task observation remains here because distinct ordered facts cross from the
guest execution thread to a potentially blocking RPC publication thread.
The product unregisters its producer before drain/stop; cancellation releases
the outstanding RPC wait, and join precedes context destruction. This queue
is not a same-thread decoupling mechanism.

run16, NTSRV, NTCON and NTMON retain their own common two-kind handling paths.
They do not link worker-base. Launcher request packing belongs to run16;
NTSRV owns worker creation and shared Console retirement/return-ack decisions,
not this worker-side library. Original DOS/PIF policy stays at its source owner
and hidden Console resource operations stay at the native worker boundary.
The ordered frontend protocol client is implemented in
compat/console_transport.c, selected once in common-console.lib. Its borrowed
handles, local event, caller locks and single-attempt activation are unchanged.
Worker-only connection adaptation remains here; protocol declarations and
shared I/O client mechanics belong to compat. No scheduler, frontend
presentation, native Console state or original DOS/WOW policy belongs here.

Original OpenNT/MVDM execution, task completion, block/resume and cleanup
remain in their original owners. No mirror algorithm is moved into this library.
Native process results, Console membership and input consumption are different
from guest records/BIOS state; those backend operations remain local.
