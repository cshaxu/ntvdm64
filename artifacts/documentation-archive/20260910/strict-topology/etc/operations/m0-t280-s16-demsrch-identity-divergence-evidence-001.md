# M0 T280 S16 — `demsrch.c` host-identity divergence evidence

## Applied source-local divergences

- `dos/dem/dosdef.h` now declares the two DOS-resident find-list fields as
  `ULONG`, preserving their original x86 four-byte layout on both host
  architectures. They are opaque host-resource identities, not native
  pointers.
- `dos/dem/demsrch.c` publishes the native `PFFINDLIST` before storing its
  surrogate, resolves before path/FCB validation, and reverse-looks-up then
  releases before `FreeFFindEntry` frees the native object.
- The source's volume-label look-ahead remains optional: failed publication
  frees only that look-ahead entry and returns the original volume result.
  The ordinary path and FCB path retain the original out-of-resource failure
  family: cleanup, zeroed persisted pair,
  `SetLastError(RtlNtStatusToDosError(STATUS_NO_MEMORY))`, then their existing
  return/client-error route.

`FFindId` remains unmodified numeric data. There is no `STOREDWORD` call that
receives a native `PFFINDLIST`, and no guest-memory operation in this work.

## Formal verification

MSVC `/MT /W4` through disposable Ninja roots produced:

```text
x64: New-T280S12DemObjectObservationNinja.ps1; ninja demsrch
x86: New-T280S12DemObjectObservationNinja.ps1; ninja demsrch
x64/x86: New-T280S16DosFindLayoutNinja.ps1; ninja; fixture.exe
```

Both syntax compositions passed. Both layout fixtures compiled, linked and
exited zero, proving `SRCHDTA` offsets `0/4`, size `43`, and `DIRENT` offsets
`12/16`, size `32`. The newly introduced `uint32_t` facade outputs have no
pointer-width/type warnings.

The x64 compile retains pre-existing unresolved pointer/HANDLE warnings in
other `demsrch.c` operations (directory buffers, device handles and pathname
helpers). They are explicitly not suppressed or altered here; S17 audits them
as one source-owner surface before any further code changes.

No DEM provider, BOP route, guest trace or full object link is enabled.
