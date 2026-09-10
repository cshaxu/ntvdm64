# T221 S3 P1: DEM whole-provider file-view boundary

## Question

Can the 29-service DEM file/handle/FCB/search package have one profile-selected
file-view authority before adding Overlay or Virtual filesystem state, without
letting a leaf silently fall back to Direct host I/O?

## Inputs

- T221 S2 whole-provider disposition and its required implementation order.
- OpenNT owner map: `demfile.c`, `demhndl.c`, `demfcb.c`, `demsrch.c`, and
  `demdir.c`; the retained T202 S3 ABI/failure map.
- Current `dem_whole_provider`, `dem_file_session`, local-file backend, CWD
  context, mutation-profile and host-namespace sources.

## Procedure

1. Added `bx_ntvdm_dem_file_view_v1`, owned by `dem_whole_provider` rather
   than any BOP partition.  It copies the existing registered DEM mutation
   profile and maps it to Direct, Readonly, Overlay, or Virtual view kind.
2. Moved local-file open/create admission behind this view.  Direct and
   Readonly retain the existing behavior.  Overlay and Virtual return explicit
   `NEEDS_*_BACKEND` dispositions; neither may call the host namespace.
3. Made the local backend reject a view whose copied profile differs from the
   token session's copied profile.  This prevents a caller from combining a
   Direct token session with a less-permissive view.
4. Extended the focused local-file fixture and built it in
   `build/M0-T221-S3/001` with MSVC x64, C11, `/W4 /WX /MT`, `ntdll.lib`.

## Observation

The fixture compiled and ran with exit code zero.  It covers Direct read and
owned-token release, Readonly read plus write refusal, Overlay and Virtual
non-fallback dispositions, malformed paths, and the new profile-mismatch
rejection.  The source closure includes the current session host-context
dependency of the CWD context, so the test does not rely on the stale T202
source list.

## Interpretation and limits

This is a provider-boundary migration, not completion of any BOP subfamily.
`mutation_overlay_v1` remains the small generic CWD/session record store; it
is neither used nor large enough to be a file COW implementation.  No file
content, directory merge, tombstone, metadata COW, virtual boot volume,
search ordering, or typed non-handle token has been added.  Direct and
Readonly local-host read behavior is retained pending the subsequent unified
typed-token/view migration.

## Follow-up

Add backend kind to the provider's opaque token state, migrate existing
Direct and readonly-namespace operations through that typed interface, then
admit package-owned Overlay and explicit Virtual backend state with family
isolation regression.  `50:47/48` remain Redirector-owned unavailable.
