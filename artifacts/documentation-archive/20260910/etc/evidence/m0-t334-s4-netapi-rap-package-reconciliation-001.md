# M0 T334 S4 — NetAPI/RAP Redirector package reconciliation

## Selected-source result

The complete T334 physical ledger contains eighteen reached NetAPI/RAP
Redirector edges.  Each now has one of three precise results:

- `VrGetComputerName`, `VrGetUserName(BX=1)`, and the local name-preparation
  portion of `VrRemoteApi` retain their original provider ordering and use
  only public, local Win32/NetAPI calls plus a bounded session lease.
- `VrGetDomainName`, `VrGetLogonServer`, `VrNetGetDCName`, and
  `VrNetWkstaSetInfo` retain their selected original
  `SET_ERROR(ERROR_NOT_SUPPORTED)` bodies.  They are not converted into
  invented local substitutes.
- RAP/SMB, XsNet, Rx, NetBIOS, DLC and VDD forms retain either their exact
  original unavailable result or their named later-owner transfer.

The only source divergences in the selected local cohort remain
`MVDM-HOST-DIV-171` (one bounded ANSI guest copy) and `MVDM-HOST-DIV-172`
(one bounded local OEM conversion before the unchanged RAP branch).  No
remote request is thereby enabled.

## Formal build verification

The fresh, disposable formal CCPU40 graphs were regenerated under
`build/M0-T334/S4/formal-x86` and `build/M0-T334/S4/formal-x64` and built with
their generated MSVC environment runners.

- x86 completed 147 Ninja actions for `original-softpc-candidate`.
- x64 completed 440 Ninja actions for the same target; a subsequent serial
  Ninja invocation reported `no work to do`.
- Both graphs selected the original Redirector source package and bindings.

The one build-graph repair is a source-neutral header-basename selection in
`New-T310OriginalSoftpcNinja.ps1`: SoftPC compilation resolves its original
`softpc.new/base/inc/config.h` before the unrelated OpenNT Net `config.h`,
while DPMI retains ordinary OpenNT include order.  No OpenNT body, BOP route,
or provider semantics changed.  Existing historical source diagnostics remain
visible; no warning was suppressed.

## Focused contract verification

- `VerifyRedirectorNegativeContracts.mjs` passed against the selected original
  `vrnetapi.c` unavailable-provider bodies.
- `redirector-guest-copy-fixture.exe` passed on both S2 x86 and x64 graphs,
  covering active-session bounded ANSI/OEM copies, capacity rejection, and
  post-close lease rejection.

## Closure limit

This closes source, ABI, selected public-local behavior, and formal build
selection for the T334 NetAPI/RAP boundary.  It does **not** claim an SMB/RAP
transaction, remote guest network operation, NetBIOS/DLC/VDD initialization,
or end-to-end guest networking.  Those capabilities remain exactly where the
ledger assigns them: unavailable or a later named owner package.
