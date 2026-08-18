# T228 S1 P31 — COMMAND transient arena persistence source audit

## Question

Does the original OpenNT guest contract permit an implementation to clear the
transient storage between `EndInit`'s temporary allocation and `LodCom`'s
later allocation, or does the observed zero transient instead identify a
different owner boundary?

## Inputs

- `src/opennt/base/mvdm/dos/v86/cmd/command/rdata.asm:623..686`
- `src/opennt/base/mvdm/dos/v86/cmd/command/command2.asm:292..456`
- `src/opennt/base/mvdm/dos/v86/doskrnl/dos/alloc.asm:337..431` and
  `490..529`
- [P26 transition result](../evidence/t228-s1-endinit-lodcom-transition-provenance-result-001.md)
- [P29 correction](t228-s1-endinit-reentry-mechanics-correction-001.md)

## Original Contract

`EndInit` obtains the largest available DOS block, chooses the top
`TrnSize` paragraphs, reverse-copies the transient with `STD; REP MOVSB; CLD`,
and then releases the allocation. Its comment explicitly says that resident
COMMAND will allocate the block again and find the transient intact.

`LodCom` then obtains the largest allocation, retains the prior transient when
the segment agrees, or copies it from the prior segment when it differs. Only
after that does it checksum the transient and use the original `LoadCom`
retry path if the checksum fails.

The original NTDOS arena implementation supports that contract. `$DEALLOC`
only validates the MCB and changes its owner word to free; `$ALLOC` selects or
splits MCBs, sets header size/signature/owner, and returns the data segment.
Neither routine clears the returned data payload. There is no `STOS`/payload
clear path in either allocation body.

## Interpretation

The zero successor observed by P26 is not compatible with treating cleared
arena payload as normal OpenNT NTDOS allocation behavior. It also does not
authorize a BOP provider, adapter MCB/PSP synthesis, or a Bochs
instruction-semantic adjustment:

- the transient move and all arena transitions remain guest COMMAND/NTDOS
  behavior;
- the existing CPU5 reverse-copy fixture already excludes the generic
  `STD; REP MOVSB; CLD` mechanic; and
- Direct and Readonly remain byte-identical, excluding the mutation-profile
  backend as the differentiator.

The still-unproven fact is narrower: whether the initial guest copy reaches
the source-selected temporary range and whether the subsequent guest arena
allocation selects/reuses that range. Current fixed terminal provenance only
observes the eventual zero target, not those two earlier mechanical events.

## Recovery Ladder And Follow-up

The first two rungs remain original guest source and its existing source-built
image; no adapter substitution is admissible. Before any repair, admit one
default-off, selector-blind mechanical observation only if existing records
cannot establish the initial-copy and reallocation facts. It must record no
BOP, DOS, COMMAND, address, or opcode predicate in product behavior, and must
not change guest memory or execution. Its only allowed outcome is attribution
to the complete guest COMMAND/NTDOS transient/arena owner package.
