# Proposal: bx Machine, BIOS and Selector Owner-Package Completion

## Purpose

Provide the governing plan for individually admitted BOP-reachable SoftPC/BIOS
machine components: reset/lifecycle, ROM/RAM, default port-space, PIC/IRQ,
FPU, CMOS, BIOS helpers and typed mechanical requests.  It is deliberately
not permission to enable those components as one product-sized machine.

## Queue relationship

This is supporting admission guidance for queue candidate 2 (keyboard/IVT/INT
15) and candidate 8 (remaining evidence-admitted PC device expansion).  The
separate first candidate owns only BOP 12h conventional-memory reset/query;
it is governed by `proposal-softpc-bios-conventional-memory-recovery-001.md`.
No candidate may consume this plan until its applicable BOP-… /
BOP-DEPENDENCY-… rows have been frozen.
## Boundary

`bx-core` owns adopted Bochs CPU, memory, exceptions and devices;
`bx-mantle` owns minimal machine assembly.  `bx-vdm` may request opaque typed
mechanics but never identifies DOS, BOP, OpenNT or SoftPC semantics inside
Bochs.  This proposal is therefore a companion to, not an adapter BOP family.

## Admission Plan

1. **S1 — audit:** map one selected machine component and its required guest
   callers to original SoftPC/OpenNT evidence, native owner, lifecycle,
   feature admission and negative tests.
2. **S2 — code complete:** complete only that component's native lifecycle and
   typed request surface; register every unavoidable Bochs intrusion before
   applying it.
3. **S3 — verification:** run that component's reset/port/interrupt/device
   regressions and one bounded native guest observation.

## Exit Rule

No selector/service recognition or OpenNT/DOS policy may enter Bochs; a
machine capability is complete only with its lifecycle and negative boundary
tests.
