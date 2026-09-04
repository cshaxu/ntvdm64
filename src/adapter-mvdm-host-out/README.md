# adapter-mvdm-host-out

This component owns only historical OpenNT-facing interface bindings required
by imported `mvdm-host` source. It is not a BOP provider, an alternate MVDM
implementation, or a general compatibility layer.

## Family ownership

- `win32/`: historical Win32, NTDLL and OEM source-facing call shapes over
  public modern Windows facilities.
- `softpc/`: historical SoftPC, CCPU and SAS call shapes over the selected
  original CCPU40/SAS owners and session-bounded services.
- `monitor/`: historical user-mode VDM monitor and command-record call shapes.
- `basesrv/`: reached BaseClient/BaseSrv VDM command protocol over copied
  session-local records; never a generic CSR or CSRSS substitute.
- `redir/`: historical VDMREDIR/Redirector product-interface call shapes.
- `wow/`: historical WOW32/WOWEXEC product-interface call shapes.
- `vdd/`: historical VDD registration and callback call shapes.
- `debugger/`: historical BDE/DBG/VDMDBG/VDMEXTS product-interface call shapes.

All implementation candidates must enter exactly one of these families through
the T284 S1 inventory. `adapter-mvdm-host-out` preserves original SoftPC
interface shapes and does not introduce an alternate machine facade.

## Current state

Families remain separately admitted. The first recovered capability is the
T302 local Base VDM protocol in `basesrv`; all other family implementation
requires its own source/ABI/failure audit and must not use this protocol as a
generic host-service shortcut.

## Registered divergences

- `ADAPTER-BASESRV-010` — `basesrv/source/base_vdm_local.c` is the bounded,
  one-session counterpart of the original BaseSrv copied-record service. It
  retains the original `VDMINFO` PIF/title/current-directory capture order,
  including the `PifFile`/`PifLen` payload needed by original SoftPC PIF
  selection, while replacing private CSRSS storage and wait handles with
  session-owned state. It neither parses PIF data nor exposes host data to the
  guest; original `GetPIFData` remains its parser/owner.
- `ADAPTER-BASESRV-011` — `basesrv/source/mvdm_image_classification.c` is the
  direct product-entry counterpart of the Windows loader's pre-VDM image
  selection. It recognizes one concrete DOS, Win16 or native image without
  parsing shell syntax. Native PE is launched by public `CreateProcessA`
  before any VDM exists; DOS proceeds through the unchanged BaseVDM record and
  first PermCom; Win16 is explicitly bootstrap-gated. No classification bit
  enters `VDMINFO`, guest memory or original `cmdmisc.c`.
