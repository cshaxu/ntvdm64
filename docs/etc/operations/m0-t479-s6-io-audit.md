# T479 S6 VM I/O implementation audit

Baseline572d854a3. S5 is delivered; this is the finite S6 work inventory,
not an acceptance report. Original guest bytes remain immutable.

[Operation inventory](m0-t479-s6-io-inventory.json) enumerates all47 live wire
operations and actual source-reference locations. Seventeen have no explicit
case in the current VM provider/application/text dispatch chain and reach its
unsupported default. A reference is not proof that an operation is complete:
VIDEO_BEGIN accepts only same-sized text, WINDOW_RECT accepts only full-grid
identity changes, modes reject processing flags, and input currently has only
provider stimulus acceptance. All47 require producer and guest-side review.

## Finite implementation groups

1. Input ownership: connect actual guest keyboard/mouse/focus to the active
   VM continuation, preserve exclusive consumption, returned receipts, mode
   semantics and cancellation. Keep guest/VMM state access on its owning CPU.
2. Text operations: WRITE, fill and scroll need actual Console semantics;
   reuse existing text primitives where possible. Do not turn unsupported
   operations into success or re-enable incompatible stream output.
3. Geometry and fonts: actual guest mode/grid/font authority, resize and
   viewport application, font queries, codepage and snapshot provenance.
   Existing text-configuration cache is an acknowledged projection, not a
   second authoritative guest state.
4. Graphics/display: implement actual graphics transport and guest rendering,
   mode changes and VIDEO_TEXT; preserve original device/VMM ownership.
5. Window metadata: title versus handoff caption, layout, pointer/clip and
   keyboard layout. Do not answer using the unrelated outer host window.
6. Publication/lifecycle: exact serial acknowledgement, final frame before
   handoff, abort/disconnect/retirement/reuse and cancellation of blocked I/O.
7. Nested acceptance: independent text task chains and nested multitasking
   payloads, no ancestor takeover, real input/output and resource retirement.

For each group, first trace existing producers and original guest services,
then implement as one bounded batch with focused and actual guest evidence.
New semantics concessions require explicit disposition. S6 cannot close merely
because MEM or a native shell runs. Win95 remains the subsequent S4 continuation.
