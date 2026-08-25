# M0 T271 S2 — DEM original-entry recovery

## Question

Can the active `50:xx` route enter the original OpenNT DEM dispatcher through
its original host BOP entry, rather than through a project-defined service
selector?

## Inputs

- OpenNT `base/mvdm/softpc.new/host/src/nt_bop.c`, `MS_bop_0`.
- OpenNT `base/mvdm/dos/dem/demdisp.c`, `DemDispatch` and `apfnSVC`.
- Current mirrors `src/opennt-bop/dem/{nt_bop_dem.c,demdisp.c}` and DEM
  provider bodies.
- Formal graph `build/M0-T271-S2/r001`.

## Recovery

- Imported the `MS_bop_0` source subset into `dem/nt_bop_dem.c`.  It retains
  the original order: `Sim32GetVDMPointer(SEGOFF(getCS(),getIP()), 1, FALSE)`,
  `DemDispatch`, `setIP(+1)`, then the original date/time idle exclusion.
- The source subset includes only the CCPU/SAS declarations it reaches.  It
  deliberately does not include the broad DEM DASD compatibility graph,
  because that graph rebinds `getCS/getIP` for a separate recursive BIOS
  operation and would change this source body's CS:IP meaning.
- The same-shaped bounded `Sim32GetVDMPointer` facade leases the requested
  synchronous guest span. `HostIdleNoActivity` is a same-shaped no-result
  adapter entry; no unavailable timer worker is fabricated.
- `dem_native_session` now stages its copied real-mode CPU image at the byte
  after `C4 C4 50`, calls `MS_bop_0`, and returns the typed four-byte resume.
  It does not select a DEM provider or invoke `DemDispatch` itself.
- Retired the former project `dem_ingress_shim` and its fixture to
  `docs/etc/legacy_code/opennt-bop/t271-s2-replaced-dem-ingress/`.

## Exception

`50:42` remains BOP-DIV-098. The reached OpenNT guest
`doskrnl/dos/handle.asm:FastOrSlow` emits `SVC_DEMFASTREAD`, but the visible
OpenNT `demdisp.c` slot is `demNotYetImplemented`; no original fast worker is
available in the admitted source. The exact service continues through the
existing imported `demRead` body seam. `50:43` remains the original terminal.

## Verification

On 2026-08-25, outside the sandbox:

- refreshed formal graph `build/M0-T271-S2/r001` and ran `ninja`; it linked
  all declared libraries, fixtures and `bin/ntvdm64.exe`;
- final `ninja -n` reported `no work to do`;
- `t230-s10-dem-native-session-direct-fixture.exe` passed, proving copied
  `C4 C4 50 1f` enters imported `MS_bop_0`, reads its staged CS:IP service
  byte and reaches original `DemDispatch`;
- `t230-s2-demhndl-direct-import-fixture.exe` passed, including BOP-DIV-098
  fast read; and
- `t230-s10-demdisp-direct-import-fixture.exe` passed, proving all 73 table
  slots and original ordinary/no-op entries.
- The remaining direct DEM, checked-RAM, direct-host and runtime-session
  fixtures passed after the runtime-session fixture was corrected to compose
  a valid minimal machine stage before asking the source-shaped SAS facade to
  lease guest RAM.  `t230-s7-demdasd-ioctl-direct-import-fixture.exe` remains
  a bounded-timeout host-volume regression: it does not exercise `MS_bop_0`
  and is not counted as dispatcher success.  Its DASD/device-capability owner
  disposition remains separately tracked in `BOP-DEPENDENCY-015` through
  `BOP-DEPENDENCY-017`.

## Result

The active ordinary DEM route no longer contains a project-authored `50:xx`
service dispatcher. Original OpenNT entry and table ordering own every normal
service; BOP-DIV-098 is the sole declared exact-service exception.
