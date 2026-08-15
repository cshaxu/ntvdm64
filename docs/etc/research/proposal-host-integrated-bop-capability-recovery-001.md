# Proposal: Host-Integrated OpenNT BOP Capability Recovery

## Decision

NTDOS64 is non-invasive because it is unpack-and-run: it does not require a
rebuilt or modified Windows installation, replacement system files or kernel
APIs, drivers/services/VDD installation, or installation-style Registry
mutation. This does **not** prohibit ordinary user-mode Win32, device,
filesystem, path, disk, port, console or Registry APIs under explicit CLI
policy and ordinary host permissions.

The default BOP host-service profile is **direct host integration**: recover
the original OpenNT owner package and retain its normal host API behavior
wherever it can be independently composed. `bx-vdm` supplies only the minimum
ABI/platform seam when historical CCPU/SAS/VDD/WOW/GUI glue blocks direct
composition. `nt_bop.c` remains dispatch/semantic evidence, not a direct
modern executable composition.

## Storage Profiles

| Profile | Proposed CLI selection | Semantics |
| --- | --- | --- |
| Direct host | default / `--storage-mode=direct` | Policy-admitted DOS drives map to real host drives; original OpenNT file, directory, handle, volume and cwd behavior calls selected Win32 capabilities. |
| Read-only host view | `--storage-mode=readonly` | Real drives remain readable/searchable; the complete mutation family is refused with source-derived DOS failures. |
| Volatile overlay | `--storage-mode=volatile-overlay` | Reads see session-memory overlay before host base; file, directory, metadata, rename and deletion changes persist only until process exit. It is a namespace/handle backend, not a write trap. |
| Virtual boot volume | `--virtual-boot-volume=<letter>` | A requested guest-RAM-only startup volume may fill a letter excluded from host projection. It never silently replaces an admitted host drive or writes boot files to the host. |

`--include-drives` and `--exclude-drives` retain their real-host admission
meaning and exclusion wins. A virtual boot volume is a general virtual volume
with an explicit boot role, not a hard-coded virtual C: feature.

## Owner-Package Method

Each BOP family/service is audited for original caller, dispatcher, source,
data layout, host API, state owner and failure path. It then has one
disposition: directly composable original provider; original provider plus CLI
platform seam; source-derived rehost; explicit optional-profile backend; or
deferred/unavailable. Only after a complete owner package is implemented and
regressed may a bounded native trace be used as integration observation.

## Existing-Code Rule

`readonly_namespace`, boot-namespace provider/plane/composition and profile
search/session/transaction are virtual boot-volume or deterministic-fixture
candidates, not default host semantics. The readonly failure providers are
read-only-profile candidates. `host_namespace`, host-drive policy and
host-volume snapshot are direct-host foundations. Synthetic-root cwd services,
broad CLI-unavailable fallbacks and duplicate root-handle holders must be
classified as retain, migrate, replace or delete; code with no practical
profile or fixture value is removed.

## Proposed S Sequence

1. Full BOP/provider/workaround audit and retain/migrate/replace/delete ledger.
2. Shared CLI storage-profile and drive-view ABI.
3. DEM direct-host namespace/directory/cwd/volume package.
4. DEM direct-host file/handle/FCB/search/read-write package.
5. COMMAND direct-host console/environment/cwd/bootstrap/launch package.
6. Complete read-only profile.
7. Volatile overlay profile.
8. Virtual boot-volume profile.
9. Remaining-family reclassification, obsolete-code deletion, package
   regressions and bounded native integration traces.

For each package record source path, host API mapping, copied ABI, policy,
positive/negative tests and prior-workaround disposition. Direct mode proves
ordinary permitted host mutation; read-only proves no mutation; overlay proves
teardown loss; virtual boot proves guest-RAM-only materialization and no host
boot-file write.
