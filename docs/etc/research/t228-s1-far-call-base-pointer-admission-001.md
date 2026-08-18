# T228 S1 P6 — Generic far-call base-pointer provenance admission

## Decision

P5 proves that the latest generic CS transition transferred directly into zero-filled ordinary RAM.  Its predecessor begins `FF 9E 4A 97`, whose x86 interpretation is outside bx-core: a 16-bit indirect far call using the stack-default effective address `SS:[BP+974Ah]`.  Existing scalar history retains `SS:SP`, but not low `BP`; therefore it cannot locate the runtime pointer storage whose already-observed content selected `5A5F:5EF9`.

Add low 16-bit BP to every existing default-off copied instruction-history position record at its existing capture site.  This is scalar state only.  It neither reads guest RAM nor recognizes, decodes, filters, or executes an opcode; the evidence document, not bx-core, performs the source interpretation.

## Boundary

The history record remains fixed-width and copied.  The default build retains the empty history macro; history must still be explicitly selected.  The explicit provenance derivative may combine the generic previous/current scalar records with its existing ordinary-RAM windows after a CS change.  No new RAM range, address calculation, pointer, callback, CPU mutation, BOP/service, device, firmware, host policy, image term or execution decision is introduced.

## Acceptance

Rebuild a full CPU5/Pentium-MMX MSVC x64 `/MT` provenance derivative; run the focused history/terminal fixture and one hash-locked Direct/Readonly pair; capture the previous/current BP values; calculate the historical `SS:[BP+974Ah]` location only in written analysis; retain Direct/Readonly parity.  A separate unchanged default formal Ninja graph must compile and pass its focused fixture.  Update the exception register, governance inventory and status before commit.