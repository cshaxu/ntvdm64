# M0 T228 S1 — immutable multi-program integration observation 001

## Inputs

A fresh T228 input root was staged from the retained original-toolchain
source-built NTIO, NTDOS, COMMAND and SHARE artifacts. Each source stage,
manifest, byte count and SHA-256 was revalidated; SHARE is copied byte-for-byte
as the declared `TARGET.EXE` profile alias. The input manifest is
`ntdos64.t228.s1.guest-exec-integration-inputs.v1`, SHA-256
`c888239b9de415f5c5f38b8d8dcacf5a30c102ab1465189c1ffc1a97fb1b9b6a`.

The current formal native CLI is
`build/M0-T227-S1/dem-semantic-20260817a/bin/ntdos64-native.exe`, SHA-256
`fd04cf4a2a46e5a65df7db96d8a8d77d89597ec7276c1d783ef9c17b9378fad8`.

## Procedure

The T228 watchdog used only existing CLI options: the same staged profile and
root, one Direct and one Readonly run, `--observe-guest-exec-lifecycle-ledger`,
and bounded instruction budgets. No BOP sequence, instruction history,
first-fault observer, guest byte mutation, synthetic EXEC/PSP/return, or
Bochs semantic variation was enabled.

| Budget | Direct | Readonly |
| ---: | --- | --- |
| 1,000,000 | exit 4; one phase-1 `50:36` record | byte-identical exit/log result |
| 10,000,000 | exit 4; one phase-1 `50:36` record | byte-identical exit/log result |
| 100,000,000 | exit 4; one phase-1 `50:36` record | byte-identical exit/log result |

At 100M the copied record is `CS:EIP=0041:5A70`, `DS:ESI=0C41:00000100`,
with resume `5A74`. No child termination `50:3C` or COMMAND parent-return
`54:0B` record occurs. The two 100M stdout SHA-256 values are both
`05ba24d43968575f43ba538118ea606631356fe70d248217b0e8ea508f7c074a`; the
stderr values are both
`905ddf181613eb9b79dfeec0a507def81b0c26ffb1a1518ce10e4a19b0166406`.

## Interpretation

The original `$Exec` entry contract is reached and resumes normally. The
run then consumes the bounded CPU budget before any normal child exit,
`demTerminatePDB`, or COMMAND parent return. Direct/Readonly parity excludes
host mutation policy as the cause at this boundary; the completed BOP local
packages also cannot be reopened from this observation.

This does **not** identify a repair. The remaining owner question is whole
Guest-DOS/COMMAND execution continuity versus bx-core/bx-mantle CPU/machine
continuity after `50:36`. A later, separately admitted default-off
instruction-boundary diagnostic may use the already registered BX-CORE-071
mechanism; S58's unaccepted working-tree material is not part of this result.