# adapter-mvdm-host-out

T412 S2 registers ADAPTER-WIN32-059: original BaseCheckVDM's standard stream
fields are refreshed from GetStdHandle when the private PEB carrier is queried;
WindowFlags comes from GetStartupInfoW. No native PEB layout is assumed and
these local handles are not broker wire values. Product selection is pending;
focused source-owned launch tests and support compilation are in S2 evidence.

T412 S2 registers ADAPTER-WIN32-058: the existing private support PEB now
supplies ImageBaseAddress from GetModuleHandleW(NULL) for original BaseClient
GetBinaryTypeW's native machine check. This adds one process-local field and
one public-API assignment, not a modern PEB cast or guest projection. The
classifier fixture verifies native x86 image acceptance and DLL rejection;
its production support translation unit is compile-checked. Product classifier
selection remains pending. See the
[S2 evidence](../../docs/etc/evidence/m0-t412-s2-original-lifecycle.md).

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
  selection. It resolves one token in DOS `.COM`/`.EXE`/`.BAT` order first
  beside the product, then through the ordinary current-directory/PATH search,
  and recognizes a resolved DOS, Win16 or native image without parsing shell
  syntax. Native PE is launched by public `CreateProcessA` before any VDM
  exists; DOS proceeds through the unchanged BaseVDM record and first PermCom;
  Win16 is explicitly bootstrap-gated. An unresolvable token remains a public
  `COMSPEC /c` host-shell request. No classification bit enters `VDMINFO`,
  guest memory or original `cmdmisc.c`.
