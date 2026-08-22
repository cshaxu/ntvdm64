# Proposal: OpenNT XMS Owner-Package Completion

## Purpose

The XMS mirror is code-complete except `52:09`; recover its remaining original
keyboard/INT 15 coordination for the declared profile.  The already-closed
A20, allocation/move/lock and empty-UMB failure contracts are not reopened.

## Queue relationship

This is supporting owner guidance for queue candidate 2, **PC keyboard, IVT
and INT 15h compatibility recovery**.  Its admission freezes `52:09` and the
corresponding keyboard/INT15 dependency rows; it does not reopen the other 11
locally closed XMS endpoints.
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
