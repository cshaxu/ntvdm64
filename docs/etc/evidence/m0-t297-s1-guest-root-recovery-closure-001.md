# M0 T297 S1 — guest original-root recovery closure

## Result

The `mvdm-guest` component now has four explicit load-only roots:

- `dos/v86/`;
- `bin86/`;
- `wow16/`; and
- `font16/`.

The former `win16/` root is absent. This is a path-only recovery; it neither
enables a guest build nor changes host, adapter, firmware or runtime behavior.

## Provenance and verification

- The hash-verified canonical move manifest records 1,991 old-to-new files:
  [m0-t297-s1-guest-root-move-manifest.tsv](../operations/m0-t297-s1-guest-root-move-manifest.tsv).
- Ten former supplementary Win16 paths were duplicate copies of canonical
  WOW16 destinations. They were removed only after equal SHA-256 verification;
  the original path identities and target hashes are retained in
  [m0-t297-s1-guest-root-duplicate-elision-manifest.tsv](../operations/m0-t297-s1-guest-root-duplicate-elision-manifest.tsv).
- The Git-Bash Node 22 move tool re-ran after the relocation and reported
  `canonical guest files moved: 1991; verified duplicate elisions: 10`.
- A PowerShell verification recomputed every canonical and duplicate-target
  SHA-256: 1,991 canonical rows + 10 duplicate rows, zero mismatches.
- `src/mvdm-guest/win16` is absent; the root directory set is exactly
  `bin86`, `dos`, `font16`, `wow16` (plus the component README).
- `git diff --check` was run. Its diagnostics are confined to pre-existing
  whitespace/EOF formatting in newly admitted byte-identical historical
  WOW16 files; those bytes were deliberately not normalized. The same check
  excluding `src/mvdm-guest` passes for every T297-authored move, script and
  governance file.

## Boundary result

All four roots remain immutable data/load-only inputs. Their sources, objects,
libraries, fonts and products do not enter any host compile or link graph.
`src.old` was not read or used.
