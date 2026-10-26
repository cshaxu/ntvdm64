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
candidate hash is `D3C8DCE16B08D862315FB6C45A18257A596BCE91CF2302F354E0B6FB8CEEE6AE`.
The formal WOW32 graph remained coherent.

## Publication and remaining gate

The matching ten-image candidate is synchronized to `assets/release` and
`O:\winnt\system32`; hashes were compared after copy.  The long Product gate
was started against a cache-first package (so the new NTVDM image was actually
staged).  Its WINMINE and SOL frontier observations reached their existing
visible application identities.  The current execution host terminated the
long WOW frontier session before the WRITE/Console-17/Window-17 completion
records were emitted.  This is **not** a Product-gate pass and S3 remains
active until the existing complete gate can run to its structured `timings.json`
result.
