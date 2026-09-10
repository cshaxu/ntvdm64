# T225 S19 DEM `50:25` DPB failure source map 001

## Question

Which error must `demGetDPB` return when a CLI snapshot cannot supply a
requested DPB, and is that outcome the same for an unadmitted drive and an
admitted drive lacking a usable volume record?

## Original source contract

- `src/opennt/base/mvdm/dos/dem/demgset.c:691-711` calls `demGetDpbI` and,
  on its `FAILURE` result, calls `demClientError(INVALID_HANDLE_VALUE,
  Drive + 'A')`.
- `src/opennt/base/mvdm/dos/dem/demerror.c:129-134` preserves a supplied
  Win32 last error but substitutes `ERROR_ACCESS_DENIED` when it is zero.
- `demGetDpbI` returns `FAILURE` when `demGetDiskFreeSpace` cannot describe
  the drive; its success path alone writes the DPB.

The CLI snapshot has no ambient Win32 operation from which to borrow a last
error.  Its failure mapping must therefore distinguish the two facts that the
original composition would otherwise obtain from the host:

| Snapshot condition | Retained failure |
| --- | --- |
| Drive index is outside `A:` through `Z:`, or not admitted by profile policy | `ERROR_INVALID_DRIVE` (15): the request names no CLI-visible drive. |
| Drive is admitted but has no usable volume record | `demClientError` zero-last-error fallback, `ERROR_ACCESS_DENIED` (5). |

## Recovery decision

The original translation unit cannot compose into the modern x64 CLI because
it depends on CCPU/SAS guest pointers and historical host composition.  The
existing `bx_ntvdm_dem_full_dpb_service_v1` is its smallest source-derived
adapter seam.  P5 changes only that seam's failure split; it adds no BOP
recognizer, host mutation, Bochs change, or new volume capability.

## Verification

The formal Ninja target
`t225-s19-dem-package-lifecycle-fixture` rebuilt only the changed DPB object,
`bx-vdm.lib`, and its executable.  Its full `50:00..48` family invocation
exited zero in both Direct and Readonly modes.  The companion result records
the exact commands and retains P4's focused lifecycle result separately.