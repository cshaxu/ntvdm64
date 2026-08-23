# Proposal: OpenNT DPMI Owner-Package Completion

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
`bx-vdm` carries only copied session requests and results; it cannot expose
CR0, LDT internals or manufacture a DPMI server.

## Admission Plan

1. **S1 — audit:** select the DOSX/DPMI profile and map all `53:00..18`, its
   guest callers, host data layout, historical NT4 dependencies and failure
   terminals.
2. **S2 — import boundary:** create the original-source mirror/compile
   manifest and named compatibility-shim ledger; no ingress or leaf route.
3. **S3--S6 — coherent subcontracts:** recover startup/table, frame/IRET,
   XMEM/memory and PM-to-DOS translation as ordered package units; retain VDD
   and debug terminals explicitly until their owner packages exist.
4. **S7 — verification:** run protected/real transition, descriptor,
   exception, memory and termination regression as one suite, then one native
   profile run.

## Exit Rule

An x64-incompatible historical LDT call is a source-composition question, not
authorization for an adapter-owned protected-mode implementation.
