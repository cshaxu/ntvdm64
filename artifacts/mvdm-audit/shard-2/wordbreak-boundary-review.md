# EDIT word-break buffer boundary

The [isolated fixture](parallel-review-fifth-wordbreak-probe.c) links the
selected original `wcall32.obj` and `walloc16.obj`, while a controlled guest
address mapping and callback replace the rest of the system. The [runner](parallel-review-fifth-wordbreak.ps1)
records exact compiler command, object/source and release hashes, and results.
It does not execute a guest, alter the product or access the installed session.

`W32EditNextWord` requests `malloc16(cbEditText)` and then copies a terminating
NUL with `lstrcpy`. With the fixture's two adjacent 256-byte allocations and
the first block freed for reuse, 255 characters preserve the second block's
sentinel `0x5a`, while 256 characters replace it with `0x00`. The strict
invariant returns failure based on the measured sentinel; its nonzero result
is an expected counterexample, not a passing compatibility test.

The retained EDIT source passes its character count to the callback and its
ANSI path can derive the count from string length. This source chain and the
isolated counterexample do not prove that an installed Win16 application has
reached this layout, nor establish all mapping failures or callback lifetimes.
Review exact guest callback length and overflow handling before implementing a
minimal host-side repair; keep the original guest unchanged. This is a bounded
EDIT callback obligation for S15, not a claim of whole-system reproduction.
