# Four old WOW32 table declaration headers

Disposition for each physical header: **selected original declaration-only header; no demonstrated active use of its old named table**. This is a role classification, not a claim of tested API functionality or a recommendation to delete the file.

The central ledger records these four included declaration roles as partial; their runtime restoration proportion is unmeasurable from inclusion and the current cached map. Functional and alternate-profile validation remains open.

| Header | Declared table / conditional debug-profile maximum | Current dispatch owner |
| --- | --- | --- |
| `wgtbl.h` | `aw32GDI` / `iGDIMax` | `aw32WOW` includes `wgtbl2.h` |
| `wkbtbl.h` | `aw32Keyboard` / `iKeyboardMax` | `aw32WOW` includes `wkbdtbl2.h` |
| `wktbl.h` | `aw32Kernel` / `iKernelMax` | `aw32WOW` includes `wktbl2.h` |
| `wutbl.h` | `aw32User` / `iUserMax` | `aw32WOW` includes `wutbl2.h` |

All four are included directly by `wow32.c:26-30`, and cached Ninja dependency records identify `obj/wow32.obj` as their consumer. Each consists of its original table `extern` and a further `extern` guarded by `DEBUG_OR_WOWPROFILE`; each has identical SHA-256 to both retained original source trees. The [existing identity observer](wow-old-table-declarations-observe.py) replays these identities, `src/mvdm` C/H old-name search, selected Ninja deps and cached link map. On this pass it completed successfully: the old names occur only in these declarations, and the map contains `_aw32WOW` in `wowtbl.obj` but not any of the old names. The identical search in the two original WOW32 C/H trees likewise returned just these header declarations.

The original `wowtbl.c:8-16` explains the 1992 consolidation into one table; `wowtbl.c:40-53` defines the listed subtable inclusions, and `wow32.c:913-949` uses `aw32WOW` for actual dispatch. These facts establish the selected table representation, not per-ordinal success, callback behavior, or an exhaustive audit of guest-visible GDI, keyboard, kernel and USER. Cached deps/map may not represent a fresh build against current sources. The bounded lexical search does not establish external binary consumers or DEBUG/PROFILE execution; no such behavior was tested. Keep functional gaps with their relevant S owner and do not infer that an entire subsystem is excluded merely because its old declaration is inactive.
