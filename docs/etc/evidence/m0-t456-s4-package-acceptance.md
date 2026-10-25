# M0 T456 S4 — package acceptance and closure boundary

## Sealed product identity

S2's production delivery is commit `48ab4e32a`. It rebuilt and published only
`WOW32.DLL`; the remaining nine release images were deliberately normalized to
the prior coherent release before publication. The full component runner at
`build/t456-s2/full-build-cim-r2/result.json` passed both x86/x64 configured
closures and reported 1,493 unchanged intermediates on its second build.

S4 independently re-read `assets/release/release-manifest.json` as the
top-level array it is, then verified all ten image hashes in the manifest,
`assets/release` and `O:\winnt\system32`. The valid output is
`build/t456-s4/release-identity-r3.json`: `RELEASE_IDENTITY_OK count=10`.
The earlier null-field and incorrect `assets/release/system32` probes are
explicitly invalid harness attempts and are not evidence.

## Repeated focused operations

The independently owned runner `build/t456-s4/repeat-r2` executed eight
complete x86 rounds without an intermediate cache reset. Every round rebuilt
and ran both checked-in fixtures:

1. `WOW_GLYPH_OUTLINE_IGNORE_ROTATION_OK` — the selected public seam still
   matches an unrotated font and restores caller selection.
2. `WOW_METAFILE_PUBLIC_HOST_OK` — classic metafile create, bits, restore,
   copy/reopen, bitmap playback, enumeration and invalid-input handling pass.

Its result is `WOW_S4_REPEAT_OK rounds=8`. This is a repeated resource and
release check for the two finite W08 host boundaries, not a claim that it
exercises every guest application feature.

## Application frontier classification

The published WRITE private-desktop startup/liveness witness from S2 remains
valid for the exact provider hash. Attempts to extend it with the existing
physical-stage application harness are recorded, not hidden:

- WINMINE's gameplay probe observed the owned worker then exited `0xffffffff`
  before a window could be enumerated (probe return 4).
- WRITE's four-sample frontier exited `0x0000042b` before its first sample.

The staged guest executables and WOW32 provider match the deployed hashes.
The same private-desktop early-exit condition was already documented for
unchanged packages in T423 S30. These are non-passes of that test environment,
not successes and not attributed regressions of this font/metafile delivery.
No test result is used to erase the retained SOL geometry or WRITE full-
editing/normal-exit limitations.

## T456 closure

The finite W08 obligation is fully accounted for: source-selected font/text
and metafile owners were audited, the one source-proven glyph rotation gap was
repaired and published, host capability/release behavior was repeated, and no
second font/metafile authority or guest change was introduced. Broader dialog,
input, hook and timer behavior transfers to the next queue candidate W06.
