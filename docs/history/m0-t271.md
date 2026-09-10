# m0-t271 historical record

Consolidated task record. Raw contemporaneous records are retained in the external documentation archive.

## m0-t271-closure-20260825.md

# M0 T271 Closure — OpenNT BOP source-recovery and mirror-boundary audit

## Closure basis

The owner revised T271's exit criterion and directed immediate closure. This
record closes the completed recovery/audit package; it does not state that
every `opennt-bop` residual is a finished mirror implementation.

## Delivered

- S1 removed the project-authored `opennt-bop/ingress` tree and classified its
  owner routes instead of moving selector logic into selector-blind
  `adapter-bop`.
- S2 recovered the admitted DEM entry and original 73-slot dispatcher path.
- S3 recovered the admitted COMMAND entry, original 17-slot dispatcher and
  its source-shaped host/adaptation seams.
- S4 recovered the admitted XMS entry, original 12-slot dispatcher and the
  bounded XMS/guest-memory composition path.
- S5 audited the whole live BOP root, archived dormant DPMI inputs, moved the
  private COMMAND, XMS and DEM source bodies behind original-name mirror
  boundaries, and finally moved the two DEM compatibility headers without
  cross-component consumers to `opennt-bop-overlay`.

## Verification

- P16 commit `b03e23d2` rebuilt the formal MSVC x64 `/MT` Ninja DEM fixture
  matrix. The DASD/IOCTL, DEM miscellaneous, DEM dispatcher and native DEM
  session fixtures each returned zero.
- Documentation governance and `git diff --check` passed for P16; the pushed
  GitHub revision is `b03e23d2`.
- The retained whole-root audit is
  [S5 P10](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t271-s5-p10-opennt-bop-mirror-reaudit-and-disposition-001.md).

## Explicitly deferred residuals

- Five non-OpenNT headers remain at the BOP root because current COMMAND,
  XMS and VDMREDIR/app consumers still require their cross-component API
  recovery: COMMAND composition/PIF, XMS compatibility, and VDMREDIR
  dispatch/mailslot compatibility.
- `app/bop/route.c` and `app/bop/redir_native_session.c` still contain BOP
  selector/service composition that needs an owner-package decision rather
  than an unreviewed relocation.
- Existing focused DEM fixtures directly include private overlay declarations.
  An attempted replacement with the native `MS_bop_0` entry showed that those
  fixtures depend on a distinct scoped-state contract. This requires a
  separately admitted test-boundary design; it is not silently accepted as a
  conforming overlay boundary.

These residuals are recorded in [TODO](../states/TODO.md). Any follow-on task must
use the BOP tracker and the S5 P10 owner ledger; it may not claim T271's
unfinished interface recovery as already complete.


## m0-t271-s1-closure-20260825.md

# M0 T271 S1 closure — non-mirror BOP composition extraction

S1 established the correct entry distinction: `adapter-bop` mechanically
enters `opennt-bop`, but does not interpret an OpenNT selector/service.

- Moved eight default-off observation pairs out of `opennt-bop`: seven to
  `app/observation`, and the DEM file-create producer observation to
  `adapter-win32/facade`.
- Moved app-owned image/launch/machine-stage setup from
  `opennt-bop/ingress` to `app/startup_composition`. COMMAND's temporary
  session now receives copied, admitted launch values from app; no
  `opennt-bop` production source includes `app/`.
- Moved the keyboard handoff display-state storage to the same-shaped
  `adapter-softpc` facade. The remaining BOP bridge retains only its
  selector-level entry/order.
- Recorded every remaining source-derived bridge, composition, compatibility
  seam and private overlay in
  `docs/etc/evidence/m0-t271-s1-nonmirror-owner-ledger-001.md`, with its
  original owner/recovery packet or explicit deferred owner.

Verification: fresh external formal Ninja graph `build/M0-T271-S1/r003`
produced `bin/ntvdm64.exe` and reported no work on a final dry run. The
affected keyboard handoff and all five moved-observation fixtures passed.
Documentation governance, production-tree gate and `git diff --check` passed.

This does not claim that the remaining project-authored selector bridges are
mirrors. They are frozen temporary boundaries; S2 replaces the DEM portion
through the original DEM dispatcher/provider structure.


## m0-t271-s2-closure-20260825.md

# M0 T271 S2 closure — OpenNT DEM original dispatcher/provider recovery

S2 replaced the active project-authored DEM service ingress with the reached
OpenNT host BOP entry and its original dispatcher ordering.

- Imported the true `MS_bop_0` source subset from
  `base/mvdm/softpc.new/host/src/nt_bop.c`. Its own order obtains the staged
  service byte through `Sim32GetVDMPointer`, calls `DemDispatch`, advances IP
  and applies the historical idle exclusion.
- Retained the original 73-slot `demdisp.c:apfnSVC` table as the normal DEM
  owner. `dem_native_session` no longer selects ordinary services or calls
  `DemDispatch` itself.
- Added only the same-shaped, bounded CCPU/SAS guest-span facade and the
  no-result `HostIdleNoActivity` SoftPC facade. Both are locally marked and
  registered as BOP-DIV-097.
- Retired the former project DEM ingress and its direct fixture to indexed
  legacy evidence. The active runtime-session fixture now creates a valid
  minimal machine stage before exercising the source-shaped guest-memory
  facade.
- Retained the owner-approved BOP-DIV-098 exception: reached guest `50:42`
  enters the imported `demRead` body because its visible original table slot
  is a no-op and no original fast worker is available. `50:43` remains
  original unavailable.

Verification outside the sandbox: refreshed formal Ninja graph
`build/M0-T271-S2/r001`, full Ninja build and final dry-run both succeeded.
The original-entry, 73-slot dispatcher, `demhndl`/fast-read, `demfile`,
`demdir`, `demsrch/fcb`, `demgset`, `demerror/lock`, `demmisc`, checked-RAM,
direct-host and runtime-session fixtures exit zero. The retained
`t230-s7-demdasd-ioctl-direct-import-fixture.exe` times out in a host-volume
exercise; it is an explicitly recorded DASD/device owner limitation and is
not counted as passing dispatcher evidence. Documentation governance,
production-tree gate and `git diff --check` pass.

S2 does not claim guest continuous execution, a real FDC/DMA/INT13 lifecycle,
or a resolved host-volume DASD regression. Those remain in their tracker owner
rows. T271 now proceeds to S3, COMMAND original package recovery.


## m0-t271-s3-closure-20260825.md

# M0 T271 S3 closure — OpenNT COMMAND original dispatcher/provider recovery

S3 restores the admitted OpenNT COMMAND package as the active source owner.

- `MS_bop_4` is the retained true subset of OpenNT `nt_bop.c`; it loads the
  service byte and enters the original `cmddisp.c` table.
- `cmd.c`, `cmddata.c`, `cmddisp.c` and reached `cmd*.c` providers are active
  OpenNT mirror source, with every modern boundary registered in the component
  divergence records.
- Same-shaped CCPU/SAS, VDM API, public Win32/OEM, child lifecycle and host
  event dependencies are now owned by `adapter-softpc`, `adapter-win32` and
  `opennt-host`, respectively. App-owned startup/session composition no
  longer remains under `opennt-bop/ingress`.
- The PIF parser remains an exact COMMAND owner-family source due its shared
  global state and incompatible historical parser/caller `PIF_DATA` layouts;
  its modern binding is private overlay code.
- The final focused formal matrix passed, including true native `54:08`
  pending/resume execution through the Bochs opaque callback route.

S3 does not claim guest-wide execution continuity or completion of
Redirector, WOW, VDD/debugger, real console input, DOS EXEC/PSP parent return
or multi-VDM NT4 product services. Those remain explicitly transferred to
their named owner packages and the T271/S5 component audit.

T271 now proceeds to S4, XMS original package recovery.


## m0-t271-s4-closure-20260825.md

# M0 T271 S4 closure — OpenNT XMS original package recovery

## Delivered

- The active XMS service route invokes the original `xmsdisp.c` 12-slot
  `apfnXMSSvc` table and original provider-family bodies.
- Project-authored package configuration/call lifecycle is isolated behind
  the private `BOP-DIV-107` overlay boundary, rather than residing in the
  mirror root.
- The reached A20/HIMEM guest-byte path uses the sole session `guest_memory`
  mapping-manager instance (`SOFTPC-DIV-006`), with bounded leases and no
  persisted native pointer.
- The non-original XMS bind/reset lifecycle now belongs to app composition.
- `xmsmemr.c` and `i386/xmsmem86.c` are source-audited: neither is directly
  admissible on modern x64; the bounded same-shaped SoftPC callbacks remain
  the required recovery seam.

## Verification

Formal MSVC x64 `/MT` Ninja roots `r001` and `r002` built the XMS closure.
Both `t237-s2-xms-source-mirror-fixture.exe` and
`t237-s3-xms-a20-source-mirror-fixture.exe` returned zero after the final
ownership move. Documentation governance, production-tree gate and
`git diff --check` passed before each recorded commit.

## Scope boundary

This is source/package code completion and local validation, not a claim that
the default native guest boot reaches every XMS call or that a physical UMB
success span has been enabled. Those remain machine/profile integration work.

## Evidence

- [P1 composition isolation](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t271-s4-p1-xms-composition-overlay-isolation-001.md)
- [P2 mapping migration](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t271-s4-p2-xms-a20-mapping-manager-migration-001.md)
- [P3 source-unit audit](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t271-s4-p3-xms-source-unit-admission-audit-001.md)
- [P4 app lifecycle extraction](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t271-s4-p4-xms-app-session-binding-extraction-001.md)

