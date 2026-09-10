# T228 S1 P5 — Generic CS-transition successor provenance admission

## Decision

The P4 retained transition is `1797:3AB8 -> 5A5F:5EF9`.  Independent comparison shows that `COMMAND.COM` has non-zero bytes at offsets `5EF9` and `9F51`, while P3 observed zeros only at the later terminal.  P4's existing `BX-MANTLE-075` copied record retains predecessor bytes and the new-stack bytes but not the initial bytes at the generic successor position.  Amend that already registered, explicit-provenance facility to retain one fixed 15-byte ordinary-RAM successor window at the same retained transition's current real-mode `CS:IP`.

## Boundary

This is one additional existing copied read only when the explicit provenance build is selected and a CS transition has already been observed.  It is selector-, service-, opcode-, address-, profile-, and image-blind: the source has no filter or interpretation and neither changes CPU state nor affects execution.  It uses the same checked ordinary-RAM primitive, fixed width, lifetime and copied-query ABI family as predecessor and stack evidence.  Default and scalar-history builds still compile no provenance read.

The output can distinguish only whether the target began with retained RAM bytes.  It cannot prove a COMMAND mapping, decode an instruction, select a BOP repair, or authorize an address-specific modification.

## Acceptance

Rebuild the complete explicit-provenance CPU5 derivative, run the existing focused provenance fixture and a fresh hash-locked Direct/Readonly observation.  Require a valid successor window in the paired run, byte-identical profile output, the unchanged default formal Ninja closure, an exception-register update, governance verification and `git diff --check`.