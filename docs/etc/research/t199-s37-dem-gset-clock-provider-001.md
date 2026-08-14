# M0 T199 S37: DEM GSET clock provider

## Question

Can the four `demgset.c` clock BOPs be recovered as one bounded DEM GSET
subprovider without adding a Bochs, DOS-kernel, or trace-selected semantic?

## Source and ABI evidence

`src/opennt/base/mvdm/dos/dem/demdisp.c` maps `50:14`, `50:15`, `50:19`, and
`50:1C` to `demQueryDate`, `demQueryTime`, `demSetDate`, and `demSetTime` in
`demgset.c`.  The original query contracts return date in `AL/CX/DX` and time
in `CX/DX`; the setters report success or failure through `AL`.

The current CLI profile admits a one-time host drive snapshot but has no
authority to mutate the workstation clock.  Therefore the query operations
are a contained read capability and both setters take the source-shaped
failure result `AL=FF`, preserving `AH`.  All paths resume after the four-byte
`C4 C4 50 xx` BOP and are real-mode-only.

## Implementation

`src/bx-vdm/bx_ntvdm_dem_clock_service_v1.[ch]` is a GSET-local provider.
It reads the process local clock only for `15` and `16`; it does not inspect
guest memory or call a Bochs API.  `bx_ntvdm_dem_gset_plane_v1` owns its
selection before the existing original `50:0F` drive-count provider.

This is not an adapter implementation of DOS timekeeping: it is a contained
CLI capability adaptation of the original DEM host-service contract.  The
machine remains owner of CPU execution and time mechanics.

## Reproducible verification

`tests/bx-vdm/bx_ntvdm_boot_namespace_composition_v1_test.c` now verifies
the provider through common DEM session routing:

- `50:14` resumes, returns a valid weekday, a contemporary year, and nonzero
  month/day fields;
- `50:19` resumes with `AH` preserved and `AL=FF`, proving the no-host-mutation
  boundary.

The complete source-built composition executable is
`artifacts/build/t199-s37-dem-clock-r3/t198-s16-bx-vdm-boot-namespace.exe`;
it exits zero under MSVC x64 `/MT`.

## Boundary and follow-up

This closes only the clock portion of the `demgset.c` package.  It does not
close GSET or DEM as a whole: drive free-space, current/default drive,
media/label, DTA/DPB and their failure contracts remain in S37.  No native
trace was run or used to select this work.
