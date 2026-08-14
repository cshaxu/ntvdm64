# T200 S29: excluded-drive owner map

## Question

Does an explicitly excluded host `C:` drive mean that the profile-owned guest
boot namespace must also lose its `C:` binding?

## Result

No. The two records have different owners and meanings:

| Record | Owner | Meaning when host `C:` is excluded |
| --- | --- | --- |
| `byob_profile_selection.command_placement.drive_index` and the readonly boot namespace | BYOB profile / adapter guest-artifact composition | Still `2` (`C:`): `COMMAND.COM`, boot files and the declared target are contained guest artifacts, not a host-drive projection. |
| `bx_ntvdm_host_drive_snapshot_v1.admitted_mask` | CLI host-capability policy | May be zero: it describes only host logical drives admitted for projection. |

The intended separation is already stated by `docs/design/HOST-NAMESPACE-CAPABILITY.md`:
drive filtering is distinct from BYOB guest-artifact admission, and the BYOB
artifact root is not an implicit host namespace root. The source-derived GSET
record likewise assigns `50:0D` to the profile-selected boot drive, not to
registry or ambient host-drive discovery.

## Current coupling

The direct composition captures the filtered host snapshot in
`bx_ntvdm_composition_runtime_v1.c` and installs it into DEM GSET. COMMAND's
`bx_ntvdm_cmd_get_next_v1_complete` then uses that same host snapshot for the
CMDINFO drive-count field while independently using the readonly namespace's
profile drive for the executable path and drive-index field. The current DEM
current-directory family also tests its drive input against that host snapshot.

This is an ownership-model defect: one snapshot is serving both the optional
host-projection plane and a guest boot-volume/session plane. It does **not**
prove that the previous bounded native timeout was caused by a particular BOP,
COMMAND field, or Bochs defect. No trace was rerun.

## Bounded evidence

`tests/bx-vdm/bx_ntvdm_cmd_get_next_service_test.c` builds a valid host
snapshot with present `C:` but include/exclude both selecting `C:`. Its
admitted mask is zero. The focused `54:01` transaction still publishes the
profile namespace drive index `2` and publishes drive count `0`.

The source-built fixture ran as:

```text
powershell.exe -ExecutionPolicy Bypass -File tools\Invoke-T198S16BootNamespaceVdmX64Probe.ps1 \
  -Fixture command-entry \
  -BuildRoot artifacts\build\t200-s29-command-host-drive-separation-r1
```

It passed as an MSVC x64 `/MT` C closure. It does not execute a guest CPU
loop, expose a host path, or add a BOP provider.

## Repair admission

S30 owns the package repair. It must introduce an explicit profile-owned guest
boot-volume/session record and keep the host-projection snapshot solely for
capabilities that actually project host drives. It may not paper over the
distinction by silently forcing host `C:` back into the admitted mask, changing
CLI exclusion semantics, or adding Bochs logic.
