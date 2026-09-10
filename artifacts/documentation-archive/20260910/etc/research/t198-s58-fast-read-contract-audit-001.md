# M0 T198 S58: DEM Fast-Read Contract Audit

## Result

The source-built NTIO witness that follows the closed snapshot-only
`50:21/AX=4408` route reaches the next BOP as `C4 C4 50 42` at
`9346:6351`.  The copied state is:

| Register or segment | Value |
| --- | --- |
| EAX | `00004001` |
| EBX | `00000000` |
| ECX | `0000000d` |
| EDX | `00000000` |
| DS:SI | `8d9b:0000` |
| ES:DI | `00a7:00f5` |

This is the DEM fast-read selector, not a BIOS, device, drive-discovery, or
host-filesystem request.

## Existing finite provider

`src/bx-vdm/bx_ntvdm_dem_fastio_provider_v1.c` is a narrow provider façade
over `bx_ntvdm_boot_namespace_provider_v1_fast_read`.  The latter retains the
existing read contract from `bx_ntvdm_dem_readonly_file_service.c`:

- validates the real-mode `C4 C4 50 42` form and copied event;
- uses the copied DEM handle, optional position, byte count, and `DS:DX`
  destination; and
- returns a bulk-result transaction plus the original-style typed CPU resume
  result.

Its data source is the already-materialized read-only boot namespace.  It
does not enumerate host drives, open host files, add a device, change a DOS
image, or depend on the detached historical runtime.  Its write is therefore
already representable by the active boot-namespace plane's existing checked
ordinary-RAM bulk-action executor.

## Admission consequence

S59 may compose this existing provider at the existing boot-namespace plane
boundary and rerun the complete source-built NTIO witness.  This is a finite
composition repair, not a new BOP implementation and not a Bochs change.

