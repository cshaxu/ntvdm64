# T220 S3: bounded DEM CurrentPDB helper result

## Delivered surface

`src/bx-vdm/bx_ntvdm_dem_process_owner_v1.[ch]` adds a DEM-private copied
process-identity helper.  It reuses
`bx_ntvdm_dem_dta_registration_v1::current_pdb`, which the existing
source-derived `50:1B` registration records from the original
`demSetDTALocation` contract.

The helper can issue exactly one read of two bytes below the real-mode 1 MiB
aperture.  It returns only a status and a 16-bit PDB segment.  It contains no
BOP selector, CPU result, host handle, profile choice, guest pointer or
callback.  `PDB == 0`, an absent registration, aperture overflow, and any
completion whose range/count is not exact produce an explicit non-owner state.

## Source-first disposition

The direct historical translation unit remains non-composable because it uses
CCPU/SAS `Sim32GetVDMPointer`, flat guest pointers and the historical host/VDD
shell.  This helper is the rung-2 minimum adapter seam: it retains the exact
first `demfile.c` fallback fact—read `*pusCurrentPDB` as a word—without
claiming to reproduce the later PDB/JFT/SFT lookup.  No Bochs intrusion or new
host behavior was used.

## Verification

The focused source-built x64 `/MT` fixture was compiled directly with the
installed VS 2022 Build Tools compiler into the disposable root
`build/M0-T220-S3/001/`:

```text
cl /nologo /std:c11 /MT /I src/bx-vdm ... /Fe:build/M0-T220-S3/001/dem-process-owner-test.exe
bx-ntvdm DEM CurrentPDB owner helper: bounded copied identity verified
```

It covers valid little-endian decoding, absent registration, zero owner,
aperture overflow, wrong completion address and wrong completion byte count.
The adjacent CMake target is retained for a normal CMake environment.  This
host's VS-bundled CMake selected the generator but reported no C compiler even
though the same `VsDevCmd` environment exposes and successfully runs `cl.exe`;
that discovery defect is recorded as a toolchain limitation, not a passing
CMake result.

## Limitation and next boundary

The helper is intentionally unbound: it does not alter a DEM BOP route,
associate a file/search/FCB resource, or alter `50:3C`.  The next whole
package step must design the DEM session's **asynchronous owner-association
transaction**: when a direct resource is created it must request this exact
copied word, preserve existing provider failure behavior, and bind the
returned PDB only after the resource creation contract succeeds.  That step
must also decide how `50:3C` releases only resources whose copied owner equals
its `BX`, without substituting a global cleanup sweep.
