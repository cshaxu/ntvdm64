# M0 T472 S2 — DOS fatal-dialog terminal-path proof

## Question

Does the existing original DOS fatal-dialog tail retain NTSRV as the authority
for exit admission, without adding a local terminator or an extra timeout?

## Source chain

`nt_error.c!ErrorDialogBox` maps a DOS `RMB_ABORT` to original
`TerminateVDM`.  `nt_reset.c!TerminateVDM` calls the original `terminate()`,
which calls `host_applClose` and `host_terminate`; the latter retains the
original `ExitVDM` then process-exit order.

The only project-owned bridge in that chain is
`ntvdm_worker_lifecycle_authorize_exit` before original cleanup.  It:

1. sends the authenticated `RequestParticipantExit` RPC once;
2. waits indefinitely for the existing NTSRV worker-close instruction or
   confirmed NTSRV loss; and
3. never interprets successful request submission as local authorization.

`OpenNtBaseServiceRequestParticipantExit` authenticates the process and
generation, resolves its owned worker watch, and calls the existing
management close transaction.  `service_terminate_worker` latches exactly one
worker-close event under the service lock and returns acceptance without
waiting for that worker.  Thus the wait is not circular: the worker submits,
NTSRV signals, then the original terminal tail proceeds.

No alert/join or second process terminator exists between selected
`ErrorDialogBox` and `ExitVDM` in the current source.

## Non-interactive witness

The cached x86 test image was run through an owned background runner:

| Case | Result | What it proves |
| --- | --- | --- |
| `console-client-test.exe --natural-exit` | Exit `0`; `PASS natural pre-cleanup permission waits for instruction/loss; same-thread recursion reports once` | A natural original-shaped terminal entry submits once, waits for the worker-close instruction, and does not duplicate the request when the original close cohort re-enters. |
| `console-client-test.exe --natural-console` | Exit `0`; same assertion | The distinct Console-close instruction follows the same authorization rule rather than becoming a spontaneous process exit. |

Inputs and captured output are disposable under `build/t472-s2/`.  The
attempted broader service-reservation executable did not leave an exit receipt
under this restricted runner, so it is deliberately not counted as evidence.
The source chain above is the service-side proof for this S; it is not a claim
that a human-clicked fatal dialog was exercised.

## Result and handoff

No production source change is justified: the selected DOS fatal path already
has one original terminator and one NTSRV authorization route.  S3 owns the
separate nonfatal `RcMessageBox` callers and the historical `host_error`
presentation/continuation discrepancy.  A real modal fatal-dialog click, if
needed later, remains an S5 integration witness rather than a reason to add
test-only product behavior here.
