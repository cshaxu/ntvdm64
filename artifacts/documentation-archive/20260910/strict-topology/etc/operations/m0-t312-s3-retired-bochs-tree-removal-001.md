# M0 T312 S3 — retired Bochs tree removal

Date: 2026-08-30  
Status: closed

## Removed production roots

- `src/bochs-core` — 208 tracked mirror files.
- `src/bochs-core-overlay` — 12 private overlay files.
- `src/adapter-bochs` — 15 retired machine-facade files.
- `src/adapter-mvdm-host-in` — former #UD ingress/frame bridge, which has no
  original SoftPC/CCPU40 owner role in the selected formal graph.

## Removed acceptance/build inputs

- 48 `tests/adapter-bochs` fixtures and the 19 fixtures directly consuming the
  retired ingress were removed from current test roots.
- The prior Bochs-specific Ninja generators, component manifest, and related
  matrix runner were moved below `docs/etc/legacy_code/retired-bochs/`.
- Earlier reconstruction fixtures beneath `tests/legacy/reconstruction` were
  moved to the same evidence tree because they name files no longer present.

## Evidence disposition

Source-derived register, SAS, frame, callback, old generator, and fixture
material is retained only below `docs/etc/legacy_code/retired-bochs/`. It may
not be included, linked, run, or accepted as a product input.

## Source scan

After removal, a source/test/build/authority scan has one intentional match:
`New-T310OriginalSoftpcNinja.ps1` lists `bochs-core` and `adapter-bochs` only
as forbidden inputs. It cannot select either path.
