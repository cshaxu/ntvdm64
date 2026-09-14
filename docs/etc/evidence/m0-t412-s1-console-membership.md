# T412 S1 Console membership boundary

## Probe and result

Run `node tools/audit/Verify-BrokerConsoleMembership.mjs`. The driver rebuilds
an x86 MSVC /MT fixture under `build/M0-T412/S1/console-membership`, verifies
its PE machine, and records `result.json`. No runtime product is modified.

The isolated controller creates hidden Console A with child A, attaches only
its own process to A, then creates child B inheriting that Console. It creates
child C with a different Console. Querying through B finds A and B but not C;
querying through C finds C but neither A nor B. Queries use AttachConsole and
GetConsoleProcessList, not window titles, HWND equality or claimed Console IDs.
All three exact owned children must exit normally after the stop event.

The first attempt lacked child-readiness synchronization and failed. The final
probe waits for an explicit child-ready event before attachment/query. It
passes. Process creation return alone is not worker/Console readiness evidence.
The controller never detaches or otherwise operates a user-owned process.

This proves membership separation in the tested native Console environment.
It is not a stable Console identifier, an authentication test, a ConPTY test,
or proof across Console close/recreation and PID reuse.

## Selected binding design

Original BaseSrv Console/DosSesId selection remains the owner. Broker Console
IDs are local surrogate keys for that owner, never client-supplied HANDLEs.
The binding checks membership against live registered process identities,
including process creation/generation and OS-authenticated caller identity.
It must not assign the same key merely because a PID, HWND or title matches.

The persistent broker must not attach itself to arbitrary caller Consoles:
Console attachment and control-event effects are process-wide. Use an owned,
short-lived internal run16 helper for the bounded membership query. This is
a role of one of the three requested binaries, not a fourth executable or a
second BaseSrv. Helper arguments are not authority: bind its request/result
to the broker's authenticated transaction and known live process handles.

The helper returns membership only for the requested caller and already
registered candidate workers/reservations. Do not scan/open unrelated PIDs.
Before accepting a result, revalidate the queried process handles and their
generations. Expired helpers, exited processes, truncated membership snapshots
or changed registration invalidate the result and cannot authorize reuse.
Do not cache a Console association across death/recreation without revalidation.

Before a worker is ready, its launch reservation is anchored to the live
launcher and original unregistered DosSesId state. After the worker completes
its explicit registration/readiness handshake, live membership supplies the
Console binding. Record cleanup remains original BaseSrv policy; the helper
does not select commands or decide worker idleness.

S3/S5 must test simultaneous launch, detached/new Console registration, same
and different Consoles, closure/recreation, helper failure and stale process
generations. Modern-terminal coverage remains mandatory where deployed. The
probe above does not waive any of those integration cases.
