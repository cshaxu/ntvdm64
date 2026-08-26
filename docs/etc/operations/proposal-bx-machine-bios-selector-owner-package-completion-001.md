# Proposal: bx Machine, BIOS and Selector Owner-Package Completion

## Supersession

This retained supporting record describes the pre-rebootstrap component model
and queue. It is not an active queue candidate. Its historical `bx-*` names
and candidate numbers do not govern the current source layout; see the
[rebootstrap program roadmap](proposal-opennt-mvdm-package-mirror-rebootstrap-001.md)
and its five queued candidates for the successor admission order.

## Purpose

Provide the governing plan for individually admitted BOP-reachable SoftPC/BIOS
machine components: reset/lifecycle, ROM/RAM, default port-space, PIC/IRQ,
FPU, CMOS, BIOS helpers and typed mechanical requests.  It is deliberately
not permission to enable those components as one product-sized machine.

## Queue relationship

This was supporting admission guidance for a former queue candidate (the reached
keyboard/IVT `BOP-5F` handoff) and candidate 7 (remaining evidence-admitted
PC device expansion).  The former is constrained to
`BOP-DEPENDENCY-104` / `BOP-TOP-5F`; it is not permission to enable any other
machine component.  No candidate may consume this plan until its applicable
`BOP-…` / `BOP-DEPENDENCY-…` rows have been frozen.
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
