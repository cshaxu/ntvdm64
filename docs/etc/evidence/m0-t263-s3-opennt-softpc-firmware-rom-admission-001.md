# M0 T263 S3 — OpenNT SoftPC firmware/ROM admission

Date: 2026-08-24

## Decision

The original OpenNT `base/mvdm/softpc.new/bios/` and `roms/` packages are
admitted source-identically below `src/opennt-softpc/`. They are original
firmware, ROM and machine-contract inputs. They do not activate a historical
SoftPC runtime, change Bochs mechanics, or create a static dependency from
`bx-mantle` to OpenNT.

## Provenance and verification

- Source: `O:\repos.external\opennt\base\mvdm\softpc.new`, Git `5c5b979e`.
- Target: `src/opennt-softpc/bios` and `src/opennt-softpc/roms`.
- Count: 27 `bios` files plus 8 `roms` files, 35 total.
- Verification: SHA-256 equality was checked recursively for every
  upstream-relative path after copy. No body differs and no local divergence
  is registered.

## Boundary

- `bios/` contains original assembly, includes, build descriptors and related
  source/build inputs for the historical firmware package.
- `roms/` contains original ROM/configuration images and their source
  companions.
- An eventual runtime selection is an `app` decision that passes opaque bytes
  through the declared mechanical load contract. `bx-core` and `bx-mantle`
  stay OpenNT/SoftPC-name blind; `adapter-softpc` remains the only
  same-shaped CCPU/SoftPC compatibility facade.

## Result

`opennt-softpc` now has its first complete, source-identical original package
set. Build or runtime use requires an independently admitted consumer and
must not be inferred from preservation.
