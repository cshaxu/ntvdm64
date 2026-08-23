# opennt-bop

Minimal-change mirrors of original OpenNT BOP providers, selector/service
routing and source-owned provider control flow. Preserve original names,
parameters, structures, ordering and observable failures.

It may call `opennt-host`, `adapter-win32` and `adapter-softpc` through their
declared interfaces; it must not touch Bochs objects, `bx-mantle` or `bx-core`
directly.

## Local intrusion register

Every edit to an imported BOP mirror is registered here with original path and
identity, exception identifier, necessity and focused verification. Each edited
source expression additionally carries a `DIVERGENCE:` comment naming the
replacement and reason. The future static library name is `opennt-bop`; its
public include surface is the BOP ingress/provider ABI consumed by `app`.

## Source layout

| Directory | Contents | Modification rule |
| --- | --- | --- |
| `original/` | Immutable OpenNT DEM, COMMAND, XMS, DPMI, BOP and SoftPC BIOS source inputs. | Never edit for product adaptation. |
| `mirror/` | Minimal-change compilable mirrors and their import manifests. | Retain original structure; each divergence needs a `DIVERGENCE:` comment and a register entry. |
| `route/` | Project-owned selector ingress, service routing and composition records. | May adapt component boundaries, but never reimplement an imported provider without a recorded source-derived exception. |
| `overlay/` | Pre-existing project overlays to an original source. | Preserve as distinct evidence; do not fold an overlay into `original/`. |

## Inherited overlay exception register

T260/S5 made no source-body edit: it moved these pre-existing overlays with
their original counterparts so that their divergence is visible and auditable.
The line counts below are the current `git diff --no-index --numstat` against
the paired `original/` source.

| ID | Original source | Overlay | Divergence | T260 disposition |
| --- | --- | --- | --- | --- |
| `BOP-OVL-001` | `original/nt_bop.c` | `overlay/nt_bop.c` | 13 added / 13 removed lines | Retain as inherited historical-host dispatcher adaptation; later provider work must cite this ID. |
| `BOP-OVL-002` | `original/dem/demfile.c` | `overlay/dem/demfile.c` | 11 added / 22 removed lines | Retain as inherited DEM file-provider adaptation; later DEM work must cite this ID. |
| `BOP-OVL-003` | `original/dem/demhndl.c` | `overlay/dem/demhndl.c` | 1 added / 1 removed line | Retain as inherited DEM-handle adaptation; later handle work must cite this ID. |
| `BOP-OVL-004` | `original/dpmi32/i386/dpmimem.c` | `overlay/dpmi32/i386/dpmimem.c` | 1 added / 1 removed line | Retain as inherited DPMI memory adaptation; later DPMI work must cite this ID. |

The migration evidence is
[`m0-t260-s5-opennt-bop-layout-migration-001.md`](../../docs/etc/evidence/m0-t260-s5-opennt-bop-layout-migration-001.md).
