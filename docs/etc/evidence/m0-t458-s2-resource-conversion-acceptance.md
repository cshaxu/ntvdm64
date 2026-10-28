# M0 T458 S2 — original resource conversion acceptance

**Later correction:** The `wcurcash.c` selected-owner claim below is not supported by the original manifest or current graph. The selected cursor/icon alias owner is `wcuricon.c`; see the [T473 source and graph reconciliation](../../../artifacts/mvdm-audit/cross-369/cursor-cache-disposition.md). This does not change the scope of the recorded extracted-body tests.

**Status:** closed test/provider boundary; no production-source or guest-media
change.

## Selected execution path

The formal x86 graph continues to select the original `wuser.c::WU32LoadBitmap`
and `wucursor.c::WU32LoadCursor` callers.  The former obtains a bounded,
read-only protected byte lease before calling `pfnWOWLoadBitmapA`, then releases
that lease before returning.  The latter retains name conversion, calls
`pfnServerLoadCreateCursorIcon`, and gives its resulting native handle to the
original `wcuricon.c::SetupResCursorIconAlias`; `wcurcash.c` remains the
task-cache owner.  `clres.c`/`cldib.c` remain the sole bitmap parser/converter.

The project-owned output slots are deliberately finite: `wow_bitmap_load`
supplies a synchronous native view/DC capability for the original bitmap body;
`wow_private_user_load_create_cursor_icon` maps only the unavailable server
helper to public `CreateIconFromResourceEx` or `LoadImageW`.  Neither keeps a
resource, alias, parser state, or a guest pointer after its caller returns.

## Evidence

`build/t458-s2/cursor-r16` compiles extracted current original function bodies,
not copied implementations.  Its markers are:

* `WOW_BITMAP_CALL_ORDER errors=0 output=2 acquire=2 release=1`: protected
  bytes acquire/release once; an acquire failure returns before the output slot;
  a no-byte name lookup remains an original output-slot call.
* `WOW_CURSOR_CALL_ORDER errors=0 cases=3 output=3 aliases=3 ...`: cursor,
  icon and no-byte source forms retain the output-then-alias order.
* Both the bounded adapter and direct public route report
  `WOW_CURSOR_RESOURCE_BOUNDARY_OK` for 1/24bpp, `BITMAPCOREHEADER` and
  `BITMAPINFOHEADER`, color/monochrome selection, default sizing/hotspots,
  shared stock lookup and malformed/type rejection.

`build/t458-s2/formal-wow32-r1` is the formal cached x86 `wow32` build and
finishes `0` with `ninja: no work to do`; it confirms the reviewed selected
providers are the currently configured product graph.

## Real guest boundary

The retained private-desktop WINMINE observer was attempted against the
hash-identical coherent package at `build/t457-s3/package-r1`.  It compiled its
native probe but failed before worker registration with `No worker in this
owned runtime`; no resource caller or guest window was reached.  This is an
explicit non-pass of that observer path, not a resource acceptance claim and
not evidence of a product resource defect.  S4 retains application acceptance;
S2's source-proven caller and output-slot boundary is nonetheless complete.

## Disposition

No source-proven production defect exists in the admitted conversion seam, so
no product image was rebuilt or published.  The only committed changes are
focused test witnesses and their extraction/runner.  The S3 handoff is limited
to original accelerator conversion/dispatch and consumption of T447's existing
MENU-01 graph; it must not add a second menu parser, mutation store or resource
owner.
