# T228 S1 P3 — Terminal provenance admission

## Decision

The scalar P2 result establishes a stable terminal run at `5A5F:9F33..9F51`, but cannot determine whether the bytes are an original relocated Guest DOS region or data reached after an earlier transfer failure.  Admit one explicit-provenance derivative under registered `BX-MANTLE-085`.

## Boundary

After only an existing watchdog-budget return, the mantle may copy exactly 15 bytes at current real-mode CS:IP and 10 bytes at SS:SP.  Both reads use existing checked ordinary-RAM copy and fail closed.  The derivative is compiled only with `BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY=1` and `BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE=1`; ordinary and scalar-history graphs contain no terminal window.

No instruction decode, selector/service/BOP recognition, guest-image mutation, CPU/machine change, provider work, address filter, or trace-derived repair is admitted.  The CLI only formats copied bytes.

## Acceptance

A result may classify exactly one complete owner domain (Guest DOS relocation/control-flow, bx-core CPU compatibility, or bx-mantle machine lifecycle) and must reject BOP leaf repair selection.  Run the same hash-locked Direct/Readonly inputs after a focused derivative test and current-source full Ninja closure.
