# T221 S3 Overlay visible-enumeration foundation

## Question

Can one selector-blind provider enumerate the Overlay-visible children needed
by the full FCB wildcard owner package?

## Implementation

`bx_ntvdm_dem_overlay_namespace_view_v1_enumerate` now merges read-only
admitted-root enumeration with COW entries, tombstones and directory
relocation. Each host candidate is re-checked through the existing visible
node model, and private direct children at both visible and effective
relocated paths are deduplicated into adapter-local DOS entries.

The seam accepts the drive root as well as non-root directories. It exposes no
host path or handle and does not decode BOP bytes.

## Evidence

The whole-provider MSVC x64 `/MT` fixture at `build/M0-T221-S3/085` passes.
It verifies a relocated private directory enumerates its COW child by its
visible DOS name. This is a provider foundation only: `50:07/20` remain
unbound pending one shared wildcard mutation binding and package regression.
