# T228 S1 P4 — Terminal CS-transition provenance admission

## Decision

P3 proves the terminal instruction window is zero-filled RAM.  Reuse the already registered `BX-MANTLE-075` facility, without modifying it: after the same existing watchdog budget return, copy its single retained latest CS-transition provenance record through a mantle-owned terminal observation.

## Boundary

The derivative remains explicitly compiled with history and provenance enabled.  The new copied terminal wrapper contains only the existing previous/current scalar records, its already fixed 15-byte predecessor window, and its already fixed 10-byte stack window.  It neither performs another RAM read nor modifies CPU execution.  Default and scalar-history graphs return unavailable; CLI output is diagnostic-only.

No selector, BOP/service, guest-image, address/opcode branch, decode, device, CPU semantic or host-profile behavior is admitted.  A result can only name the predecessor control-transfer owner domain; it cannot create a leaf repair.

## Acceptance

Use the current full provenance Ninja graph, focused fixture and the same hash-locked paired Direct/Readonly invocation.  Record the exact copied transition and whether it reaches the zero-filled terminal region; then map the predecessor byte window against original source before selecting a repair package.
