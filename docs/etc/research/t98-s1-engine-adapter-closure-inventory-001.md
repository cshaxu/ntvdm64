# T98 S1 Engine/Adapter Closure Inventory 001

## Retained engine identity

The only selected runtime artifact is
`artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r5/ntdos64-s7-runtime-trace.exe`:

| Property | Value |
| --- | --- |
| SHA-256 | `F4416F20F456366CC1AFC83D8B40748BF331D02E336BB9350EE8045329582DC2` |
| build recipe | `nmake /f ntdos64-s7-runtime-trace.mak ntdos64-s7-runtime-trace.exe` |
| Bochs replacements | only `main.cc` and `cpu/exception.cc`, as pinned by its r5 manifest |
| Bochs mode | retained 2.6 source root; r4 CPU archive intact; no current full configure/build admission |

The r5 makefile proves this is a direct MSVC object/link composition, not an
opaque external executable. Its map contains `bx_ntvdm_adapter_runtime` and
the old v3 profile parser, but contains none of the current pathname symbols.

## Current-source delta

The current adapter closure adds these required objects to r5's `ADAPTER_OBJS`:

- profile snapshot: `bx_ntvdm_profile_search_snapshot_v1`;
- source-shaped pathname transaction: search session/result/plan/request/
  transaction plus `bx_ntvdm_dem_path_search_service_v1`;
- common route proof: BOP ingress, provider registry, DEM plane, COMMAND plane
  and legacy plane gate;
- updated `bx_ntvdm_adapter_runtime` and `byob_profile` (v4 declaration and
  dispatch lifecycle).

The current r5 map has no `bx_ntvdm_dem_path_search`,
`bx_ntvdm_profile_search`, `bx_ntvdm_legacy_plane_gate`, or
`bx_ntvdm_bop_ingress` entries. The v4 live-engine rejection is therefore
explained by an exact object-link absence rather than a newly inferred Bochs
or guest defect.

## Required/default/rejected classification

| Class | Item |
| --- | --- |
| Required for next narrow build | rebuild only current CLI/adapter objects, retain r5 Bochs archives and its two documented Bochs replacement objects, then relink the existing `ntdos64-s7-runtime-trace` target. |
| Default but not required | all r5 device/plugin archives already named by the retained link recipe; no changed device source/object is selected by T98 S1. |
| Explicitly rejected | full Bochs configure/build, archive repacking, device/plugin enablement, BIOS/firmware changes, and any new Bochs intrusion or bridge ABI. |

## Next admission boundary

The only candidate build action is a copied, hash-recorded r5 derivative with
the listed current objects compiled by the recipe's MSVC island and the same
link order. It must first prove that no unchanged Bochs object or archive is
rebuilt and that the map gains only the intended adapter/profile symbols.
No build was run for this inventory.
