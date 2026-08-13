# T198 S24 P3 Terminal Snapshot 001

## Scope

P3 admits the registered `BX-MANTLE-069` fixture-only diagnostic.  It is
default-off and, after `cpu_loop` returns, copies CPU0's existing CS selector
and EIP into a fixed-width mantle-private record.  It does not change CPU
execution, read guest memory, decode BOPs, or expose a CPU object to `bx-vdm`.

## Build And Observation

Only `bx_ntvdm_finite_run.cc`, the native observation fixture, and the
current x64 adapter objects were rebuilt; the retained CPU5 core objects and
all Bochs core source remained unchanged.  MSVC x64 `/MT` output is:

```
t198-s23 status=0 observed-5011=1 observed-503b-resume=1 observed-stop=0 next=00:00 terminal=1:956b:0001
```

The fixture output was initially printed with an uninitialized local record
in a function-argument expression; P3 corrects that test defect by clearing
the record and retrieving it before formatting.  The corrected rerun has no
compiler warning and gives `956B:0001`.

## Interpretation

The halt occurs in a relocated guest code segment, not at an unmapped
zero-segment vector.  The source has two `cli; hlt` failure sites, but the
retained source tree has no exact NTIO link/listing map that associates the
runtime relocated segment `956Bh`, offset `0001h`, with either source label.
The result therefore narrows the question to original NTIO/sysinit image
layout and relocation; it does not authorize a memory value, device,
interrupt, or BOP change.

## Follow-Up

S24 remains open.  Its next evidence step is a read-only original image
layout/relocation audit for the emitted NTIO bytes and `sysinit1.asm`; no
additional runtime diagnostic is currently admitted.
