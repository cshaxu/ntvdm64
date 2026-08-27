# M0 T290 S4 closure — mailslot and asynchronous completion

## Closed scope

- Original `vrmslot.c` is locally composed on formal x86/x64 fixtures.
  `57:09..0e` source bodies cover make, info, write, read, delete, original
  peek decline and process/PDB teardown.  The proof includes stale-handle
  rejection after both explicit delete and original termination cleanup.
- Mailslot guest `WORD` identities use the existing session host-resource
  mapping only.  Native private record pointers and native HANDLEs do not
  enter guest memory.
- Original asynchronous named-pipe request parsing retains its fixed four-byte
  16:16 ABI on both widths.  The current source-shaped cdecl worker facade
  declines creation; the original body returns its existing carry/error branch
  before it queues a request, starts I/O, retains a guest alias or requests an
  ANR callback.
- The original empty-queue `VrCancelPipeIo` path is executed on both widths.
  Since the worker-creation failure prevents any request from existing, this
  is the complete meaningful cancellation/disconnect negative proof for the
  admitted state: no disconnected native handle can be touched after an I/O
  never starts, and no completion record exists to become stale.

## Explicit transfer

`VrQueueCompletionHandler`, `VrRaiseInterrupt`, `VrNmPipeInterrupt`,
`VrHandleAsyncCompletion`, `VrDismissInterrupt` and
`VrEoiAndDismissInterrupt` retain original source ownership, but physical IRQ
delivery is outside T290/S4's admitted selector-blind mechanical contract.
ANR-bearing async completion therefore remains source-defined unavailable.
This record does not claim a guest callback, IRQ, BOP ingress or a second
broker.

## Evidence

- [S4 source/ABI audit](m0-t290-s4-async-mailslot-source-abi-audit-001.md)
- [P18 original mailslot composition](m0-t290-s4-p18-original-mailslot-composition-001.md)
- [P19 async safe decline](m0-t290-s4-p19-async-pipe-safe-decline-001.md)

All formal commands and both architecture results are recorded in P18 and
P19.  T290/S5 now owns the remaining NetAPI/NetBIOS/DLC/window/mode provider
groups; S6 owns whole-family reconciliation and bounded integration.
