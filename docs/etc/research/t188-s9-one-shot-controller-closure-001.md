# T188 S9 one-shot controller closure 001

Date: 2026-08-12  
Packet: M0 T188 S9  
Disposition: controller and fixed bundle are closed; no guest was run.

## Cause addressed

T187's sole observation produced a log but no terminal `observation.json` after
its process-tree cleanup reported access denied.  The old controller then made
an unbounded `WaitForExit()` after `taskkill`; a failed tree kill could prevent
its `finally` record from being reached.  This packet changes the controller,
not Bochs, the adapter, a provider, or the CLI.

## Controller contract

`tools/probe/Invoke-T188S10CopiedStateObservation.ps1` is the only planned S10
invoker.  Before launch it rejects an existing evidence directory and writes
an immutable input record.  It launches exactly one wrapper process and fixes
`retryPolicy` to `none`.

On a timeout it can issue only `taskkill /T /F /PID <its-wrapper-pid>`; this is
the observation's own process tree, never a name-based or unrelated-process
operation.  It captures the cleanup command output and exit code, waits at
most five further seconds, and records `cleanupWaitTimedOut` if the process is
still alive.  Its `finally` writes `observation.json` with launch, watchdog,
cleanup, child-exit and log-hash fields even when cleanup fails.  There is no
unbounded wait, sleep, loop, retry, or result-success interpretation.

`tests/bx-ntvdm-adapter/Test-T188S9ObservationControllerBoundary.ps1` statically
verifies those boundaries and passed.  This is an intentionally static
controller proof: exercising a cleanup-failure path would require another
process observation and is therefore reserved to the one separately admitted
S10 execution.

## Fixed bundle

The existing fixed-bundle tool copied the S8 engine once to
`artifacts/build/current/t188-copied-state-bundle-r1`.  Its manifest records:

| Asset | SHA-256 |
| --- | --- |
| Engine | `766D3A7AAB980B05C683DD99530801F61578F9F886DD0A1122BF810D6EA8E938` |
| BIOS | `D974DE50C94D76445123FD1AC886612351BED7CC5E3C8D07C556814DA72BEBD6` |
| VGA BIOS | `CD9FDD6A789DCD22B8A6B3B152788D43238DE49CCE674CFF57BDEB94580246C6` |
| POST option ROM | `E4CA4AE5190A15ED741D1DFED5A58533A2B6926A40302D839FB714F9FC673FFB` |

The retained CLI input identities for a future S10 are runner
`67F50C468F29305A23A11E9BCF0E4CB8B1EBD909E78C251A399D74347ED41661`, shim
`4D8750B9D9C6FFFAF39643EFCF4668F8DDF83EC114166DFCE6B6435C07D80B59`, v5
profile `09E06FA81225E2308415DABFFC2BBBCAC03E6918185F7A139D7A97960662F129`,
and target `69DABBDB754B358AC4FE4B22DE04C0E4C93076816F14BB0730CAA9FD223996FC`.

## Next boundary

S10 may make exactly one watchdog-bounded run using these fixed inputs.  It
will classify only the copied lifecycle line emitted by BX-TRACE-060 and the
existing pass-through/continuation evidence.  A missing lifecycle line,
access-denied cleanup, or other failed result remains recorded evidence and
does not authorize a retry or a semantic change.
