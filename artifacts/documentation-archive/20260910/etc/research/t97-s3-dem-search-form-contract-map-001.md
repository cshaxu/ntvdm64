# T97 S3 DEM Pathname and FCB Search Contract Map

## Result

OpenNT DEM search has two distinct guest ABIs. They share original
`demsrch.c` ownership but cannot share a generic adapter error/result path.
Neither form is admitted to runtime in T97/S3.

| Form | BOP services | Inputs | Guest result and continuation | First-profile disposition |
| --- | --- | --- | --- | --- |
| Pathname | `50:09` find-first, `50:0B` find-next | First reads `DS:DX` wildcard pathname and `CX` attributes; both use the DTA registered by `50:1B`. | Packed `SRCHDTA` is 43 bytes (`dosdef.h`): two 32-bit continuation fields in bytes 0--7, then attributes/time/date/size/name. First/next failure clears both fields; invalid next returns `ERROR_NO_MORE_FILES` (`0x12`) with CF set. First normalizes file-not-found to no-more-files and bad pathname/directory to path-not-found. | Candidate only for an immutable, profile-declared entry snapshot. Existing path transaction correctly models opaque words, two cleared words, and no-more output, but has no runtime decoder/plane integration. |
| FCB | `50:0A` find-first, `50:0C` find-next | First reads `DS:SI` 53-byte `SRCHBUF`, `ES:DI` FCB pathname, `AL` extended-FCB flag, and `DL` attribute. Next consumes the `SRCHBUF`. | `SRCHBUF` is 53 packed bytes: leading FCB fields followed by a `DIRENT` whose reserved area contains the original find pointer/id. `demsrch.c` also associates the search with a PSP/PDB list and frees it through process termination. | Deferred. Existing source-shaped success fixture deliberately writes only bytes 1--51 and leaves byte 52 untouched; FCB no-match and PDB lifetime behavior are not closed. |

## Source and Boundary Evidence

`demsrch.c:240-478` is the pathname owner. It initializes both DTA
continuation words, writes source-shaped metadata through `FillSrchDta`, and
has an explicit invalid/terminal next rule. `dosdef.h` supplies the packed
43-byte `SRCHDTA` layout. The existing `search_result_v1` mirrors only that
proven surface: its path success transaction writes the two opaque words and
the metadata region; its no-more transaction clears those words, sets AX=12,
and sets CF.

`demsrch.c:506-665` is the FCB owner. `dosdef.h` supplies the distinct packed
53-byte `SRCHBUF`/`DIRENT` layout. The source uses native pointer values and
find IDs in guest-reserved fields, plus global/PDB search lists and VDD process
termination hooks. `search_result_v1_prepare_fcb` therefore remains a static
success-shape probe, not a provider: it preserves the unresolved final byte
and exposes no FCB no-match API.

The original component's `SearchFile`, `FileFind*`, `NtVdmControl`, Unicode
conversion, directory handles, native pointer continuation, and PDB lists are
unavailable historical host composition. Replacing them with a host directory
enumerator would violate the immutable CLI profile. A future source-derived
pathname provider may enumerate only the finite BYOB resource declaration,
copy a private ordered snapshot into adapter-local session state, and place
opaque non-pointer continuation values in the source-defined DTA fields.

## Fixture Closure Limitation

The existing `search_plan_v1` transaction fixture is not an admissible T97
provider precursor. It links `host_namespace_v1`, whose test creates and
enumerates a host temporary directory. A strict current-SDK C11 link also
stops in `bx_ntvdm_host_namespace.c`: its native directory-query branch names
`FILE_ID_BOTH_DIR_INFORMATION` and `FileIdBothDirectoryInformation`, neither
available in the current public SDK headers. The search-request decoder itself
passes strict syntax checking after an explicit-parentheses correction to its
existing mixed `||`/`&&` bounds condition.

This is recorded as an architecture limitation, not a request to add a Windows
native-API compatibility shim. Repairing the host namespace would expand an
ambient host-directory model expressly excluded by T97. The future S4 provider
must instead start from BYOB's finite resource declaration and be linkable
without `host_namespace_v1`.

## S3 Decision

Pathname search has sufficient evidence for a later contained provider only
after S4 reconciles it with the withdrawn file-token resource and the exact
guest DTA registration/physical-address flow. FCB search is expressly
deferred; its service numbers must remain pass-through/deferred even if the
pathname form becomes available. No host scan, PDB emulation, VDD call, or
new direct BOP recognizer is admitted by this map.
