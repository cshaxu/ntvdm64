# Worker Base

Project-added worker mechanisms reused by NTVDM and NTCON only. The current
connection.c/h owns their shared ConnectCurrent/WatchBroker initialization and
disconnect, including failed-watch cleanup. Both production entries call it.
The NTCON-only GetNextNativeCommand wrapper belongs to `ntcon-exe`, not this
shared library; NTVDM retains its original BaseSrv-shaped GetNextVDMCommand.
The S21 audit records this shared boundary and the owner-local mechanisms that
must not be merged: [worker-base audit](../../docs/etc/evidence/m0-t423-s21-worker-base-audit.md).
The service still owns authentication; the worker owns its heap/backend state.

run16, NTSRV, NTKVM and NTMON retain their own common two-kind handling paths.
They do not link worker-base. Launcher startup preparation belongs to run16;
console_client.c owns their common ordered frontend client, validation,
cancellation, frame chunks, input codec and atomic key-return encoding.
It borrows pipe/peer/cancel, owns its event, and requires the caller's existing
lock across each exchange/frame. Dispose follows completion of in-flight calls.
Activation is one attempt; backend retry/teardown/guest state stay local.
Cross-component declarations belong to interface. No scheduler, frontend
presentation, native Console state or original DOS/WOW policy belongs here.

Original OpenNT/MVDM execution, task completion, block/resume and cleanup
remain in their original owners. No mirror algorithm is moved into this library.
Native process results, Console membership and input consumption are different
from guest records/BIOS state; those backend operations remain local.
