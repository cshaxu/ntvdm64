# T221 S3 P4: Overlay store foundation

## Question

Can the DEM whole provider own volatile copy-on-write file state without a
host write, a guest pointer, or a BOP-local overlay branch?

## Implementation and observation

`bx_ntvdm_dem_overlay_store_v1` is a dynamic session-private object table.
Each object has a selected drive, canonical relative DOS path, file bytes and
attributes, or a tombstone.  Replacing a file copies bytes into process memory;
turning it into a tombstone releases those bytes.  The store does not contain a
host path or handle and performs no host I/O.  Provider initialization owns its
lifecycle and teardown releases all storage.

A focused MSVC x64 C11 `/W4 /WX /MT` fixture in `build/M0-T221-S3/004` passed
overwrite, case-insensitive lookup, drive isolation, tombstone and teardown.
The complete current whole-provider fixture also built and ran with exit zero
in `build/M0-T221-S3/005`; it remains source-built fixture evidence, not a
native trace.

## Limits and follow-up

This is only the storage owner.  It is not yet connected to namespace open,
handle read/write, directory records or merged search, so no BOP currently
claims Overlay behavior.  The next P must add the view resolver's base-copy,
file/directory state and typed overlay token operations as one provider change;
it must not expose this store through individual selector branches.
