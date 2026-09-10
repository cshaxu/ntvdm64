# T221 S3 Overlay search-entry binding

The first-search half of the `demsrch.c` package now selects Overlay visible
entries once inside `dem_search_partition`: both pathname `50:09` and FCB
`50:0A` decode their copied request, enumerate through the selector-blind
Overlay view, then reuse the existing entries transaction. `50:0B/0C` keep
their existing typed continuation state.

The entries transaction no longer incorrectly requires the request directory
to be the drive root, so an already-copied DOS-relative subdirectory remains
valid. MSVC x64 `/MT` source build passes at `build/M0-T221-S3/093`.

This is not search package closure: dedicated Overlay pathname and FCB
first/next regression, including tombstone/COW/relocation visibility and
Virtual refusal, remains required before any trace.
