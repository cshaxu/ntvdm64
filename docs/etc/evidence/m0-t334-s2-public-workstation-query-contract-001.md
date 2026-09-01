# M0 T334 S2 — public workstation-query contract verification

## Selected original source

The selected mirror already contains the smallest established source-shaped
bindings. S2 introduces no provider body and does not expand a BOP route.

- `VrGetComputerName` retains its `LM20_CNLEN` check and AX/CF sequence; only
  its old direct guest alias is replaced by one bounded ANSI lease copy
  (`MVDM-HOST-DIV-171`, `ADAPTER-REDIR-003`).
- `VrGetUserName(BX=1)` retains the original `CX` capacity gate,
  `NetWkstaUserGetInfo` and `NetApiBufferFree` ordering. The selected public
  Unicode carrier is converted to the original OEM result encoding before the
  one bounded guest copy. `BX=0` remains unadmitted because the source
  declares no finite destination span.
- `VrRemoteApi` retains its original local computer-name fallback and frees
  its `WKSTA_INFO_100` before its later RAP branch. Its one required Unicode
  conversion is `MVDM-HOST-DIV-172`; it never claims the following RAP/SMB
  transaction succeeded.

`VrGetLogonServer` was re-read in the selected original source and corrected
in the S1 ledger: it is directly source-shaped `ERROR_NOT_SUPPORTED`, not a
public workstation-query candidate.

## Verification

`tools/build/GenerateRedirectorWorkstationQueryNinja.mjs` generated the
disposable `build/M0-T334/S2/{x86,x64}` graphs. Each graph compiled selected
original `vrnetapi.c` and `vrremote.c`, compiled the existing Redirector lease
adapter, linked and executed `redirector-guest-copy-fixture.exe` successfully.
The fixture verifies: no session rejects a copy; an active bound session
commits bounded ANSI and OEM-converted Unicode values; an out-of-range write
is rejected; and a closed guest-memory lifetime rejects a later write.

The formal T333 Redirector source graphs were also rechecked for both
architectures: `original-mvdm-redir.lib` and `redirector-bindings.lib` were
incrementally clean. Historical diagnostics in unselected RAP/SMB paths remain
visible; none were suppressed or converted into a success claim.

## Limit

This verifies a host-side, local workstation-query contract only. It does not
prove a remote request, a RAP/SMB transaction, NetBIOS/DLC/VDD service or
guest-network execution.
