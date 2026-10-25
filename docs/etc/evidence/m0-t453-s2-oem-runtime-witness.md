# M0 T453 S2 OEM runtime witness

Status: partial S2 evidence; it does not close the remaining WOW directory
and retained-delete consumers.

## Reconciled direct and nested DOS evidence

`tools/audit/Verify-T420S37OemGuest.ps1` was a retained real-DOS OEM witness,
but its historical flat-package assumptions no longer matched the deployed
layout. The test now selects the one production binary root
`<package>\system32` when present while keeping the DOS probe's working
directory at the package root: the authored probe deliberately uses relative
`tests\O37G\...` paths. It explicitly sets the established private-desktop
and selected runtime `PATH` environment only for its child observer, then
restores both process variables. `--wait` makes this a completion witness,
not a mere Run16 submission observation.

The initially incorrect SYSTEM32 working-directory revision failed at the
probe's first relative directory operation. It is discarded as a harness
failure, not product evidence. Its correction is intentionally limited to
layout, child environment and completion semantics; it introduces no product
path resolver, OEM conversion, guest change or retry policy.

Against `O:\winnt`, run `t453-s2-oem-r7` passed both routes:

| Route | Evidence | Proven boundary |
| --- | --- | --- |
| Direct | `O:\winnt\logs\t453-s2-oem-r7-direct.txt` and `.console.txt` | selected DEM/OEM create, write, rename, attribute, re-open/read, directory, FCB delete, disk/media and COMMAND drive-environment publication. |
| Nested | `O:\winnt\logs\t453-s2-oem-r7-nested.txt` and `.console.txt` | the same operations through two `COMMAND.COM /c` levels, including final cleanup. |

The probe is byte-identical to the retained S37 input:
`DD643A9D94ACC08E8D1548FEE4B933E8F3747FCDAD20BF9466CAD75B14A87F78`.
Each successful route checked the actual host Unicode filename/content and
removed only the exact verified `tests\O37G` files/directories.

## Sourced correction found during the sweep

`WK32FileSetAttributes` rejects a non-root trailing separator before its
`NormalizeDosPath` call. Its original cleanup predicate then read the
normally-produced named-pipe flag and potentially freed the normally-produced
pointer despite neither output existing on that branch. DIV-451 initializes
only those cleanup locals. The original error precedence remains
`ERROR_PATH_NOT_FOUND`; the DOS mask, native call, result encoding and normal
path stay in their selected original owner. The exact `wkfileio.obj` rebuild
passes; focused execution coverage remains an S2 requirement before this
correction can be delivered.

The coherent x86 WOW32 provider target was rebuilt after the correction:
`ninja -C build/.ninja/msvc-x86-release/wow32 wow32.dll` compiled the selected
`wkfileio.c` and linked `wow32.dll`.  The remaining compiler diagnostics are
pre-existing legacy warnings in unrelated original routines; no new error or
link failure was introduced.

For the previously unobservable cleanup edge, a temporary x86 fixture linked
the production-built `wkfileio.obj` and called its real
`WK32FileSetAttributes` entry with the exact guest `<drive>:\\W04\\` frame.  It
provided only the frame-address and `SetFileAttributesOem` external boundary;
it did not include, copy or replace the selected source body.  The result was
`WOW_FILE_ATTRIBUTE_TRAILING_SEPARATOR_CLEANUP_OK`: the original
`FFFF0003`/`ERROR_PATH_NOT_FOUND` result and no native attribute call.  The
linker correctly reported unresolved references in other, uncalled legacy
thunks in the same object; `/FORCE:UNRESOLVED` confines them to that disposable
test image and is not a production-link setting.  The temporary source and
image are removed after recording this witness.

The same bounded method exercised the production-built `wkman.obj` through
the original retained-file branch of `WK32WowDelFile`: the fixture made the
first OEM delete succeed while the name remained visible, then supplied the
original full-path, temporary-name, replace-existing move and temporary-delete
results.  The real thunk returned success after exactly one direct delete,
one retained-file move and one temporary delete, without entering the
font-removal fallback (`WOW_DELETE_RETAINED_FILE_ROLLBACK_OK`).  It supplied
only imported OEM API outcomes and the guest-address edge, not a copied delete
algorithm or a product fault-injection hook.  As with the attribute fixture,
unresolved references are other uncalled entries in the monolithic original
object and exist only in the disposable `/FORCE:UNRESOLVED` test image.

Two additional disposable x86 fixtures exercised the production-built
directory consumers rather than a copied approximation. The real
`WU32DlgDirList` body reported `WOW_DIALOG_DIRECTORY_SYNC_ORDER_OK`: the
fixture supplied only the guest pointer mapping, `UpdateDosCurrentDirectory`
boundary and native `DlgDirListA` boundary, and observed the exact original
sequence `DIR_DOS_TO_NT` → native directory list → `DIR_NT_TO_DOS`. The real
`WK32LoadLibraryEx32` body likewise reported
`WOW_LOAD_DIRECTORY_IMPORT_ONLY_OK`: it performed exactly one
`DIR_DOS_TO_NT` import before its existing native-extension load boundary,
and did not publish an artificial post-load DOS update. Both fixtures linked
the production objects with `/FORCE:UNRESOLVED` only because each historical
translation unit also contains unrelated uncalled provider entries; their
temporary source, object and image are removed after this record. Neither
fixture changes a process cwd, adds a current-directory owner or substitutes
a dialog/load implementation.

## Retained original ownership checks

The selected `WK32WowDelFile` body and both `WU32DlgDirList` bodies compare
byte-for-byte (apart from file position caused by unrelated completed
divergences) with the pinned OpenNT source.  In particular, the delete body
still owns its direct-delete visibility check, temporary rename/delete,
rename rollback, font-removal fallback, `WM_FONTCHANGE`, and final DOS error
packing.  The dialog bodies still perform the original DOS-to-native import,
native dialog call, then native-to-DOS publication; `WK32LoadLibraryEx32`
still performs its original import-only boundary.  No substitute delete
algorithm, current-directory cache or global cwd lock is present or added.

`verify-dem-directory-reset.ps1` was also rebuilt against the current shared
fixture closure.  It reached the real selected directory open, 8.3 formatter
and buffer-reclamation branches, then terminated with `STATUS_INVALID_HANDLE`
in this restricted fixture environment before its historical summary.  The
same limitation was already recorded by T448: the fixture relies on a broad
`/FORCE` test closure and is not a runtime guest failure or evidence for a
new DEM repair.  It is therefore retained only as a harness limitation; this
packet neither changes `demsrch.c` nor uses that result to claim a product
defect.

The separately authored Win16 directory probe was rebuilt from
`tests/observation/win16_directory_probe.asm` (1,277 bytes) and launched in
an isolated copy with the existing observer.  Its r3 record reached the
selected `run16.exe`, NTSRV and NTVDM images but did not reach the guest
directory calls before the observer's existing 20-second bound; no `DIRP.DON`
or `DIRP.BIN` was produced.  This is an unclassified isolated-startup
observation, not evidence of a cwd defect and not a basis for modifying the
original KERNEL/USER directory bodies.  The raw r3 observer excerpt is
retained only until this evidence entry is written; the disposable copied
package is then removed under the existing exact-target policy.

## S2 consumer boundary

The retained-file direct/rename/delete sequence and native dialog/load
directional boundaries are now exercised through their production objects.
The remaining S2 work is coherent package review: rebuild after the bounded
attribute correction, assemble the full delivery package, and exercise the
affected real-DOS OEM routes. Font fallback and non-ASCII consumers remain
explicitly assigned to S3; neither permits a generic file/environment service
or a second cwd state.

## Coherent package and product witness

`tests/build/Test-FullComponentBuild.ps1` recorded the first and unchanged
second cached passes in `build/t453-s2/full-component-build/result.json`.
The exact ten selected images passed, including the rebuilt x86 `ntvdm.exe`,
`WOW32.DLL`, `VDMREDIR.DLL`, `nthook32.dll` and the x64 carrier/service/hook
set.  The record has `passed=true`, 1,493 stable intermediates and
`unchangedIntermediates=true`; it is the build identity for this packet rather
than a separate cache or test-made binary.

`Stage-System32ProductPackage.ps1` then created the fresh coherent
`build/t453-s2/candidate-package` and its ten-file
`product-system32-manifest.json`.  A direct launch from that deep repository
path correctly exposed the retained guest path-budget limit (the physical
system root was 54 characters and returned the historical `0x42b` route
failure), so it is not acceptance evidence.  The existing
`New-PhysicalRuntimeStage` mechanism instead copied that exact package to a
short, disposable physical-stage root.  With the OEM witness's supplied-package
`logs` directory creation fixed, direct and nested `O37G` both passed there
under `t453-s2-oem-stage-r1`.  This harness correction only supplies a log
destination omitted by the deliberately slim stage; it changes no product
binary, guest media or OEM behavior.

The complete product gate then passed from the same candidate package in
`build/t453-s2/product-r1`: `wow-frontiers` (67,328 ms), `Console-17`
(48,894 ms) and `Window-17` (47,420 ms), 173,474 ms total.  It used current
source-built read-only observers, not an old temporary observer.  The gate's
SOL and Write assertion was corrected before this run because its retained
T452 baseline contains the exact visible `Solitaire` and `MSWRITE_MENU`
application windows, while the previous expression only accepted stale
`Static` status windows.  The revised assertion accepts either exact retained
application identity; it does not accept an arbitrary visible window.  The
candidate manifest was rechecked after the gates, so this is an identity and
behavior witness for the same selected ten images.

Before publication, the previous complete release and installed ten-image sets
were copied to `build/t453-s2/publication-r1/recovery-assets` and
`recovery-system32`.  The publication record then copied all ten candidate
images—not only the changed WOW32 provider—to `assets/release` and
`O:/winnt/system32`, regenerated the committed release manifest, and
verified every candidate/release/installed SHA-256 equality.  A final direct
and nested OEM smoke from the published installation passed under prefix
`t453-s2-published-r1`.  `assets/winnt.zip` remains unchanged
(`7A0534DEC9940B2D8366C8EF71DD536ED94BE7817D7C4E1E19BED05F6E60B8F7`):
this S makes no guest-media change.
