# Shard 1: WOW combined thunk table boundaries

`wow32/wowtbl.c` matches both frozen OpenNT editions. Its object and the current release WOW32 code are bound by `wow-table-boundary-observer.py`; JSON records input identities and offsets. The original includes ten module tables in one immutable declaration. The observer counts the unconditionally included table rows, confirms all ten `0xffffffff` separators in both object and released image, and checks that the released `W32DllInitialize` contains the resolved call to `InitThunkTableOffsets`. It also binds `W32GetTableOffsets`, which copies the computed offsets to a guest-provided buffer. Original WOW16 boot source calls `WOWGetTableOffsets` in `kernel31/ldboot.asm:1680`, via `kthunks.asm:256`.

| Module | First combined index | Entries | Separator |
| --- | ---: | ---: | ---: |
| KERNEL | 0 | 310 | 310 |
| USER | 311 | 539 | 850 |
| GDI | 851 | 491 | 1342 |
| KEYBOARD | 1343 | 138 | 1481 |
| SOUND | 1482 | 19 | 1501 |
| SHELL | 1502 | 104 | 1606 |
| WINSOCK | 1607 | 152 | 1759 |
| TOOLHELP | 1760 | 3 | 1763 |
| MMEDIA | 1764 | 3 | 1767 |
| COMMDLG | 1768 | 27 | 1795 |

Under `wow.h:1496–1511`, the original local offset array has eleven elements: initial kernel offset and ten separator-derived offsets. Ten actual separators therefore fit that array; the last computed element follows the final separator and is not assigned to a named module. The explicitly assigned first ten entries correspond to the module boundaries in order. These counts describe the release profile and aren't proof of individual function behavior.

The initialization owner is the original DLL process-attach path; after initialization the offsets are consumed by the guest and dispatch, with no per-task separate table created by this source. Module tables include unimplemented and local sentinels as well as real thunks: a non-separator slot, or successful publication, must not be characterized as a restored implementation. Historical per-module observers bind selected entries; this report does not claim that all 1,786 function entries, guest ordinals, startup and failure paths, or any individual API have been validated. It also does not assert live guest-image lineage from the adjacent WOW16 source.

Replay `python artifacts/mvdm-audit/shard-1/wow-table-boundary-observer.py`. No product, guest, shared ledger, or build tree was modified; no product process ran.
