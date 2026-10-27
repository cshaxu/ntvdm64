# S10 shard 2: DPMI exception, mode and XMEM boundary cohort

## Scope and source ownership

The frozen/current hashes of `dpmi32/dpmiint.c`, `modesw.c` and `xmem.c`
were independently checked by re-diff against OpenNT. They are all changed
selected original product sources and remain partial. Their original DPMI
service table, interrupt/fault frames, guest mode and XMEM record decisions
remain source-owned; the CPU40 adapter supplies only described host/guest
boundary projections. This is not a whole-file equivalence proof.

- `dpmiint.c` retains `DpmiDispatchFault` / fault frame and original handler
  path. CPU40 projects locked PM-stack TIB fields into guest-addressable XMS
  storage (DIV-226), rejects unpublished IRQ vectors before constructing an
  interrupt frame (inline DIV-229), and handles one exact WOW INT 2Ah
  signature using `DpmiSetWowLdtEntry` rather than kernel VDM context.
  The original unhandled 32-bit IRET frame writes EFLAGS at offset 8 rather
  than overwriting CS at offset 4 (DIV-280). It also has opt-in trace calls
  for software interrupt, IRET and fault stack. A null protected selector
  lookup in `DpmiEmulateInstruction` returns FALSE to original fault handling.
  Verify nesting and failure separately; traces do not define CPU behavior.
- `modesw.c` enters/exits protected mode through original DOSX continuation,
  adapting immediate CCPU selector-cache loading, guest SAS RM_BIT,
  real-mode pseudo descriptors and the existing WOW page-domain leave/reenter
  owner. FastBOP support is the limited selector/offset carrier, not a general
  replacement for hardware or a scheduler. Verify PE/CS/SS ordering, return
  frame and page-domain lifetime on both entry routes and rejection; don't
  infer all guest mode transitions from successful product startup.
- `xmem.c::DpmiReallocateXmem` changes the provider result test from NTSTATUS
  to BOOL (DIV-272): a false result sets CF before changing the record; a true
  result updates address/length. Original XMEM record and allocator policy
  remain the owner. T420 S36 and S38 record real growth, forced relocation,
  failed-growth preservation and guest slot 09; they are historical evidence,
  not this review's fresh current-package verification.

T420 S38 records real fault/IRQ stack behavior, field publication, client
reuse and required-memory failure for PM stack information, and reports its
runtime boundary for DIV-280. T473 S2 records selected CPU40/DPMI link graph
and focused source fixtures, explicitly reserving production-caller lifecycle
proof. The S10 W31 DDE probe recorded vector 0d and a far-target mismatch;
that observation does not prove this DPMI frame implementation caused the
fault. No fixture was re-run for this review.

## Divergence registration and finite outstanding evidence

`modesw.c` marks pseudo-descriptor restoration as DIV-230, but README's
DIV-230 instead describes a separate HMA store witness. Inline DIV-229 in
`dpmiint.c` is the pre-publication IRQ check, while the README DIV-229 row
lists only the separate BOP FD mode route. Inline DIV-242 and DIV-243 mark
mode ordering in `modesw.c`; current README search found no corresponding
entries. These are per-behavior registration gaps/collisions; independently
reconcile owner and history, retain still-live behaviors, and avoid blind
renumbering. Other repeated identifiers may belong to separate cohorts.

The current registry now separately records the IRQ and real-mode cache
behaviors under their existing duplicated IDs and adds entries for 242/243.
This is metadata reconciliation, not an execution test or a change of either
implementation. Duplicate IDs remain explicit and must not be treated as
globally unique identifiers.

A focused current-artifact matrix should demonstrate: invalid/unpublished
interrupt selector returns without changing guest flags/frame; nested 16/32
fault/IRET frames preserve CS, EIP, EFLAGS and prior stack; mode exit/reentry
with original and rejected descriptors restores correct page-domain and
segment lifetime; exact INT 2Ah success/rejection preserves native status and
carry; XMEM failed growth keeps the old record/data and successful growth has
one owner. Tie each result to package identity and original guest inputs; keep
any unexecuted rows pending. No guest, source, runtime or tests were changed
by this static audit.
