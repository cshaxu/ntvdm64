# M0 T301 S1 P21 — OpenNT-src-2 MVDM supplement

## Question

Which original-relative MVDM paths supplied by
`O:\repos.external\opennt-src-2\nt\private\mvdm` are absent from the current
project `mvdm-*` mirrors, and can they be preserved without changing selected
source or runtime behavior?

## Inputs and procedure

- The supplemental tree is recorded locally at OpenNT-src-2 revision
  `5e4619ab61c2aa76151e03973cce340be2933e61`.
- The importer maps each MVDM path to its existing owner root: host, guest,
  support, tools or SoftPC-firmware; it never overwrites a target.
- Every source and target is SHA-256 checked. Existing non-identical targets
  are retained as their already-registered mirror divergences.

## Result

- 1,019 absent paths were copied byte-exactly: 605 to `mvdm-guest`, 369 to
  `mvdm-host`, 23 to `mvdm-softpc-firmware`, 17 to `mvdm-tools`, and 5 to
  `mvdm-support`.
- 842 of those paths are historical `.obj` products. The remaining carry
  contains historical link controls, libraries, images, debug products and
  five WOW16 assembly source paths.
- 3,632 paths were already byte-identical. Thirty-eight existing targets
  differ and were not overwritten; they remain registered source-mirror
  divergences.

## Interpretation and limits

This is an owner-approved mirror-completeness exception. It does not add an
OpenNT-src-2 source-definition root to the T301 zero/one-degree BFS, select a
third edition for a conflicting source file, alter the build graph, enable a
runtime provider or claim a clean historical whole-tree build.

The machine-readable [import manifest](../operations/m0-t301-s1-opennt-src-2-mvdm-extra-import-manifest.tsv)
is the authoritative per-file provenance, size and hash record.

## Verification

The post-copy manifest reports 1,019 `supplement-imported-verified` rows with
matching source and target SHA-256 values and no remaining missing target.
Documentation governance verification passes. `git diff --check` reports
trailing whitespace and final blank lines in byte-exact historical `.CL*`,
`.mac` and build-log inputs; they are upstream bytes and were intentionally
not normalized.
