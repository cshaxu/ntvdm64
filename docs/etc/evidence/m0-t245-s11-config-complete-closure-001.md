# M0 T245 S11 — Config-complete source-body closure

OpenNT `nt_bop.c:MS_bop_E` is mirrored as one body: when `AL==0`, it calls
the original-empty `UMBNotify(0)` and imported `demDasdInit()` in that order.

The new bx-vdm scoped-body helper is selector-blind: provider code recognizes
`C4 C4 5E`; the helper only scopes copied CPU state, invokes the body, and
uses an explicit three- or four-byte resume width.  No Bochs code changed.

Formal `M0-T245-S11/formal-r3` built the 265-edge closure.  The focused
fixture bound an empty synthetic drive snapshot, dispatched AL=0 at `0x1234`,
and exited zero after verifying typed resume at `0x1237` with no GPR delta.
This proves local source/ABI closure only; it is not a guest-continuity or
floppy-device claim.
