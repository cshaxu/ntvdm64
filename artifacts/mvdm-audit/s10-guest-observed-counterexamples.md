# S10 review of 195 observed guest-target C/H roles

[Per-file results](s10-guest-observed-counterexamples.csv), [full evidence](s10-guest-observed-counterexamples.json), and [observer](s10-guest-observed-counterexamples.py) recheck the 195 current `guest-target-observed` C/H paths in `host-ch-scope/inventory.csv`. The observed distinction is a physical-source role in four selected build trees; it neither removes a guest feature nor establishes any guest runtime result.

All 195 paths have at least one currently matching source/recipe receipt; all 436 collected witness pairs matched their recorded hashes. The observer explicitly reads the four current worker/WOW32 Ninja dependency streams with Ninja 1.13.2, reporting zero stale dependency blocks. It finds no exact selected-tree dependency, exact selected graph path, local-resolved physical include from non-WOW16/non-DOS-v86 C/H, same-basename non-guest C/H include, or adjacent recipe with both an explicit WIN32 marker and matching source/object token for these 195 paths. Therefore no path warrants return to host/shared on these bounded observations. This is not an exhaustive search of alternate targets or ambiguous indirect rules. The previously identified dual Win16/Win32 DDEML test files are `role-pending`, not part of these 195.

`observe.py` verifies current source and recipe hashes, current `build.ninja` hash against the receipt, and the receipt's recorded `stale_blocks`; it does not independently re-read current `ninja -t deps` output, rescan newly added include consumers, or compare current dependencies to the recorded dependency hash. In fact, the x86 worker's graph hash remains unchanged while its dependency-output hash differs from the earlier round-3 receipt. The current independent recheck found zero stale blocks and no exact dependencies, but a later graph-preserving dependency change or new consumer still requires renewed evidence. Do not interpret a continuing `guest-target-observed` label as an automatically refreshed or all-profile conclusion.

The include scan covers C/H beneath `src` and `tools`, excludes the historical guest directories, and detects literal includes. It does not resolve conditional preprocessing, generated includes, assembly or indirect build systems; same-name collisions, had they occurred, would require actual include-path resolution. Adjacent WIN32 recipe token search does not resolve every other recipe or variable expansion. The receipt checks reproduce the present observer's hash policy; they do not independently validate every original guest executable or run/rebuild guest. Preserve related S11-S17 feature obligations and revisit any path when new exact contrary evidence appears.

The 195 guest-target source-role handoff was revalidated against the current
worker and WOW32 Ninja commands and dependency streams with
`check-guest-pending-crosswalk.py`: all 195 entries have one scoped host-role
disposition, match their registered original source hashes, and have no
mirror-difference row. None appears as an exact selected x86 host graph or
dependency path. The combined mixed-role ledger intentionally still marks
all 195 pending; this is not a guest capability or runtime closure and does
not cover alternate profiles. Per-file stage handoffs remain in the six
crosswalk tables and the T recovery worklist.
