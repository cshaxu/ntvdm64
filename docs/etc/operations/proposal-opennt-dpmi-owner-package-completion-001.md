# Proposal: OpenNT DPMI Owner-Package Completion

## Rebootstrap owner note

DPMI32 host source is a package/library family inside the canonical
`opennt-mvdm-host` mirror; guest DPMI remains in `opennt-guest-dos`.
References below to a former BOP owner or `bx-vdm` mean the source-proven DPMI
package plus current adapter/session contracts, not a production component.

## Purpose

Recover one complete OpenNT-derived DPMI profile (`53:xx`) rather than a
collection of protected-mode service stubs.

## Queue relationship

This is the admission plan for queue candidate 2, **OpenNT DPMI protected-mode
owner-package recovery**.  It may be admitted only after the protected-mode
machine dependency slice and every `53:xx` source/ABI/failure row are frozen.
## Boundary

Bochs owns protected-mode execution, descriptor checks and architectural
faults.  OpenNT DPMI/DOSX owns selector/session/memory/exception policy.
The declared adapters carry only copied session requests and results; they cannot expose
CR0, LDT internals or manufacture a DPMI server.

## Admission Plan

1. **S1 — audit:** select the DOSX/DPMI profile and map all `53:00..18`, its
   guest callers, host data layout, historical NT4 dependencies and failure
   terminals.
2. **S2 — import boundary:** create the original-source mirror/compile
   manifest and named OpenNT-shaped compatibility-facade ledger; no ingress or
   leaf route.  A facade preserves original call/macro/structure shape and
   maps only the source-proven session contract to bounded internals; it is
   not a license to rewrite the source body around a new provider.
3. **S3--S6 — coherent subcontracts:** recover startup/table, frame/IRET,
   XMEM/memory and PM-to-DOS translation as ordered package units; retain VDD
   and debug terminals explicitly until their owner packages exist.
4. **S7 — verification:** run protected/real transition, descriptor,
   exception, memory and termination regression as one suite, then one native
   profile run.

## Exit Rule

An x64-incompatible historical LDT call is a source-composition question, not
authorization for an adapter-owned protected-mode implementation. A DPMI BOP
is not code-complete merely because an adapter provider has equivalent-looking
behavior: its original translation unit must be composed directly or through
the recorded OpenNT-shaped facade, or the evidence must name the exact
unavailable dependency and an explicit temporary migration disposition.
Any project-defined DPMI interface or algorithm requires a registered recovery
exception first, including the rejected direct/facade routes, minimal ABI,
retained source-visible behavior and planned migration disposition.
