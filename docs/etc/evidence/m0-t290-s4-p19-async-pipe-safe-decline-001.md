# M0 T290 S4 P19 — original async named-pipe safe decline

## Result

The original `VrReadWriteAsyncNmPipe` body now has formal x86/x64 evidence
for the only safe current result: it parses a fixed-layout DOS asynchronous
request and returns through its existing source failure path when the
same-shaped worker facade is unavailable.

## ABI preservation

`MVDM-SUPPORT-DIV-004` keeps every far address in
`DOS_ASYNC_NAMED_PIPE_STRUCT` as four bytes.  On x64 these values are not host
pointers: original `READ_FAR_POINTER` decoding is permitted only while the
existing bounded Redirector pointer scope is active.

## Regression

The existing formal `t290_s3_vrnmpipe_name_fixture` is extended for S4.  It
registers a real local named pipe with the original source open-list, provides
a correctly scoped read request and calls the original `VrReadWriteAsyncNmPipe`
with `ANP_READ`.  The current same-shaped cdecl thread facade returns
`ERROR_CALL_NOT_IMPLEMENTED`; the original source closes its wake event and
returns `ERROR_NOT_ENOUGH_MEMORY` with carry set before allocating/queuing a
request or starting I/O.

Run on both architectures:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T290S3VrNamedPipeNameNinja.ps1 -Architecture <x86|x64> -RepositoryRoot .
ninja -C build/M0-T290/S3/vrnmpipe-name-<x86|x64> -v
build/M0-T290/S3/vrnmpipe-name-<x86|x64>/bin/t290-s3-vrnmpipe-name-fixture.exe
```

Both runs passed on 2026-08-27.

The same fixture initializes the original cancellation critical section and
calls `VrCancelPipeIo` for an empty request queue after the declined request.
Both architectures prove that this source cancellation path leaves no pending
request, guest write, callback or host handle behind.

## Boundary retained

This is deliberately not a worker-success claim.  The source retains buffer,
result, semaphore and ANR locations past the call and completes ANR requests
by requesting a physical IRQ.  S4 may not retain raw guest pointers in a
worker, synthesize a callback or deliver a physical IRQ.  A future safe
completion record must first have an admitted selector-blind mechanical
delivery request; until then the original failure direction is the only
correct current product result.
