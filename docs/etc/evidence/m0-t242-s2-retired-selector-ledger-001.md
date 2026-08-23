# M0 T242 S2 Retirement Of Selector-Only Guest Lifecycle Observers

## Question

Can the retained `guest_exec_lifecycle_*_v1` modules serve as evidence for
original NTDOS EXEC/PSP/parent-return execution?

## Inputs

- The two `src/bx-vdm/bx_ntvdm_guest_exec_lifecycle_*_v1` modules and their
  fixtures.
- Their CLI flags and full-Ninja manifest membership.
- T242 S1 source/ABI audit.

## Observation

The modules accepted only a synthetic resumed sequence of three selector
windows: `50:36`, `50:3C`, then `54:0B`. They neither identify `NTDOS.SYS`,
read guest PDB/arena/JFN/vector bytes, nor prove execution of `$Exec`,
`$Exit`, or `$Abort`. Their `consider` functions had no real ingress caller;
the CLI merely exposed their default-off output.

## Decision

They are retired rather than reused for S2. The removal includes both modules,
their headers/fixtures, the old integration-launch script, CLI switches and
the formal Ninja source/fixture entries. Historical evidence that refers to
the retired observer remains history; it is not a current implementation
route.

## Verification

A fresh external formal Ninja graph at `build/M0-T242-S2/formal-r1` compiled
and linked `bin/ntdos64-native.exe` successfully (255 edges, exit `0`). A
source-tree sweep finds no remaining production/test/tool reference to the
retired observer names or CLI options.

## Follow-Up

S2 must replace this pseudo-ledger only with a checked source-built guest
state observation tied to an original NTDOS edge. It must not resurrect a
selector-order proxy.
