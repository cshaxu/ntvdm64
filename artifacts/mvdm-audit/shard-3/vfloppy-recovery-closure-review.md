# Legacy virtual-floppy recovery closure

## Conclusion

The checked-in original `nt_vflop.c` is **not currently wired into the observed NTVDM worker**, and its legacy B: file-backed behavior cannot be advertised as supported. This is a source-traced capability candidate, not a demonstrated user failure. The neighboring real removable-drive provider is not an equivalent image backend. No product, guest, physical media or shared ledger was changed.

## Original mechanism and missing entry

The `VFLOPPY` body registers `gfi_vdiskette_command`, on/off and reset for drive B in `gfi_function_table` (`nt_vflop.c:170–187`), but neither current nor compared OpenNT MVDM C/H searches find callers of `gfi_vdiskette_init` or `_term` outside that file. The selected NT host `sources`, current worker Ninja and generator do not name this file or enable `VFLOPPY`. Thus the original entry's actual profile activation and removal are not recovered; a linkable function alone is not an accessible drive.

Its own backing is the original host configuration directory plus `diskette_name`; it tries read/write then read-only. `fl_read_SID` currently opens hard-coded `c:\\softpc\\dos.SID` rather than using its parameter, parses textual track/head/N/sector IDs, and populates a 40-track, two-head table (up to 12 sectors per track). This is a bounded historical sector-map format, not proof that arbitrary raw floppy images or formats work. Its dependencies include host configuration and path handling, ownership helpers, GFI/FLA, DMA and SAS; the current legacy hard-coded path is not a valid portable production integration.

## Why enabling the macro is not a complete repair

`FDC_READ_DATA` logs short read/seek errors but can still return a success-shaped result; its sector-missing branch later uses `transfer_count` despite its initialization being confined to the other branch. `FDC_WRITE_DATA` computes a position even when sector lookup returns -1; read/write index `fl_sector_sizes` by guest `N` without visible bounds checking. SID parsing indexes fixed arrays from file input without visible range validation. `FDC_READ_TRACK` does no transfer and `FDC_FORMAT_TRACK` logs supplied IDs but does not format a medium. These observations warrant controlled validation and correction before production exposure; they do not establish that a real guest exercised the paths. Table installation sets only four callbacks, so compatibility with the other `GFI_FUNCTION_ENTRY` fields and replacement/teardown must also be proved rather than assumed.

## Finite restoration boundary

1. Trace the original B:/SID protocol, activation and media selection semantics under this T's recovery objective. A raw image mounting interface is a separate feature and is not proven equivalent to this source.
2. Trace and reuse selected GFI/FLA/DMA ownership, install all required callbacks with once-only deactivation and session-scoped file lifecycle. Supply an explicit controlled path/media descriptor through an existing authoritative owner; no hard-coded user path or second independent media state.
3. Preserve original sector-mapping and FDC result semantics where established; validate sizes and indices, correct failure reporting and implement only the operations actually required. Any behavioral changes to original mirror code require the project exception register. Do not report format/read-track success when not performed.
4. Test actual selected provider with fixture-owned files and controlled valid/malformed SID, empty media, write protection, read/write, seek failures, bounds, reset, attach/detach and overlapping teardown; test FDC command/result bytes and DMA. Then use an isolated nonmodified guest with disposable media for applicable DOS/WOW/Win3.x regression. Record physical-media work separately.

Until those entry, contract and tests exist, keep this original provider pending and do not conflate it with the separately partial real-floppy path. Existing evidence is in `vflop-real-header-selection-review.md` and `../host-ch-scope/virtual-real-floppy-owner-review.md`.
