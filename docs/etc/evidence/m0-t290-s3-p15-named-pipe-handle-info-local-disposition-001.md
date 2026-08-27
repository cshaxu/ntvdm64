# M0 T290 S3 P15 — Original NetHandle pipe-information local disposition

The formal x86/x64 `vrnmpipe.c` fixture now compiles and executes unchanged
original `VrNetHandleGetInfo` and `VrNetHandleSetInfo` together with unchanged
original `vrputil.c` (`VrpMapLastError`/`VrpMapDosError`). Both functions use
the existing BP:BX session opaque identity and, where the source reads a
guest value, a prepared exact pointer scope.

For a local public Win32 named-pipe client, the source-matching
`GetNamedPipeHandleState` collection request fails. `VrNetHandleGetInfo`
retains that failure, invokes the original error mapper and returns the mapped
word in `AX` with carry set. The output guest span is not requested or
committed on this failure path.

`VrNetHandleSetInfo` retains an original observable peculiarity: it
pre-decrements `ParmNum` before testing it for 1 or 2, so its final
`SetNamedPipeHandleState` call receives no collection pointer and returns
success without changing a collection setting. The fixture proves its original
carry-clear result rather than correcting it.

Both formal Ninja architectures exit `0`. This does not claim remote
collection information support. No raw guest pointer, raw host handle,
selector ingress, async completion, broker or mapping manager is added.
