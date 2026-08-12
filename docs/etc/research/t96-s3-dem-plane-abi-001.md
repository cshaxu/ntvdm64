# M0 T96 S3 Unified DEM Plane ABI 001

## Decision

`src/bx-ntvdm-adapter/bx_ntvdm_dem_plane_v1.{h,c}` is the single adapter-local
DEM plane classifier. It receives only the common BOP ingress record and its
provider-registry selection, then returns a fixed-width record containing the
DEM service number, its original component group, and one disposition.

It deliberately has no callback, historical dispatcher call, guest-memory
address, host pointer, result transition, or Bochs API. It cannot turn a
deferred operation into success. Runtime provider migration stays S8.

## Complete classification

All 73 callable `demdisp.c` services are classified from their original
definition module:

| Component | Services | Plane disposition |
| --- | ---: | --- |
| Namespace (`demfile`/`demhndl`/`demdir`/`demsrch`) | 24 | deferred |
| FCB (`demfcb`) | 8 | deferred |
| Drive/system (`demgset`) | 13 | deferred |
| Raw media (`demioctl`, `demdasd`) | 3 | deferred |
| Misc (`demmisc`) | 13 | deferred |
| Error/lock (`demerror`, `demlock`) | 3 | deferred |
| V86 bridge (`demdisp`) | 1 | deferred |
| Original `demNotYetImplemented` | 8 | original no-op |

The original `SVC_DEMLASTSVC` boundary is excluded: common ingress marks it
not callable, and the plane rejects it. The grouping matches the completed
component-boundary and raw-media/IOCTL evidence; it is not an implementation
priority order.

## Verification

Focused Clang C11 compilation with `-Wall -Wextra -Werror` linked the existing
fixed-width exception, CPU-state/result, instruction-window, ingress, and
registry ABIs with the new plane and its test. The test constructs every DEM
service 0 through 72 through common ingress/selection, proves each has a
nonzero expected original component and its correct deferred/no-op
disposition, and proves service 73 cannot be classified.

The default MinGW build probe remains unavailable because its MSYS2 runtime
cannot create temporary files in `C:\\msys64\\tmp`; that limitation is not
treated as a passing integration build.
