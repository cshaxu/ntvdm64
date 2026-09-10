# T228 S1 P21 — Bounded Software-Interrupt CLI Observation Admission

## Decision

Consume P20 only through the existing diagnostic-CLI lifecycle: a diagnostic
build may accept `--observe-software-interrupts`, configure the mantle ring
before the already-existing worker thread, copy and print its fixed-width
records only after that worker joins, then clear the ring. The normal product
build neither recognizes the option nor arms the ring.

## Boundary

This is a CLI presentation of P20 facts, not a new adapter or BOP mechanism.
It does not inspect guest RAM, parse a selector/service, choose a provider, or
change Direct/Readonly profile policy. The worker and CPU loop retain their
existing contracts; the CLI only configures and later copies a mantle-owned
record list. A finite paired Direct/Readonly run is used to correlate the
records with P19's already-complete COMMAND/NTDOS owner map.

## Recovery Ledger

1. Original source reuse: not applicable; no OpenNT host behavior is being
   recovered.
2. Smallest adapter/shim: reuse the established guarded CLI observation
   lifecycle used by terminal history and position.
3. External-code intrusion: none beyond registered P20 `BX-CORE-088`.
4. New behavior: one diagnostic-only flag and fixed-width formatter.

## Acceptance

The diagnostic graph must show the option absent from the default graph,
accepted in both Direct and Readonly, and emit a bounded record list with the
same mode-independent terminal classification. The result may attribute only
the pre-mapped complete COMMAND/NTDOS transient/MCB owner domain. It cannot
create a BOP leaf repair or change a guest image.