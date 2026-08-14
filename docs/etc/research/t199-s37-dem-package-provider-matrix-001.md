# M0 T199 S37 DEM Package Provider Matrix

## Question

How must the 73 callable DEM BOP services be recovered as complete original
source components, without treating a trace hit or a common adapter stop as a
substitute for an individual DEM contract?

## Authority and inputs

The sole dispatch authority is
`src/opennt/base/mvdm/dos/dem/demdisp.c`.  Its `apfnSVC` table maps
`50:00..48`; `50:49` is the non-callable `SVC_DEMLASTSVC` sentinel.  Handler
implementation source remains below `src/opennt/base/mvdm/dos/dem/`.

The current package is `src/bx-vdm/bx_ntvdm_dem_package_session_v1.c` plus
its existing bounded subproviders.  It proves unified routing, not full DEM
host capability.

## Original component packages

| Original source owner | Services | Current rehost boundary | S37 completion rule |
| --- | --- | --- | --- |
| `demhndl.c`, `demfile.c`, `demdir.c` | `00-06, 08-09, 0B, 12-13, 16-18, 1E, 22-23, 27, 31, 44, 47-48` | Handle/path/filesystem operations. Existing immutable namespace provides selected open/read/close/seek flows only. | One readonly namespace session with every mutation and write path returning its source-derived DOS failure, never pass-through or ambient host mutation. |
| `demsrch.c`, `demfcb.c` | `07, 0A, 0C, 20, 2C-31` | DTA/FCB layout and search continuation. Pathname/DTA and the bounded FCB first/next profile search are present; FCB open/create/I/O remains unavailable. | Recover pathname and FCB as distinct subproviders, including continuation and invalid-layout failure behavior. |
| `demgset.c`, `demlabel.c` | `0D-10, 14-15, 19, 1A-1C, 25, 41, 46` | Drive snapshot, DPB, DTA, clock, current/default drive and computer name. | One immutable CLI drive/state provider; date/time and writable state use declared deterministic policy or source-proven failure. |
| `demdasd.c`, `demioctl.c` | `21, 29, 2A` | Raw media/DASD and IOCTL. No Bochs device/DASD ownership crosses to bx-vdm. Metadata-only IOCTL 08/09 is admitted; remaining IOCTL now returns AX=1/CF and absent-BDS absolute read/write return AX=21/CF. | Explicit per-operation failure contract, except admitted metadata-only IOCTL routes; no synthetic disk device. |
| `demerror.c`, `demlock.c` | `32, 33, 3F` | Hard-error registration/retry/lock model. | One copied registration state plus source-shaped unavailable/clear behavior for absent host lock/critical-error facility. |
| `demmisc.c`, `demmsg.c`, `demdisp.c` | `11, 1D, 23, 34-3E, 45` | DOS load/app state, debugger-callout placeholder, symbols, stream I/O, debugger and VDM lifecycle. | Retain DOS image load/symbol/debug bounded contracts; `50:1D` is the original ordinary return, not a V86 capability; define contained terminal/console policy without moving host-process paths to mantle. |
| `demNotYetImplemented` in `demdisp.c` | `1F, 24, 26, 28, 2B, 40, 42-43` | Original common implementation. | CF clear plus four-byte continuation exactly. |
| Fast I/O compatibility exception | `42` | Existing source-derived readonly fast-read, although original table selects `demNotYetImplemented`. | Keep it separately registered and fully regress both original-table reasoning and compatibility behavior. |

## Required provider precedence

For every group, selection order remains: independently composable original
OpenNT code; the same provider with only a declared contained CLI capability;
minimal source-derived rehost after an evidence-recorded historical blocker;
then the individual original failure/unavailable behavior.  `DemDispatch` is
not a direct link target because it combines CCPU/SAS register access and the
historical host composition.

## Deliberate exclusions

- No generic DEM `STOP` replaces per-service CF/AX/memory contracts.
- No FCB, raw-media, V86, device, DOS-kernel, or Bochs implementation enters
  bx-vdm merely to eliminate a deferred route.
- No trace determines ordering.  Trace follows only the complete S37 package
  regression.

## Next implementation order

1. Readonly namespace/search/FCB package, including all mutation and invalid
   request failures.
2. Immutable GSET/drive/state package.
3. Error/lock and miscellaneous lifecycle package.
4. Raw-media/V86 terminal dispositions and all-family regression.

This order follows original source ownership and dependency direction, not
service-number reachability.
