# M81 OpenNT Source-Scope Reconciliation

Status: completed local source-scope audit, 2026-08-08.

## Finding

The checked-in `src/opennt` tree is a fixed, manifest-backed historical input
island for the selected guest closure and host behavior oracle. It is not a
full copy of the broader OpenNT `base/mvdm` reference corpus from which the
older 75-row descriptor census was generated.

| Source root | Descriptors | Declared-output rows |
| --- | ---: | ---: |
| `src/opennt` staged input island | 79 | 43 |
| `D:\home\repos.hobby\opennt` full local reference corpus | 194 | 75 |
| Shared descriptors | 79 | 43 |
| Present only in reference corpus | 115 | 32 |

The staged island covers the selected direct NT4 DOS source closure and the
historical host modules used as behavior evidence. The missing reference
descriptors are predominantly WOW16/WOW32, VDD samples, product utilities,
and unselected historical composition/provider branches. Their absence is a
source-scope fact, not a failed guest build or a claim that 32 direct DOS
artifacts are unbuilt.

## Inventory Tool Correction

`tools/Export-OpenNtArtifactInventory.ps1` formerly derived relative
descriptor names by subtracting a possibly relative input string from an
absolute file path. Invoking it with `-OpenNtRoot .\src\opennt` consequently
produced invalid `rc/opennt/...` descriptor names. The tool now resolves the
input root before deriving `base/mvdm` and relative paths.

With that correction, a temporary dual-root scan proves the table above. It
does not modify either source tree or the tracked historical census.

## Governance Result

The 194/75 census remains the full-reference discovery and classification
baseline described by `OPENNT-MVDM-DECLARED-OUTPUT-TRIAGE.md`. It must not be
presented as a fresh scan of `src/opennt`. Conversely, the 79/43 staged scan
is the only source-root inventory that can prove what current local historical
targets may consume without another import.

If a future bounded task needs one of the 115 reference-only descriptors, it
must first copy the necessary original files into `src/opennt` at their
original relative paths and update `SOURCE-MANIFEST.json` with upstream
revision, source path, hash, license/research restriction, and any local
overlay. It may not compile against `D:\home\repos.hobby\opennt` implicitly.

No additional import is justified by the first NT4 EN-US command profile:
the direct guest closure is already recorded separately, while the external
backend gate remains the current execution blocker.

## Evidence

- `tools/Export-OpenNtArtifactInventory.ps1` run with `ExecutionPolicy Bypass`
  only for temporary local scan output;
- `src/opennt/SOURCE-MANIFEST.json`;
- `docs/OPENNT-MVDM-ARTIFACT-INVENTORY.md`;
- `docs/OPENNT-MVDM-DECLARED-OUTPUT-TRIAGE.md`; and
- `docs/NT4-GUEST-ARTIFACT-CLOSURE.md`.
