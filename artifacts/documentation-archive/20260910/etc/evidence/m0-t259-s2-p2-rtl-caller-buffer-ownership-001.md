# M0 T259 S2 P2 — RTL Caller-Buffer Ownership

`cmdCreateProcess` in the imported OpenNT COMMAND source uses
`RtlUnicodeStringToAnsiString(..., FALSE)` with a caller-owned bounded command
buffer. The prior facade overwrote that buffer's `MaximumLength` on success
and could free/clear it after a conversion failure. Both actions violate the
original non-allocating contract and can corrupt later source-visible buffer
ownership.

The same-shaped `bx-vdm` facade now preserves the original pointer and
capacity for a non-allocated destination; only a `TRUE` allocation result is
owned by `RtlFreeAnsiString`. No OpenNT mirror or Bochs source changed.

External Ninja incrementally rebuilt formal-r3 and passed the COMMAND fixture,
including a caller-owned `UNICODE_STRING → ANSI_STRING` conversion assertion
and the existing source-mirror `cmdCheckBinary` guest-writeback regression.
