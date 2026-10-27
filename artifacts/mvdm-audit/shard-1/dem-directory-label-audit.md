# Original DEM directory, label and diagnostic ownership

The frozen sources `dos/dem/demdir.c`, `demlabel.c`, and `demmsg.c` match
OpenNT byte-for-byte. Current selected x86 worker build lists each source in
`original-mvdm-dem.lib`. Worker map SHA-256
`641E5C33430445ECC73885176250194BE936549654E14864074BD0B7D92D3CE2`;
worker image SHA-256
`D3437091D920A74FF72FDC8ED023C6207642682A7739C45751E4F03A8C1D53E5`.
Map ownership and behavior are distinguished below.

## Directory

`demdisp.c`'s SVC table dispatches create, delete, query-current and
set-current to the selected `demdir.obj` functions. `demCreateDir` and
`demDeleteDir` pass guest paths to OEM wrappers and propagate errors through
`demClientError`; `demQueryCurrentDir` verifies the drive and CDS path,
resetting an invalid path and associated `=X:` variable to drive root;
`demSetCurrentDir` updates host current directory then the drive-specific
variable. Creation with nonzero BX/SI takes the original extended-attribute
early branch, without claiming completed EA support. Source call and map
identity do not establish guest-pointer lifetime or all edge-case behavior.

Historical T420 S33 `docs/etc/evidence/m0-t420-s33-dem-capability-closure.md`
records actual guest create/remove directory and selected ordinary file
operations. T420 S37 `docs/etc/evidence/m0-t420-s37-oemuni-capability-review.md`
records non-ASCII create/chdir/remove and a COMMAND `=X:` readback on its
specified historical product, with exact environment/path comparison. Those
witnesses have different historical binary hashes and do not prove this S10
snapshot or all errors, EA cases, missing drives and nested/reentrant lifetime.
The T430 S5 directory reset fixture concerns `demsrch.c` and must not be
misattributed to `demdir.c`. Maintain pending until the original function
and guest wiring have current-identity integration and negative coverage.

## Volume label

`demfile.c::demCreateCommon` dispatches create with `ATTR_VOLUME_ID` to
`demCreateLabel` after obtaining a bounded guest-path copy; `demfcb.c` has
separate FCB create/delete label routes. The selected map attributes the
create/delete label routines to `demlabel.obj`. They delegate to host
`SetVolumeLabelA` and map boolean failure to 1; real volume mutation must not
be performed on any user volume as an audit shortcut. A historical T230
component fixture `t230_s3_demfile_direct_import_fixture.c` uses an invalid
`?:LABEL` path to test a label failure, but it is not evidence of success on
real media. T420 S37's later GetMediaId volume query and padding test is a
read-only query, not evidence of create/delete label semantics.

A source-level potential read-boundary concern deserves separate proof:
`demCreateLabel` loops on the pointer `while(s)`, increments the pointer, and
breaks after its length counter exceeds 11; it does not stop on NUL. Whether a
reachable caller can provide a sufficiently short accessible guest buffer or a
path whose end lies at a protection boundary, and whether resulting bytes
change observable behavior, has not been established. This is original source,
not proof of a new divergence, crash, or vulnerability. Use a fixture with an
explicit mock volume-label sink, bounded guard/canary and documented caller
shape; do not modify a real volume. Preserve the row as pending.

## Debug diagnostic

`demmsg.c` defines `demPrintMsg` and message array only under `#if DBG`;
`dem.h` defines `demPrintMsg(x)` to no-op in the other branch. The release
worker graph contains `demmsg.obj`, but inspection of that selected object
and the release map shows no `_demPrintMsg` definition. Therefore mere
compiled-object membership does not establish debug diagnostic functionality
in this release. This is consistent with a conditional debug-only profile,
not evidence that message handling is fully restored or approved for removal.
Verify the intended profile and any separately selected debug build before
classifying. No new runtime or user-media tests were run in this audit.
