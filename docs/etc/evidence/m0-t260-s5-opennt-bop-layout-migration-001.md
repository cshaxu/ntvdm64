# M0 T260 S5 — OpenNT BOP layout migration

## Decision

`opennt-bop` is now the only product component which owns OpenNT BOP source
identity, minimal-change provider mirrors, and selector/service route records.
This is a layout/provenance packet only: no BOP behavior, provider disposition,
Bochs mechanic, source expression, or formal build graph was changed.

## Content-preserving migration

| Destination role | Count | Inputs moved with `git mv` |
| --- | ---: | --- |
| `src/opennt-bop/original/` | 152 | OpenNT DEM, COMMAND, XMS, DPMI/DPMI32, BOP headers, `nt_bop.c`, and SoftPC BIOS inputs. |
| `src/opennt-bop/mirror/` | 59 | Existing minimal-change DEM/COMMAND/XMS/DPMI/SoftPC mirrors and import manifests. |
| `src/opennt-bop/route/` | 37 | Existing BOP ingress/provider routes and the BIOS-selector map. |
| `src/opennt-bop/overlay/` | 4 | Existing `nt_bop`, DEM-file, DEM-handle and DPMI-memory overlays. |

The rename-detection gate reports 252 moves and zero non-`R100` moved files.
The count includes source headers and source/build metadata because provenance
is part of the original package boundary.

## Boundary retained for later packets

`src/bx-vdm/bop/` has no top-level BOP route or mirror file after this packet.
Its remaining `shim/` subtree is deliberately unclassified implementation work
for S6 (`opennt-host` / `adapter-win32`) and S7 (`adapter-softpc`); its
`observation/` subtree is S7 mechanical observation work.  Neither is BOP
selector or provider ownership, so neither moved here.

The formal Ninja module manifest still declares several historical source
paths.  The live owner manifest resolves their content-preserving S3--S5
migration locations while retaining an explicit reason that formal graph path
replacement remains S8 work.  Therefore this packet makes no build-pass or
runtime claim.

## Overlay register

The four inherited overlays are registered as `BOP-OVL-001` through
`BOP-OVL-004` in [`src/opennt-bop/README.md`](../../../src/opennt-bop/README.md).
T260/S5 did not alter those source bodies; subsequent owner packages must
preserve their source identity and cite the corresponding entry before
changing, replacing, or retiring one.

## Verification

1. `Export-T260S1OwnerManifest.ps1` regenerated the live owner/provenance
   record without actual top-level BOP files remaining in `src/bx-vdm/bop`.
2. The source-role scan found 152 original, 59 mirror, 37 route and 4 overlay
   files under `src/opennt-bop`.
3. `git diff --name-status --find-renames HEAD` found 252 BOP-layout moves,
   all `R100`.
4. Documentation inventory/governance and `git diff --check` are required
   before packet closure.
