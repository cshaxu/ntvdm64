# SAS startup debug observation: non-pass

A test-only debugger attempted to start an isolated broker under DEBUG_PROCESS,
then launch the unchanged DOS MEM guest, checking the selected CCPU tail
breakpoint address adjusted for the worker image base. The launcher exited
with 1722 (RPC server unavailable); no NTVDM process creation event was
observed. The broker did produce debugger events. The result therefore says
nothing about SAS execution, startup reachability, or guest compatibility.
It is not a product regression verdict or a successful test.

The first attempt likewise returned 1722 without identifying a worker; the
retained second attempt adds debugger event logging. Its exact test source,
run wrapper, output and binary/input hashes are recorded here. The reason for
the broker/launcher failure was not established. No guest or installed product
was changed, and the isolated stage was cleaned by its established owner.

## Run16-owned bootstrap control

A second test-only observer let Run16 create the broker under the normal
bootstrap path, with a hidden new console and `DEBUG_PROCESS`. It observed
an NTVDM worker created under the debugger with image base `0x01000000` and
selected tail breakpoint address `0x011782E0`; neither tail breakpoint fired
among the captured events. However the launcher did not complete before the
60-second test watchdog. The worker reported multiple first-chance
`0x40000005` exceptions; this observation does not establish their cause or
whether the debug observer affected progress. The runner reported `done=0`,
so this remains a non-pass and is not a successful MEM run, full startup-to-
exit observation, or proof the breakpoint is unreachable. Raw events, source,
wrapper and identities are retained here. The isolated package cleanup ran;
no installed component or guest was changed.

## Debug event and isolation controls

`0x40000005` is the original `STATUS_VDM_EVENT`: `dbg.c::SendVDMEvent`
raises it and existing `wow_exception_observer.c` explicitly continues it.
The v2 observer passed it as unhandled. The v3 observer continued these
events but still timed out; the event-handling mistake was real but is not
proved to be the timeout's sole cause.

A same-setup no-debug control also timed out. Repeating with the established
fresh isolated package identity, stage-local TEMP/TMP/PATH, and an absolute
MEM path completed with exit 0. Therefore the earlier timeout cannot be used
as evidence of a debugger-only failure or SAS behavior. Applying that stage
setup and VDM continuation to v4 reached the worker but exited with
`0x4000001F` before a valid completed run. Windows SDK names this status
`STATUS_WX86_BREAKPOINT`. V5 additionally continued one such event per process
as a provisional initialization breakpoint; another distinct WX86 breakpoint
was delivered and passed through, and the worker and launcher exited with
`0x4000001F`. Neither breakpoint matched the instrumented SAS address. This
is a non-pass; the second event's origin and correct disposition remain
unverified. Do not suppress it or claim an unreachable SAS entry on this basis.
All runs used an isolated stage, with no product or guest modification.
