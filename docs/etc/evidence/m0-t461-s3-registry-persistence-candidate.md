# T461 S3 — Registry persistence candidate evidence

## Implemented boundary

`mvdm_shadow_registry.c` remains the only Registry-state owner.  A named
product commit mutex serializes the parent processes; each mutation reloads
the persisted parent footer, applies the selected original operation, writes a
flushed sibling and uses `ReplaceFile` (or first-create `MoveFileEx`) to make
the new `NTVDM.REG` visible.  The ordinary `.reg` body remains parser input;
the private comment footer holds only the parent overlay and key tombstones.
No guest input and no Windows Registry key is written.

## Focused executable evidence

`tests/mvdm-host/t461_registry_overlay_fixture.c` was rebuilt in
`build/t461-s3/registry-overlay-r1` and printed
`T461_S3_REGISTRY_PERSISTENCE_OK`.  It proves baseline override/create,
recursive-delete leaf tombstone, shutdown/restart reconstruction, a
replacement failure after the read phase without mutation of the accepted view,
and two child parent processes committing independent values without a
lost-update overwrite.

The formal x86 worker graph rebuilt `ntvdm.exe` from the changed source; the
published hash is `D77E9F7D1622BA53F1CAD23F46A15E2B24A6F76D50D43AD382D8CC4E5DD1799D`.
The formal WOW32 graph remained coherent with no pending work.

## Publication and remaining gate

The matching ten-image candidate is synchronized to `assets/release` and
`O:\winnt\system32`; hashes were compared after copy.  Product gate `gate-r4`
passed in 160,333 ms: retained WOW WINMINE/SOL/WRITE frontiers (66,167 ms),
Console-17 (45,290 ms) and Window-17 (44,931 ms).  The gate uses the retained
`G7.COM` fixture, not the removed historical build path.  Guest media is
unchanged.
