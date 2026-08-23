# M0 T259 S4 P4 — Final Host/Machine Source-Shaped Reconciliation

Status: completed S4 part

## Question

After the catalog, RTL, copied-state, machine-disposition, and dialog passes,
does every *reached* historical host or machine seam retain an original
interface shape, a public-API composition behind that shape, or an explicit
source-proven decline—without turning `bx-core` or `bx-mantle` into an OpenNT
product shell?

## Final reconciliation

| Catalog family | Final disposition | Residual owner, if any |
| --- | --- | --- |
| `HOST-01` file/path | OpenNT DEM/COMMAND mirrors retain their file ordering; named OEM/Unicode and directory facades use public Win32. | raw/removable media remains machine-owned. |
| `HOST-02` raw media/device | Source-visible FDISK cleanup is retained; raw/floppy open and media success paths explicitly decline. | FDC/DMA/CMOS/media package. |
| `HOST-03` child/process | Imported COMMAND process ordering remains behind the session child facade and public `CreateProcess`/pipe endpoints. | redirector/WOW and multi-session broker packages. |
| `HOST-04` CSR/BaseSrv broker | No false broker is recreated; local session continuation is bounded and historical remote broker paths are deferred. | explicit product-shell package. |
| `HOST-05` console/std handles | Original notification and dialog forms are preserved through session state/public dialogs; parent standard handles are never replaced. | real input/PIF and console injection packages. |
| `HOST-06` environment/registry/PIF | Source-shaped environment and public host capability facades remain active. | PIF/WOW product branches stay deferred. |
| `HOST-07` NTDLL/RTL | Named facades retain counted-string, heap, status and device-link contracts where the public host can supply them. | no private `ntdll` replacement ABI. |
| `HOST-08` WOW/redirector loaders | Explicitly unavailable/deferred; no loader is fabricated. | WOW16/Redirector packages. |
| `HOST-09` VDM control | `NtVdmControl` returns `STATUS_NOT_IMPLEMENTED`, enabling the original slow fallback where the source provides one. | historical VDM-control product shell. |
| `HOST-10..12` host handles, guest pointers, CCPU frame | T258's one mapping-manager implementation and the copied-state CCPU facade are the only active crossings. | long-lived pointer consumers migrate package-by-package. |
| `MACH-01` CCPU simulation | Original reset/interrupt no-ops are preserved; recursive `host_simulate` is an explicit unavailable result. | no CCPU loop is recreated. |
| `MACH-02` SAS/UMB | Checked finite RAM/IVT/A20 facades remain; `UMBNotify` is direct source parity (empty original body). | real UMB reservation and device/media paths. |
| `MACH-03..05` reset/BDA/PIC/keyboard | Typed mechanical requests remain selector-blind and Bochs owns CPU/device mechanics. | native machine capability packages. |
| `MACH-06..09` VDD, dispatcher, BIOS, devices | Historical spellings remain at the mirror boundary; unavailable product shells are explicit, not reimplemented in Bochs. | VDD/debugger, BIOS and device packages. |

## Last source-shape correction

OpenNT `softpc.new/host/src/nt_msscs.c:HostTerminatePDB` calls
`FloppyTerminatePDB(PDB)` followed by `FdiskTerminatePDB(PDB)`.  The active
composition now retains the directly composable second call in exactly that
source position.  The first call is not silently dropped as a success: it is
documented as the still-unavailable FDC/DMA/CMOS owner.  Similarly,
`VDDTerminateUserHook` is explicitly deferred because its original body walks
the historical private VDD callback list.  Recreating that list would be a
new VDD product broker, not a compatibility facade.

`softpc.new/host/src/nt_umb.c:UMBNotify` itself has an empty body.  Its active
no-effect seam is therefore direct source parity, not an implicit claim that
UMB allocation or hardware setup succeeded.

## Static closure and focused proof

Fresh external run root: `build/M0-T259-S4/formal-r16/`.

- The generated graph contains 454 build nodes and declares 78 fixtures plus
  the native target.  Manifest target names exactly match produced names.
- `ninja -C build/M0-T259-S4/formal-r16 -n` reports no work.
- `t230-s5-demsrch-fcb-direct-import-fixture.exe` passes, including slow-reset
  search lifecycle, tokenized FCB operations and PDB cleanup.
- `t230-s7-demdasd-ioctl-direct-import-fixture.exe` passes its CCPU accessor,
  IOCTL boundary and raw-drive failure contracts.
- `t245-s11-config-complete-fixture.exe` retains its pre-existing r15/r16
  exit-2 ambient limitation; it is not used as passing evidence and no source
  in that path changed in this pass.

No OpenNT mirror, `bx-core`, or `bx-mantle` source was changed.  The two shim
edits are source-shaped host composition only.

## Conclusion

S4 meets its exit criterion.  Reached seams are now either source-shaped
facades/direct public composition or source-proven declines.  The remaining
CSRSS, VDD, WOW, Redirector, full UMB, floppy/device and long-lived mapping
semantics are named owner packages—not hidden fallback implementations.
