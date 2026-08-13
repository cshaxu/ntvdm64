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

## Corrected Interpretation

The fixture's NTDOS input is deliberately `0x6cd2` bytes with first byte
`HLT`; it is published by the source-derived `50:11` transaction.  Thus the
terminal `956B:0001` proves that guest execution reached the fixture's
controlled NTDOS endpoint after `50:3B`; it is not evidence for a relocated
NTIO fatal path.  The prior static discussion of `sysinit1.asm` HLT labels is
retained only as an unrelated source fact.

## Follow-Up

S24 closes with an intentional mock-NDTOS terminal.  S25 must replace that
mock with the immutable source-built NTDOS input before interpreting any
later guest control flow or admitting another BOP service.
