# Proposal: OpenNT XMS Owner-Package Completion

## Purpose

Recover the complete OpenNT XMS family (`52:xx`) for one declared profile,
including A20, extended-memory allocation/move/lock, UMB and the relevant
INT 15 coordination.

## Boundary

Bochs owns CPU/memory/A20 machine mechanics through `bx-core` and
`bx-mantle`; OpenNT XMS owns XMS policy and contracts.  The adapter may not
become an allocator, A20 controller or BIOS substitute.

## Admission Plan

1. **S1 — audit:** map every `52:xx` endpoint and its A20/UMB/INT 15
   prerequisites to original source, native owner, ABI and failure rule.
2. **S2 — code complete:** connect the complete XMS provider to admitted
   native mechanics, retaining profile-specific unavailable behavior only
   where source and capability evidence require it.
3. **S3 — verification:** run allocation/move/lock/A20/UMB and failure
   regressions, followed by one bounded source-built integration run.

## Exit Rule

No XMS service is declared successful merely because a memory buffer exists;
the original lifecycle, errors and machine-owner boundary must be covered.
