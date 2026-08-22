# M0 T236 S4 P4 — DEM DASD/IOCTL Disposition Closure

## Question

After recovering the source-shaped fixed-volume `nt_fdisk` seam, does every
reachable branch of the imported OpenNT DEM `50:21`, `50:29`, and `50:2A`
package have one explicit owner and failure disposition without placing
machine behaviour in `bx-vdm`?

## Inputs

- `src/opennt/base/mvdm/dos/dem/demioctl.c`
- `src/opennt/base/mvdm/dos/dem/demdasd.c`
- `src/opennt/base/mvdm/softpc.new/host/src/nt_fdisk.c`
- `src/bx-vdm/bop/shim/demdasd_ioctl_shim.c`
- P1--P3 evidence, including the authorised disposable-VHDX lifecycle
  witness in [P3](t236-s4-p3-authorized-vhdx-raw-volume-success-001.md)

## Disposition

| Entry / original branch | Disposition | Basis and boundary |
| --- | --- | --- |
| `50:21`, `AL=00..07,0A..0C,0E..10`, or `AL>=12` | Original source failure | `demIoctlInvalid` returns `ERROR_INVALID_FUNCTION` and `CF=1`. The historical `DBG`-only table-range guard is repeated at the copied bx-vdm boundary for `AL>=12`, preventing an out-of-table release build access while preserving that original failure body. |
| `50:21`, `AL=08/09` | Direct original source | `demIoctlChangeable` retains the original `demGetPhysicalDriveType`/`GetDriveTypeOem` result and DOS register contract. |
| `50:21`, `AL=0D`, fixed BDS `SETDPM`, `GETDPM`, `READTRACK`, `WRITETRACK`, `VERIFYTRACK`, access state | Original source plus public-Win32 fixed-volume seam | `demIoctlDiskGeneric` and `demdasd.c` remain the owner. Recovered `nt_fdisk` preserves BDS, geometry, raw read/write/verify, close and source error conversion with documented public volume handles. |
| `50:21`, `AL=0D`, fixed BDS `FORMATTRACK` | Direct original source | OpenNT's non-removable branch calls `demDasdVerify`; it is not a formatter. P3 proves the underlying `IOCTL_DISK_VERIFY` path. |
| `50:21`, `AL=0D`, fixed BDS `SETMEDIA` | Direct original source, destructive capability not exercised | The original branch reads and rewrites sector zero through `nt_fdisk_*`. It remains source-owned and permission-controlled; no destructive test or adapter policy was admitted in S4. |
| `50:21`, `AL=0D/11`, no BDS or unsupported generic code | Original source failure/declaration | `demIoctlDiskGeneric` and `demIoctlDiskQuery` retain their `DOS_FILE_NOT_FOUND`, `DOS_INVALID_FUNCTION`, and capability-query contracts. A query is not an admission of a successful media operation. |
| `50:21`, any removable-media branch | Explicit unavailable / machine owner | `nt_rflop` and FDC/DMA/CMOS/timer mechanics are not composed. The retained `nt_floppy_*` seam returns a real unsupported host error; no success is fabricated. |
| `50:29`, no BDS | Original source failure | `demAbsReadWrite` returns `DOS_DRIVE_NOT_READY`, before a raw-device call. |
| `50:2A`, no BDS | Original source failure | Same original `DOS_DRIVE_NOT_READY` path. |
| `50:29/2A`, fixed BDS | Original source plus public-Win32 fixed-volume seam | `demAbsReadWrite` retains offset/sector/range/error ordering and calls recovered `nt_fdisk_read`/`nt_fdisk_write`. P3 proves init/read/verify/write/readback/exact-restore/close on the named disposable medium. |
| `50:29/2A`, floppy with the historical INT 13 vector | Explicit unavailable / machine owner | Original source reaches `nt_floppy_*`, which reports failure until the bx machine/BIOS package owns a device implementation. |
| `50:29/2A`, floppy with a hooked INT 13 vector | Machine-owner transfer | Original `demBiosDiskIoRW` requires recursive SoftPC `host_simulate`. The seam returns `ERROR_CALL_NOT_IMPLEMENTED`, `AH=BIOS_INVALID_FUNCTION`, and `CF=1`; a future bx-core/bx-mantle bridge may supply only opaque mechanical execution. |

## Verification

A fresh formal MSVC x64 `/MT` Ninja graph was generated in the disposable
root `build/M0-T236-S4/p4-disposition-r1`. The focused imported-source fixture
was rebuilt and run without opening a volume:

```text
T230 S7 direct OpenNT DASD/IOCTL import: IOCTL, boundary and raw-drive failure contracts verified
```

The fixture covers `AL=08` success, the `AL=12` guarded source-failure
contract, and both no-BDS absolute-operation failures. P3 remains the positive
fixed-volume backend witness; this P4 run intentionally did not access the
VHDX or any disk.

## Interpretation

S4 is closed. The only new code is the smallest release-boundary validation
needed to select the already-imported `demIoctlInvalid` body for malformed
`AL`; it neither recognizes a BOP selector nor changes an OpenNT IOCTL
operation. All other semantics remain in the mirrored source or the
documented public-Win32 `nt_fdisk` compatibility seam.

Floppy/FDC/DMA/CMOS, INT 13 recursion, and `host_simulate` are not deferred
bugs inside this provider. They are transferred to the queued bx
machine/BIOS owner package. Destructive fixed-media operations are retained
as original direct capabilities, but no destructive success claim is made.

## Follow-up

- T236 S5 next audits COMMAND WOW helpers.
- The bx machine/BIOS package must separately admit any floppy, INT 13 or
  device-controller closure; it must not move those semantics into bx-vdm.
- Any future destructive-media test needs a separate named medium,
  restoration procedure, and explicit user approval.
