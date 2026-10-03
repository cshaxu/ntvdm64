# M0 T423 S19 P1 — PID-first worker management

T424 S11 naming normalization: frontend labels/current source links now use NTCON.
This does not claim the new basename existed in the recorded historical package.
Exact earlier source, commands and product names remain in Git and sealed build
evidence; recorded hashes, dates, results and limitations are unchanged.

## Delivered boundary

The versioned `service.idl` management boundary is protocol 17.  Its public
worker identity is only the authenticated live process PID; service sequence
and management epoch remain server-private.  NTSRV resolves a Delete request
under its service lock against the currently registered process.  It does not
accept a caller-selected internal generation, and a PID that has subsequently
been registered again resolves to that current registration.

NTMON now renders `PID KIND ELAPSED STACK TASK`.  NTW32 is rendered as
`WIN32`, reports `MEMBERS=<n>`, and reports either its current Console client
image or `<EMPTY>`.  Its Delete path signals the NTW32 session owner and waits
for the normal Console-close acknowledgement; it is not a raw carrier kill.

## Focused x86 evidence

All commands used `build/M0-T423/S19/formal-final` and the MSVC x86 toolchain.

| Check | Result |
| --- | --- |
| NTSRV formal graph | 39/39 commands; `ntsrv.exe` linked. |
| NTW32 formal graph | 50/50 commands; `ntw32.exe` linked. |
| NTMON formal graph | 11/11 commands; `ntmon.exe` linked. |
| Versioned public management RPC | `monitor-rpc-test.exe --empty` passed: v17 empty snapshot, revision mismatch and absent PID rejection. |
| Authenticated native registry | `basesrv-service-reservation-test.exe --native-backend` passed: authenticated root, unique live instance, real identity, member report, no fake close and rundown. |
| NTW32 close path | `ntw32-close-test.exe` passed: `target-image-and-normal-console-close=PASS`. |
| NTMON rendering | `verify-monitor-layout.ps1` passed its full layout/selection/confirmation fixture. |

## Deliberate remaining work

This is P1, not S19 closure.  The formal `run16`, `ntvdm`, `ntcon` and
`VDMREDIR` targets are being rebuilt against protocol 17 before a coherent
runtime package may replace `O:/winnt`; full DOS/Window/WOW non-regression,
publication, final governance and owner verification remain S19 exit gates.

The historical Console named-pipe portion of `monitor-rpc-test` is retained
unchanged.  On this host `CreateFileW` on its `PIPE_REJECT_REMOTE_CLIENTS`
fixture returns `ERROR_ACCESS_DENIED`; the new `--empty` mode isolates the
management contract rather than relabeling that unrelated host pipe failure as
a management pass.
