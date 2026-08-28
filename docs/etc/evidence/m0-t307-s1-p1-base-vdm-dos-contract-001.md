# M0 T307 S1 P1 — Base VDM one-session DOS contract

## Question

Can the reached DOS-side Base VDM public family retain the original OpenNT
entry names, `VDMINFO` layout, copy/capacity/reentry ordering and terminal
intent without importing the unavailable CSR/CSRSS product shell?

## Original source and selected route

- `opennt-host/base/win32/client/vdm.c`: `GetNextVDMCommand`, `ExitVDM`,
  `SetVDMCurrentDirectories` and `GetVDMCurrentDirectories`.
- `opennt-host/base/win32/server/srvvdm.c`: the DOS paths of
  `BaseSrvGetNextVDMCommand`, `BaseSrvIsFirstVDM`, `BaseSrvSetVDMCurDirs`,
  `BaseSrvGetVDMCurDirs` and DOS exit handling.
- Direct source composition stops at CSR capture buffers, console records,
  duplicated wait handles and BaseSrv/CSRSS global process state. T306 records
  this as a non-invasive hard boundary.

## Result

`adapter-mvdm-host-out/basesrv` now provides the smallest source-shaped local
facade:

- `GetNextVDMCommand` retains caller-local `VDMINFO` capture/result copying,
  required-size feedback, environment retrieval, one DOS record, and reentry
  increment/decrement behavior.
- `GetNextVDMCommand(NULL)` now retains the original first-VDM query-and-clear
  behavior on the product's single bound session.
- `SetVDMCurrentDirectories` and `GetVDMCurrentDirectories` retain the
  original copied `MULTI_SZ` capacity and one-shot-consumption contract. The
  copied record is owned by the bound session rather than a CSRSS console
  record.
- DOS `ExitVDM(FALSE, ...)` retains terminal VDM intent by completing the
  bound session. It deliberately does not terminate the CLI host process.

Every changed source branch has a `DIVERGENCE:` comment and an indexed
`ADAPTER-BASESRV-*` entry in the component README.

## Verification

The formal, disposable T307 Ninja graph was generated and run outside the
sandbox for each required architecture:

```text
powershell -ExecutionPolicy Bypass -File tools/build/New-T307BaseVdmBrokerNinja.ps1 -Architecture x86 -RepositoryRoot O:\repos.hobby\ntvdm64
ninja -C build/M0-T307/S1/x86 test
PASS: local Base VDM broker contract

powershell -ExecutionPolicy Bypass -File tools/build/New-T307BaseVdmBrokerNinja.ps1 -Architecture x64 -RepositoryRoot O:\repos.hobby\ntvdm64
ninja -C build/M0-T307/S1/x64 test
PASS: local Base VDM broker contract
```

The fixture covers unbound rejection, first-VDM query/clear, environment and
full command transfer, insufficient sizes, no-command/reentry, WOW rejection,
directory insufficient/success/one-shot behavior, DOS exit and
post-completion rejection.

## Explicit limits

- The original CSRSS capture transport, console records, duplicated wait
  handles and global first-VDM state are not recreated.
- `ExitVDM(TRUE, ...)`, `RegisterWowExec`, PIF, shared-WOW wakeups, child
  records and cross-process broker behavior remain the future WOW/broker
  owners.
- `VDMOperationStarted` is reached only by the excluded historical SoftPC
  executor path; no Bochs substitute is introduced by this Base VDM packet.
