# r20 `DemGetDPBList` Boundary

`C4 C4 50 46` is `SVC_DEMGETDPBLIST`. NTDOS calls it during `msinit.asm`
after device initialization, with ES:BP naming the destination. The original
`demGetDPBList` in `demgset.c` enumerates drive types, writes a variable-length
DPB list into guest memory, and returns BP just past the list. Its source
explicitly says it fills drive/unit fields according to the host-visible drive
configuration.

This is not another initialization-locator registration. The original source
does **not** query volume information: for each `DRIVE_REMOVABLE` or
`DRIVE_FIXED` entry it writes only `DriveNum`, `Unit`, and `Next`; all other
DPB fields retain their destination contents. Therefore the existing immutable
26-slot type inventory is sufficient input for this exact historical slice.

The admitted implementation is now split at the intended boundary.  Adapter
code recognizes `50:46`, derives the source-written descriptor pairs from the
immutable 26-slot inventory, produces a checked 64-write/4096-byte opaque
transaction, advances BP, and binds it to one copied boundary.  The Bochs
consumer sees only that opaque transaction: it preflights every ordinary-RAM
span, copies only after complete preflight, and applies the checked CPU result.
It contains no selector, service, drive, DPB, DOS, or OpenNT policy.

The packed-layout/output proof is `sizeof(DPB) == 33`, with the only written
fields at offsets 0, 1, and 25.  Thus each admitted drive produces two
disjoint writes (2 and 4 bytes); at most 26 drives use 52 descriptors and 156
payload bytes.  Unit tests prove the descriptor contents, BP contract,
session binding, overlap/range rejection, and adapter-runtime dispatch.  The
r22 isolated native fixture is the pending end-to-end proof of the copied
Bochs closure; it must not be confused with a filesystem or full NTDOS boot.

The currently admitted runtime fixture has two qualifying drive types.  The
historical empty-qualified-list return is not yet an admitted multi-write
case, because an empty descriptor transaction is intentionally rejected by
the generic ABI.  It remains an explicit adapter-side follow-up, not a reason
to fabricate a zeroed DPB record or add a Bochs special case.
