# T412 S4 DOS execution, reentry and stream closure

## Scope

S4 connects the retained DOS `BOP 54:08`/`cmdExec32` native-child route to
the three x86 programs.  It does not implement guest COMMAND redirection or
WOW16 execution.  The original `cmdexec.c` worker, its reentry ordering and
the original BaseClient/BaseSrv service calls remain the policy owners;
`run16` is only the public child launcher and `basesrv` only the copied IPC
binding.

## Requirement ledger

| Required S4 result | Current evidence | Disposition |
| --- | --- | --- |
| Direct DOS application launch and return | `m0-t412-s4-direct-mem-r3` reaches package-root `MEM.EXE` and exits 0. | Passed. |
| COMMAND prompt/input/return | `m0-t412-s4-console-exit-r27` records original banner/prompt, ordinary `ver`, `exit`, and launcher exit 0. | Passed. |
| EDIT startup and real Console input | Direct EDIT reaches `QBASIC.EXE`; the retained visible owner click and Console observer records establish the selected Console mouse route. | Passed for the admitted startup/click route; it does not claim full editor/WOW16 completion. |
| Guest BOP DOS child | `m0-t412-s4-bop-native-child-r2` records outer reentry, nested `run16 -> basesrv -> ntvdm` Check/reserve/prepare/Get, child return and parent cleanup for `MEM.EXE`. | Passed. |
| Quoted shell tail and public fallback | `m0-t412-s4-bop-shell-fallback-r4`/`r5` retain a quoted copied tail, real `54:08` reentry and zero child return. | Passed. |
| stdout and stderr bytes | `m0-t412-s4-bop-shell-fallback-r6` records the two distinct `echo` operations and both markers in its real Console snapshot. | Passed for the Console stream path. |
| EOF and nonzero result | `m0-t412-s4-bop-shell-eof-r1` proves a native `more` receives shell-provided NUL EOF; `m0-t412-s4-bop-shell-nonzero-r1` records native child phase-1/status-service `0x25`. | Passed. |
| Captured command/environment/stream lifetime | The retained native-child payload reports copied command and environment transform before detached child creation; the S4 stream reservation evidence covers Update-time worker-local stream use and its rejected receipt-only variant. | Passed for the selected lifecycle. |
| Failure rollback/disconnect | S3 reservation/registration/disconnect negatives plus S4 nonexistent native-child observation retain reentry decrement and parent cleanup after failure. | Passed. |
| Direct versus guest `>` | Direct literal `>` remains a literal argument; guest COMMAND redirection creates no output file. | Distinguished. Guest redirection remains the explicit Redirector owner boundary, not an S4 substitute implementation. |

## New final observations

The final deployed x86 three-program image uses `run16.exe` SHA-256
`A3A9DF96DEEC83ED4F714BA98A84A502E29C0C0965E387203B95CF7FDCD6F33A`.
The `r4` failure to reach the BOP path used redirected standard handles and
therefore had no Console event-thread container; the Console-owning `r5` and
`r6` reruns correct that test setup without changing product code. Every
temporary `O:\ntvdm64\COMMAND.PIF` sidecar was a byte-identical copy of the
packaged pure-DOS PIF and removed after its observation.

## Limit and handoff

S4 does not establish guest-internal COMMAND redirection, full WOW16/WRITE,
idle worker retirement or empty-broker exit.  Guest `>` remains owned by the
existing Redirector recovery boundary.  S5 receives broker singleton/WOW
coordination and eligible-idle cleanup only; it may not regress the completed
S4 COMMAND/EDIT interaction path by reaping an active Console worker.
