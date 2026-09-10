# T225 S8 Primary Source-Built Four-Image Cross-Validation Plan

## Question

Can the existing four-image installation fixture consume the *primary*,
original-toolchain output sequence independently from the already accepted
same-hash OpenNT-tree reference sequence?

## Required primary stages

| Role | Reproducible disposable stage | Required manifest | Artifact |
| --- | --- | --- | --- |
| NTIO | `build/M0-T225-S8/ntio-primary-source-build-r8` | `ntio-build-manifest.json` | `base/mvdm/dos/v86/doskrnl/bios/NTIO.SYS` |
| NTDOS | `build/M0-T225-S8/ntdos-primary-source-build-r1` | `ntdos-build-manifest.json` | `base/mvdm/dos/v86/doskrnl/dos/NTDOS.SYS` |
| COMMAND | `build/M0-T225-S8/command-primary-source-build-r6` | `command-build-manifest.json` | `base/mvdm/dos/v86/cmd/command/COMMAND.COM` |
| SHARE | `build/M0-T225-S8/share-primary-source-build-r1` | `share-build-manifest.json` | `base/mvdm/dos/v86/cmd/share/SHARE.EXE` |

The primary stager must require each stage and manifest before copying any byte.
It verifies the locked size/SHA-256 identities but does not treat hash equality
with `refs/opennt` or the secondary reference root as provenance equality.
The earlier availability record names unavailable `artifacts/toolchain-runs/`
roots. Those were an initial restoration hypothesis, not a permitted
long-lived build location. The managed source-build scripts supersede that
hypothesis by reproducibly creating the manifest-bearing inputs under the
disposable `build/M0-T225-S8/` hierarchy, consistent with the build-output
rules. The corresponding primary-source-build evidence records the toolchain
and exact identities.

## Procedure

1. Build the existing admitted fixture through the T225 S7 Ninja graph.
2. Stage the four primary sources into a fresh disposable `build/t225-s8/`
   input root, retaining a machine-readable origin record.
3. Run Direct and Readonly installation only. Overlay and Virtual must reject.
4. Compare these four outcomes with the existing secondary reference outcome,
   preserving the two origin sequences as separate facts.

## Boundary

No guest instruction trace is admitted. A missing root or manifest is a
source-artifact restoration result, not permission to use a reference file,
modify a guest image, implement a BOP, or alter Bochs mechanics.