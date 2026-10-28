# Original WOW printer spool interface and dispatch

This shard-2 review covers `wow32/wspool.h` and `wow32/winspldl.h`, with
`wspool.c` used as a separately owned source context. The [observer](wow-spool-image-observer.py)
and [receipt](wow-spool-image-observations.json) check both original editions,
DOS16/WOW32 catalog identity, selected WOW32 graph, production object
dependencies and the released image. All three sources retain 100% original
bytes; this does not establish a behavioral restoration percentage.

`wspool.h` is included through `walias.h` and is in the valid `wowtbl.obj` and
`wspool.obj` dependencies. The selected original GDI table's six job entries
resolve through actual COFF relocation and released image pointers to original
`WG32OpenJob`, `WG32WriteSpool`, `WG32CloseJob`, `WG32DeleteJob`,
`WG32StartSpoolPage` and `WG32EndSpoolPage`. Their emitted bodies match their
selected object outside relocation words. Two real `WG32OpenJob` calls bind
to the selected `wgprnset.obj::LoadLibraryAndGetProcAddresses` provider.
The original source lazily resolves `WINSPOOL.DRV` functions; the host owns
actual printer handles, jobs and driver facilities. Initial binding does not
prove provider availability, exact host function resolution, real print
output, failure handling or lifetime.

The original guest-side alias functions map 16-bit print handles to host
handles, and source calls EndDoc/ClosePrinter before `FreePrn`. The original
code returns the requested byte count when WritePrinter succeeds, rather than
the `dwWritten` value; exact short-write semantics and real reachability need
verification. Source reading also leaves an open question about ownership on
StartDoc or alias creation failure after OpenPrinter succeeds. These are
unverified boundaries, not asserted current-product failures. Check actual
failure, cancellation, nested callbacks, concurrent task death and final handle
cleanup using isolated resources; never send test jobs to a real printer
without the authorized test setup.

Recommended file disposition: **partial** for `wspool.h` as the selected
declaration/ABI owner; execution and lifecycle are not accepted from a header
or linked pointers. `winspldl.h` has no observed dependency in the current
Ninja dependency blocks, but its full historical consumers and relation to
the selected declarations have not been resolved: retain **pending**, not an
automatic exclusion. `wspool.c` belongs to another partition and is not
classified by this checkpoint. Existing [printer-provider review](../shard-1/external-printer-provider-review.md)
and T468 bounded PDF results do not prove every spool, driver or media path.

The coordinator should replay the observer before merging. No shared ledger,
product image, guest, installed package, printer or user session was changed.
