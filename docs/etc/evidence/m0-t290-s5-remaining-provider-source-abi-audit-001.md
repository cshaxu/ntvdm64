# M0 T290 S5 — remaining Redirector provider source/ABI audit

## Original owner groups

- `vrnetapi.c`: local computer/user/workstation names and historic Lanman
  use/service/transaction APIs.  The simple name bodies retain public
  `GetComputerName`/`NetWksta*` dependencies but also retain unbounded VDM
  aliases.  Their bounded guest-span conversion is a prerequisite.  Most
  `XsNet*`, `Rx*`, `Netp*` and remoting worker calls are private NT Lanman
  product interfaces, not modern public Win32 APIs.
- `vrremote.c`: historical remote-API transaction conversion and
  `RxpTransactSmb`.  It requires old server-side XactSrv/SMB transaction
  contract and multiple persistent 16:16 conversion buffers.  There is no
  public same-shaped replacement; this group must retain an explicit source
  unavailable disposition unless a separately owned Redirector protocol
  package is admitted.
- `vrnetb.c`: NetBIOS NCB submission, post queue and NetBIOS/DLC callbacks.
  It depends on `Netbios`, DLC headers, raw VDM address conversion and network
  line IRQ completion.  No public modern equivalent retains this contract;
  keep source-defined unavailable rather than creating a network emulator.
- `vrdlc5c.c`, `vrdlcpst.c`, `vrdlcbuf.c`: DLCAPI/LLC adapter ownership,
  private VDM window, event worker and physical IRQ callbacks.  `DLCAPI.DLL`
  and its driver contract are unavailable; this entire group remains an
  explicit source-proven unavailable provider group.
- `vrinit.c` NetBIOS/DLC completion branches and `vrdll.c` DLL lifecycle
  remain dependent on the same unavailable provider/IRQ groups.  No partial
  initialization success is permitted.

## Admissible S5 subset

`VrGetComputerName`, `VrGetDomainName`, `VrGetLogonServer`, `VrGetUserName`,
`VrNetGetDCName`, `VrNetWkstaSetInfo`, `VrReturnAssignMode` and
`VrSetAssignMode` are the only small local entry bodies worth individual
source composition review. The latter five already retain original no-op or
`ERROR_NOT_SUPPORTED` results; they must be regression-proven, not expanded.
`VrGetComputerName` is now locally proven through its original bounded
`LM20_CNLEN` output rule. Only the bounded `BX=1` `VrGetUserName` form still
needs a source-shaped public Unicode-to-OEM/ANSI conversion binding; its
unbounded `BX=0` form does not have an admissible output-span proof.

The original-object fixture proved a declaration-closure precondition:
`vrnetapi.c` includes `apinums.h`, `remdef.h`, `remtypes.h`, `rxp.h`,
`apiparam.h`, `xstypes.h`, `xsprocs.h`, `netlibnt.h`, `rxuser.h` and `crypt.h`.
T290 S5 P2 resolved that input at package scope: the exact 95-file OpenNT DS
declaration package plus its required exact OpenNT public-internal base and
network declaration cohorts are now in `mvdm-platform-abi`. The original body
compiles on both host widths. Its many private Lanman/SMB/SoftPC imports remain
unresolved for the unselected sibling bodies; the focused local-result fixture
uses a test-only forced-unresolved link and calls only the four original
`ERROR_NOT_SUPPORTED` bodies and two original no-op bodies. This is not a
product-link claim and does not provide a substitute for the private services.

## Frozen decision

S5 will not attempt to recover a private Lanman transaction server, NetBIOS,
DLC driver, VDD hook, physical IRQ, or a new network policy.  It first proves
the small local original result bodies and then records all remaining groups
as explicit source-owned unavailable outcomes for the S6 whole-family matrix.
