# M0 T290 S3 closure — Redirector lifecycle, synchronous pipe and local COMMAND group

## Completed original source bodies

- `vrinit.c`: `VrInitialize` retains the source-defined VDD-unavailable
  result; `VrUninitialize` retains carry-clear teardown.
- `vrmisc.c`: `VrTerminateDosProcess` retains its PDB forwarding order and
  `VrUnsupportedFunction` retains its error result.
- `vrnmpipe.c`: original classification, open-list, synchronous read/write,
  `VrGetNamedPipeInfo`, `VrGetNamedPipeHandleState`,
  `VrSetNamedPipeHandleState`, `VrPeekNamedPipe`, `VrTransactNamedPipe`,
  `VrCallNamedPipe`, `VrWaitNamedPipe`, `VrNetHandleGetInfo` and
  `VrNetHandleSetInfo` are locally composed. `vrputil.c` provides its original
  error mapping body.
- `cmdredir.c`: original local standard-handle classification/cleanup and the
  non-pipe `cmdGetStdHandle` return path execute through session identities;
  no second broker was created.

## Boundary and proof

The formal external Ninja matrix ran all four fixtures on x86 and x64:
`vrinit-failure`, `vrmisc`, `vrnmpipe-name`, and `cmdredir`. Every fixture
exited `0`; the focused recipes use original source bodies and reject
`src.old`, `adapter-bochs` and `adapter-mvdm-host-in` inputs.

Native handles and host records are always session-owned opaque identities.
Guest bytes exist only within checked leases. The two registered mirror
changes are `MVDM-SUPPORT-DIV-002` (packed CallNamedPipe physical far fields)
and `MVDM-HOST-DIV-020` (COMMAND record/handle identity crossings).

## Explicit transfer

- S4 owns `vrmslot.c`, `VrReadWriteAsyncNmPipe`, async pipe cancellation and
  completion-record lifecycle. It may not create a raw guest callback or
  physical interrupt delivery contract.
- S5 owns NetAPI, NetBIOS, DLC/window and mode groups.
- S6 owns `VrDispatch`, all `57:00..31` family reconciliation and the complete
  `cmdmisc.c`/`cmdexec.c` producer-consumer handoff for the COMMAND redirection
  record identity.

No Redirector selector is enabled by S3.
