# adapter-mvdm-host-out

This component owns only historical OpenNT-facing interface bindings required
by imported `mvdm-host` source. It is not a BOP provider, an alternate MVDM
implementation, or a general compatibility layer.

## Family ownership

- `win32/`: historical Win32, NTDLL and OEM source-facing call shapes over
  public modern Windows facilities.
- `softpc/`: historical SoftPC, CCPU and SAS call shapes over typed
  `adapter-bochs` mechanics and session-bounded services.
- `monitor/`: historical user-mode VDM monitor and command-record call shapes.
- `basesrv/`: reached BaseClient/BaseSrv VDM command protocol over copied
  session-local records; never a generic CSR or CSRSS substitute.
- `redir/`: historical VDMREDIR/Redirector product-interface call shapes.
- `wow/`: historical WOW32/WOWEXEC product-interface call shapes.
- `vdd/`: historical VDD registration and callback call shapes.
- `debugger/`: historical BDE/DBG/VDMDBG/VDMEXTS product-interface call shapes.

All implementation candidates must enter exactly one of these families through
the T284 S1 inventory. `adapter-mvdm-host-out` never calls `bochs-core`
directly; its `softpc` family reaches machine state only through the typed
public operations of `adapter-bochs`.

## Current state

Families remain separately admitted. The first recovered capability is the
T302 local Base VDM protocol in `basesrv`; all other family implementation
requires its own source/ABI/failure audit and must not use this protocol as a
generic host-service shortcut.

## Registered divergences

None. This root contains no recovered implementation at admission.
