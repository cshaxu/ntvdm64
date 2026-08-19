# T230 S7 — OpenNT DASD / IOCTL Direct-Import Map

## Question

Can the original OpenNT DEM `50:21`, `50:29` and `50:2A` owner package be
introduced as source-shaped code without reviving the former v1 provider
plane or placing SoftPC device semantics in `bx-vdm`?

## Inputs

- `src/opennt/base/mvdm/dos/dem/demdasd.c`
- `src/opennt/base/mvdm/dos/dem/demioctl.c`
- `src/opennt/base/mvdm/dos/dem/demdasd.h`
- formal manifest `tools/build/t225-s7-full-module-manifest.json`

## Imported shape

The three input files are directly mirrored under
`src/bx-vdm/bop/opennt/dem/`.  The imported bodies retain their original
functions, global BDS list, IOCTL table, packed DOS structures, register
order and DOS error conversions.  The mirror changes only its include surface
and the `DPB.Next` host-pointer field: that field is retained as a 32-bit DOS
guest far-link, matching the pre-existing x86/x64-safe DPB seam.

`src/bx-vdm/bop/shim/demdasd_ioctl_shim.c` owns only unavailable historical
composition mechanics: CCPU/SAS register access, bounded IVT reads, the
former GUI diagnostic, the SoftPC recursive INT 13 path, and the raw-device
module ABI.  It contains no selector recognition or BOP service policy.

## Observations

The formal MSVC x64 `/MT` Ninja graph source-builds the complete `bx-vdm`
static library with both imported translation units.  The focused fixture
`t230-s7-demdasd-ioctl-direct-import-fixture` then links and exits zero.

It proves these source-owned local contracts:

| Entry | Original function | Observed contract |
| --- | --- | --- |
| `50:21` | `demIOCTL` → `demIoctlChangeable` | Host `C:` reports non-removable media (`AX=1`, `CF=0`). |
| `50:29` | `demAbsRead` → `demAbsReadWrite` | No registered BDS returns original `DOS_DRIVE_NOT_READY` (`CF=1`). |
| `50:2A` | `demAbsWrite` → `demAbsReadWrite` | No registered BDS returns original `DOS_DRIVE_NOT_READY` (`CF=1`). |

The fixture intentionally does not perform raw disk I/O.

## Raw-device dependency disposition

The source definitions for `nt_fdisk_*` live in
`src/opennt/base/mvdm/softpc.new/host/src/nt_fdisk.c`; the `nt_floppy_*`
definitions live in `nt_rflop.c`.  They are full historical SoftPC host-device
components, coupled to configuration/reset/UI and, for floppy, FDC/DMA/CMOS/
timer mechanisms.  They are not valid bx-vdm shims.  The present shim therefore
returns a genuine unsupported host error on those still-uncomposed calls, so
the imported DEM code performs its original error conversion; it never
pretends a raw transfer succeeded.

Their future recovery belongs to a separately admitted OpenNT host-capability
package with its own source/ABI/failure map.  That package may replace this
temporary ABI endpoint but may not migrate device mechanics into bx-vdm.

## Verification

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\build\New-T225S7FullNinjaGraph.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\build\M0-T230-S7\direct-import-r3
ninja -C O:\repos.hobby\ntdos64\build\M0-T230-S7\direct-import-r3 `
  bin/t230-s7-demdasd-ioctl-direct-import-fixture.exe
& O:\repos.hobby\ntdos64\build\M0-T230-S7\direct-import-r3\bin\t230-s7-demdasd-ioctl-direct-import-fixture.exe
```

Observed output:

```text
T230 S7 direct OpenNT DASD/IOCTL import: IOCTL query and raw-drive failure contracts verified
```
