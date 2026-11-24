# T484 S4 VGA cursor preparation and current observation owner

P21 stages cached CURSOR.COM through CMD TestFiles instead of interpreted
copies into the runtime root. Preserve real register readback, CAF ownership,
normal exit and three ordered unchanged-text frame assertions.

## Failed evidence and correction

build/t484-s4-cursor retains all attempts. PID15448 completed cached full build
but its temporary runner referenced nonexistent worker/run-ninja.cmd and stopped
before the diagnostic test. Corrected runner PID17964 built and staged the
console-wire worker through existing CMD owners. Guest exited0 and CAF reached
the window, but there were zero captured frames and the state assertion failed.

Source and MAP inspection proved the test wrapper still included F's
ntvdm-exe/win32/console_client.c after video publication moved to B's
compat/console_services.c. Its observed_worker_video was no longer selected.
The current frame ABI still matches the test offsets. Update only the diagnostic
wrapper to include the actual service provider and use that owner's profile_24
compile flags. The real transport, reply/failure and product code are unchanged;
no fake frame or assertion relaxation is introduced.

## Final verification

Owned modern pwsh/current-token Userenv PID10444,
2026-11-24T18:40:34Z to18:42:08Z, exit0: cached full build, current diagnostic
compile/link, CMD alias/package preparation and real private-desktop guest run.
The diagnostic package uses existing released inputs and only stamps its owned
copy. Five frames prove visible at20,10 with start2/height3, hidden at20,10,
then visible at21,10 with identical text cells. Register readback and CAF window
ownership pass; guest and launcher exit normally. The known diagnostic link
output-name warning is recorded and VdmTib ownership/overlap verification passes.

All11 raw product inputs and22 dual-published hashes remain unchanged. No
original guest modification, new Full run or production cursor fix is claimed.
The source change repairs the observer, not the product. Old zero-frame failure
is retained separately and never counted as a pass.

## Accounting

Frozen census remains343/355 disposed,12 pending. Supplemental preparation
chains decrease12 to11. S4 remains active; S5 final clean acceptance remains
outstanding. Governance, parser and diff checks gate P21 delivery.
