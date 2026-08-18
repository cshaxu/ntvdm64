# Proposal: bx Machine, BIOS and Selector Owner-Package Completion

## Purpose

Complete BOP-reachable SoftPC/BIOS selector mechanics as a native machine
package: reset/lifecycle, ROM/RAM, default port-space, PIC/IRQ, FPU, CMOS,
BIOS helpers and typed mechanical requests.

## Boundary

`bx-core` owns adopted Bochs CPU, memory, exceptions and devices;
`bx-mantle` owns minimal machine assembly.  `bx-vdm` may request opaque typed
mechanics but never identifies DOS, BOP, OpenNT or SoftPC semantics inside
Bochs.  This proposal is therefore a companion to, not an adapter BOP family.

## Admission Plan

1. **S1 — audit:** map all BOP-reachable machine/BIOS selectors and required
   guest callers to original SoftPC/OpenNT evidence, native owner, lifecycle,
   feature admission and negative tests.
2. **S2 — code complete:** complete the smallest native machine closure and
   typed request surface; register every unavoidable Bochs intrusion before
   applying it.
3. **S3 — verification:** run full machine-family reset/IRQ/FPU/port/ROM
   regressions and one bounded native guest observation.

## Exit Rule

No selector/service recognition or OpenNT/DOS policy may enter Bochs; a
machine capability is complete only with its lifecycle and negative boundary
tests.
