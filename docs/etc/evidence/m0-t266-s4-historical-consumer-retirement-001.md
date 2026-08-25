# M0 T266 S4 — historical-consumer retirement evidence

## Disposition

- 41 exact Git renames are recorded in `m0-t266-s4-historical-consumer-disposition.tsv`: 7 reconstructed fixture sources now live in `tests/legacy/reconstruction/`; 34 historical scripts/manifests are retained only as non-executable `docs/etc/legacy_code` evidence.
- The CMake paths for the three reconstruction fixture targets now use `tests/legacy/reconstruction/`; no `refs/archive/reconstruction` consumer remains.
- The companion [`artifacts/research` archive report](../../../artifacts/research/m0-t266-s4-historical-consumer-archive-001/README.md) and JSON manifest record source/destination/hash/size for the entire rename set.

## Verification and limitation

- A tracked executable/configuration scan finds no `refs/` path after excluding documentation provenance and the root `.gitignore` entry reserved for S5 removal.
- Fresh CMake configure r002 was rejected before target generation because the long-obsolete global CMake file still names many removed `src/bx-vdm` and `src/cli` sources. The error lists no `refs/` input.
- Direct legacy fixture compile r003 likewise found pre-existing source/test naming drift (`app_s4_*` test names versus `ntdos64_s4_*` retained source names). This is evidence for their retired/default-disabled status, not a reason to restore a historical product branch during source closure.

## S4 conclusion

No still-executable tracked historical tool, probe, test or CMake configuration reads `refs/`. The two observed failures are retained as non-passing historical verification limitations; neither is claimed as a successful build.
