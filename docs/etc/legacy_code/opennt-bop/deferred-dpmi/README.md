# Deferred DPMI source package

This directory retains the complete pre-T271 DPMI experiment as review and
provenance material. It is not a production component, include root, formal
Ninja source input, runtime provider, or current test input.

The package contains two categories that must be admitted together by a later
DPMI owner-package task:

- OpenNT-shaped `dpmi32.c` / `dpmiselr.c` material and its headers, sourced
  from `base/mvdm/dpmi32/` in the pinned OpenNT trees.
- The former source-derived startup/descriptor seams. Their original
  `BOP-DIV-067` through `BOP-DIV-079` records remain historical evidence, not
  approval to compile a partial DPMI provider.

Promotion requires an owner-approved protected-mode, LDT/IDT, exception and
guest-linear-memory closure. Until then, the BOP tracker remains authoritative
that no `53:xx` provider is active.
