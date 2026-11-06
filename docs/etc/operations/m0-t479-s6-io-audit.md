# T479 S6 VM I/O implementation audit

Reconciled against delivered d3bd17b6b. This is the finite S6 work inventory,
not an acceptance report. Original guest bytes remain immutable.

[Operation inventory](m0-t479-s6-io-inventory.json) enumerates all47 live wire
operations and current provider-reference locations. Fifteen have no explicit
dispatch and reach unsupported default; the previous17 count predates the two
fill operations. The32 remaining entries have dispatch references, not blanket
semantic acceptance. VIDEO_BEGIN still accepts only same-sized text, WINDOW_RECT
only full-grid identity changes, and modes reject processing flags. Source
references and positive payload coverage must remain separate.

## Input gap traced after mouse repair

The delivered mouse/VM repair proves the actual Windows pointer, DOS command
entry and noninteractive CLS/VER/DIR sequence. It does not prove input to a
running native interactive child. Source sweep of src and tests finds only the
definition/declaration of mvdm_vm_executor_key, and its internal call to
mvdm_vm_io_key. No production or test caller enters executor_key. The current
addon command_return waits on a counted VMM semaphore with virtual interrupts
serviced, but supplies no keyboard records to the host queue. Notification BOP
operations0..15 have no input-delivery operation. Therefore READ_INPUT's queue
mechanics cannot substitute for the absent guest producer.

The bounded input repair must connect guest-selected VM input to the exact
live continuation, retain key-up/down, repeat and modifier meaning, and prevent
both guest BIOS and host child from consuming the same event. Retirement and
channel cancellation must invalidate delivery. Do not read the outer host
Console or route by foreground native HWND: neither identifies the guest VM.
Existing command-entry and mouse tests remain valid within their own scope.
VMM/VKD service choice and actual integration must be proven before runtime
changes; no polling or synthetic successful input is authorized by this audit.

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
