# T199 S37 DEM FCB Handle Family Disposition 001

## Scope

This record covers one original component family, not BOPs selected from a
trace: `demfcb.c`'s file-handle operations.  Its selector authority is
`src/opennt/base/mvdm/dos/dem/demdisp.c`.

## Original dependency boundary

`demfcb.c` stores historical NT handles in FCB-visible register pairs and
uses `CreateFileOem`, `CloseHandle`, `ReadFile`, `WriteFile`,
`SetFilePointer`, and VDM DTA pointers.  The readonly CLI profile deliberately
has no writable filesystem, projected NT-handle table, or host-pointer DTA
mapping.  Direct linkage would reintroduce the historical intrusive host
composition.

## Provider disposition

| DEM service | Original handler | CLI disposition |
| --- | --- | --- |
| `50:07` | `demDeleteFCB` | source-derived unavailable handle/filesystem result: `AX=5`, `CF=1` |
| `50:0A`, `50:0C` | `demFindFirstFCB`, `demFindNextFCB` | separately bounded immutable-profile search provider; it owns copied SRCHBUF and continuation handling |
| `50:20` | `demRenameFCB` | `AX=5`, `CF=1` |
| `50:2C`, `50:2D` | `demCreateFCB`, `demOpenFCB` | `AX=5`, `CF=1` |
| `50:2E` | `demCloseFCB` | zero historical handle succeeds with CF clear; all nonzero handles return `AX=5`, `CF=1` |
| `50:2F` | `demFCBIO` | `AX=5`, `CF=1` |
| `50:30` | `demDate16` | existing bounded DOS-format clock provider |
| `50:31` | `demGetFileInfo` | `AX=5`, `CF=1` |

The explicit set in `bx_ntvdm_dem_fcb_provider_v1.c` prevents an unrelated
future FCB route from silently inheriting the terminal result.  It does not
claim that the full FCB package is closed: malformed SRCHBUF handling,
continuation edge cases, clock-policy review, and package-wide regression
remain part of S37.

## Evidence

`artifacts/build/t199-s37-dem-package-r62-fcb/` is the MSVC x64 `/MT` full DEM
family fixture.  In addition to the canonical `50:00..48` sweep it asserts
the explicit `AX=5`/CF failure for all six unavailable handle operations and
the nonzero-handle `50:2E` failure.  It also verifies that `50:30 demDate16`
writes its two results to AX (packed date) and DX (packed time), as documented
by `demfcb.c`; an earlier provider incorrectly used BX for the latter.  No
native trace was run.
