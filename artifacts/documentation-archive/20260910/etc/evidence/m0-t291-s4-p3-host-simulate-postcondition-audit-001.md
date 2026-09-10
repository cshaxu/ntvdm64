# M0 T291 S4 P3 — `host_simulate` postcondition audit

## Question

Does the recovered `host_simulate(void)` spelling justify treating every
historical caller as the same modern CPU-loop operation?

## Procedure and observations

The selected MVDM host mirror and the current SoftPC/WOW adapter were searched
for direct calls. The resulting call-site groups and their required source
postconditions are recorded in the [postcondition ledger](../operations/m0-t291-s4-host-simulate-postcondition-ledger-001.md).

The source shows incompatible continuations: `wcall16.c` restores IP/stack and
reads its callback frame; DPMI expects monitor-frame effects; `demdasd.c`
expects BIOS disk carry/error results; mouse, video, ROM and floppy callers
expect device timing/IRQ/DMA effects; CCPU/SAS callers assume the executor
that Bochs replaces.

## Interpretation

The existing same-shaped `host_simulate(void)` is the permitted mechanical
entry only. It does not make a caller's source postcondition true. HS-01 has
the bounded S3 callback foundation; every other group remains with its stated
owner and unavailable/not-host-runtime disposition.

## Follow-up

S4 P4 closes the present WOW TD/TEB boundary. S5 may admit only the
source-evidenced command-broker and monitor re-entry groups; device, DPMI and
firmware groups retain their own owners.
