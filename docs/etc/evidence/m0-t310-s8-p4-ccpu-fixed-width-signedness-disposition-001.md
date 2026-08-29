# M0 T310 S8 P4 — CCPU fixed-width signedness disposition

The whole-tree CCPU review identified 40 remaining C4018/C4146 records in
the address, basic, debug, interrupt, segment, stack, task-switch, transfer
and single-instruction implementation units.  Each has been read in its
original instruction context: segment/descriptor bounds, Intel flag or count
comparison, or fixed-width two's-complement bit arithmetic.

The source operands are the original fixed-width Intel state and address
types.  No record transports a native pointer, `HANDLE`, `size_t`, callback,
mapping-manager token or host resource across an x86/x64 boundary.  A signed
comparison or unary-minus warning may describe a historical instruction
semantic concern, but it is not a host-width compatibility defect.

All 40 records stay compiler-visible and source-unchanged.  The tracker marks
them `not-x86-x64-fixed-guest-signedness`, leaving any independent instruction
semantic review separate from the current x86/x64 repair scope. No warning
suppression, cast, adapter or overlay was introduced.
