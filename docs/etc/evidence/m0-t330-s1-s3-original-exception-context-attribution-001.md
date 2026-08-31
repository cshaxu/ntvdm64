# M0 T330 S1–S3 — Original exception-context attribution

Date: 2026-08-31

Status: closed at a source-defined unavailable filter boundary; no exception
continuation, device result or guest behavior changed.

## Original contract and report seam

The selected original `VdmUnhandledExceptionFilter` is the common SEH filter
used by original `ntvdm.c`, timer, event, thread and communications worker
wrappers. Its unchanged order is:

```text
copied diagnostic (opt-in only)
  -> SuspendTimerThread
  -> UnhandledExceptionFilter
  -> original terminate/resume result
```

The report companion is entered only after the original filter has already
received `EXCEPTION_POINTERS`. It copies the exception code/address, native
IP/SP/FP/CS, access parameters and a bounded readable host-stack window to
the selected observer console and to the observer-only report path. It does
not retain a pointer, change the filter result, resume a CPU frame, alter
guest state, change timer behavior or create a continuation.

## Formal build evidence

The changed adapter object, library and final product linked through the
formal selected CPU40 graph on both host architectures:

```text
ninja termination-x86  -> 3 actions, passed
ninja termination-x64  -> 3 actions, passed
```

The x64 result is a build closure only. T330 makes no x64 runtime claim.

## Fixed-container observation

A new immutable short-root container was staged as `build/r330` from the
formal x86 product and the existing source-selected DOS/firmware media. It
does not modify the earlier `build/r327` evidence container. The fixed,
console-owning, non-debug observer ran exactly:

```text
build/tools/console-startup-observer.exe
  build/r330/original-softpc-process.exe
  build/r330
  artifacts/research/m0-t330-s3-exception-context.txt
```

It exited within the fixed eight-second interval with `0xc0000005`. Its
console snapshot records reached original BOP ingress through `50:11`,
`50:3B`, `50:0F`, `50:1B`, and `54:05`. No
`MVDM-ORIGINAL-EXCEPTION` line and no `.exception.txt` report were produced.
The observer supplies the report path before process creation, therefore this
absence proves that this terminal exception did **not** reach the selected
original `VdmUnhandledExceptionFilter` report call.

## Disposition

The exact owner reached by the available source-defined contract is the
unavailable boundary before the original filter receives an exception record;
the filter cannot attribute a context it never obtains. There is no admissible
T330 behavior change: installing a catch, inventing a continuation, or
changing FDC, ROM, vector, BOP or CPU behavior would only hide the
unattributed exception.

The copied report seam remains opt-in product diagnostics for future failures
that do traverse the original filter. The existing guest-execution roadmap
continues from the already observed original owner packages; no new frame or
exception workaround is admitted by this task.
