# M0 T260 S4 app and guest migration

## Result

T260 S4 completes the source-layout move for the product entry and confirmed
guest inputs without changing their implementation behavior:

| Original root | Target component root | Result |
| --- | --- | --- |
| `src/cli` | `src/app` | 24 implementation files are history-preserving Git renames; the obsolete component README was merged into `app/README.md` and removed. |
| `src/opennt/base/mvdm/dos/v86` | `src/opennt-guest/dos-v86` | 577 original DOS V86 input files are 100% Git renames. |
| `src/opennt/base/mvdm/wow16` | `src/opennt-guest/wow16` | 2 original WOW16 ABI/include input files are 100% Git renames. |

The guest source text is unchanged.  Prebuilt OpenNT guest artifacts remain
the default packaging input; the moved trees remain source-build and
reproducibility input only.

## Owner-map correction

The former broad `dos/* => guest` classifier was wrong: `dos/dem` and
`dos/command` are host-side OpenNT BOP/provider sources, not guest image
content.  The live generator now classifies them to `opennt-bop` for S5.  The
refreshed manifest proves:

- 580 `guest-source-candidate` rows, all owned by `opennt-guest`;
- zero DEM/COMMAND rows in that guest set; and
- 38 `src/opennt/base/mvdm/dos/{dem,command}` rows owned by `opennt-bop`.

## Deferred work

Historical evidence documents retain their former paths as source-identity
citations.  Formal Ninja source lists, fixture directories and all remaining
legacy path references are deliberately switched once in S8.  S4 therefore
makes no build-success or guest-runtime claim.
