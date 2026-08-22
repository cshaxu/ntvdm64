# Proposal: OpenNT DPMI Owner-Package Completion

## Purpose

Recover one complete OpenNT-derived DPMI profile (`53:xx`) rather than a
collection of protected-mode service stubs.

## Queue relationship

This is a supporting owner-workstream plan, not an independent queue position.
The dependency-driven queue admits it only inside candidate 3,
**External compatibility, machine and legacy-composition BOP closure**, after
its applicable BOP-… / BOP-DEPENDENCY-… rows have been frozen. It must
follow the tracker’s Win32/x64 compatibility decision rule and may not use its
former family position to bypass lower-dependency work.
## Boundary

Bochs owns protected-mode execution, descriptor checks and architectural
faults.  OpenNT DPMI/DOSX owns selector/session/memory/exception policy.
`bx-vdm` carries only copied session requests and results; it cannot expose
CR0, LDT internals or manufacture a DPMI server.

## Admission Plan

1. **S1 — audit:** select the first DPMI guest profile and map every reached
   `53:xx` service, LDT dependency, exception/IRET, memory, transition and
   cleanup contract.
2. **S2 — code complete:** recover the whole selected DPMI composition by
   original-source reuse or smallest capability seam; explicitly record any
   profile exclusion before coding.
3. **S3 — verification:** run protected/real transition, descriptor,
   exception, memory and termination regression as one suite, then one native
   profile run.

## Exit Rule

An x64-incompatible historical LDT call is a source-composition question, not
authorization for an adapter-owned protected-mode implementation.
