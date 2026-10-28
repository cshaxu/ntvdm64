# S10 shard 1: WOW tabbed text dispatch and conversion

`wow32/wutext.c` is a pending shard-1 source. The [observer](wow-tabbed-observer.py)
confirms its current bytes match both original editions and that it has one
DOS16 and two WOW32 frozen catalog rows. The [recorded evidence](wow-tabbed-evidence.json)
binds the selected object and both released/installed WOW32 DLL identities.
The selected cached DLL and released DLL have different whole-file hashes,
but their executable sections are byte-identical; this is the comparison used
for object-to-image binding.

The original USER table assigns TabbedTextOut ordinal 196 and
GetTabbedTextExtent ordinal 197. Both actual initial image slots resolve to
the original `wutext.obj` thunks, whose mapped code matches the compiled
objects outside relocation words. All eight relocation targets per thunk are
resolved individually: both route the signed 16-bit tab array through original
`getintarray16` in `wstruc.obj`, use the project GDI-handle conversion import,
and call the respective modern USER32 ANSI entry. Allocation, pointer mapping,
free and stack-cookie targets are recorded separately. This establishes an
initial linked call chain, not actual guest invocation or every execution path.

`wstruc.c` has other mirror differences, but the checked source body of
`getintarray16` matches primary OpenNT exactly. For a positive count, if
the mapping returns null while the selected stack buffer exists, the helper
returns without filling it and the unchanged original caller still passes that
buffer to USER32. If heap allocation returns null, the caller passes null to
USER32. These are source-level conditional paths, **not a reproduced guest
fault**. In release, `GETVDMPTR` delegates to `GetPModeVDMPointerAssert` and
the `cb` argument is not passed by that macro, so actual selector, mapping and
failure behavior must be traced rather than assumed.

| Source | Recommendation | Retention and remaining scope |
| --- | --- | --- |
| `wow32/wutext.c` | Partial candidate | 100% original source bytes; overall behavior percentage unmeasurable. Exercise both guest dispatches, signed tab positions, zero/negative and stack/heap-threshold counts, ANSI/DBCS, invalid selector, allocation failure, returned values and cleanup. Verify handle and mapping lifetimes. |

The existing extracted Edit fixture calls a separate wide API and does not
cover these two actual thunks. No product or guest was modified and no new
runtime test was claimed by this source/image observation. `wstruc.c` and the
USER table are dependencies, not additional shard-1 dispositions here.
