# T188 S10 copied-state observation 001

Date: 2026-08-12  
Packet: M0 T188 S10  
Disposition: one observation completed; no retry is permitted.

## Immutable execution

The sole invocation used the S9 controller with its 30-second watchdog and
fresh root `artifacts/analysis/t188-s10-copied-state-observation-r1`.  Inputs
match the S9 frozen identities: runner
`67F50C468F29305A23A11E9BCF0E4CB8B1EBD909E78C251A399D74347ED41661`, shim
`4D8750B9D9C6FFFAF39643EFCF4668F8DDF83EC114166DFCE6B6435C07D80B59`, S8
engine `766D3A7AAB980B05C683DD99530801F61578F9F886DD0A1122BF810D6EA8E938`,
v5 profile `09E06FA81225E2308415DABFFC2BBBCAC03E6918185F7A139D7A97960662F129`,
and target `69DABBDB754B358AC4FE4B22DE04C0E4C93076816F14BB0730CAA9FD223996FC`.

The terminal record is hash
`64C944B0B8FA16711C37EFEDB1887C0824479E2F0B62BE8B6981A97ABC106BCF`.
It records a 30,427 ms run, `watchdogTerminated=true`, `treeKillExit=0`,
`cleanupWaitTimedOut=false`, and no launch error. The controller terminated
only its wrapper PID 25040 and descendants. Its stderr hash is
`637E19EF28046309C1471766B13FD2FA6A073809E708DD77DB01F924FB3E83CB`.

## Observed ingress state

The stderr log records 185,570 BOP observations. At the repeated command BOP:

```text
rip=732 selector=54 ... service=0c ... cs=8dc8 ds=8dc8 dx=33d9
ntdos64 adapter lifecycle installed=1 provider=1 pending=0
ntdos64 adapter boundary passed through ... bytes=c4c4540c
ntdos64 real-mode vector vector=06 ... target-cs=f000 target-ip=ff53
```

There are 185,559 lifecycle/pass-through records and 185,558 vector-06
records. The count edge is log truncation at watchdog termination, not a
different classified outcome.

## Interpretation and limitation

This directly distinguishes the prior alternatives:

- The adapter runtime is installed.
- The boot namespace provider is present.
- No pending adapter transaction exists at the observed `54:0C` ingress.
- The generic interceptor therefore keeps its unchanged pass-through behavior;
  Bochs takes its unchanged real-mode #UD vector path.

It does **not** establish why the selected v5 route produced no pending
transaction. The T188/S2 static fixture proves the same source-derived route
can create one under its controlled state, so the remaining work is to recover
the exact runtime predicate/input divergence. It is not evidence to add a
selector recognizer, force a transaction, modify Bochs #UD behavior, or retry
this run.
