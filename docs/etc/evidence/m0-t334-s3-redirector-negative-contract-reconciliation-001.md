# M0 T334 S3 — Redirector negative-contract reconciliation

## Reconciliation

The T334/S1 physical ledger was compared to the selected original providers.

- `VrGetDomainName`, `VrGetLogonServer`, `VrNetGetDCName` and
  `VrNetWkstaSetInfo` each retain an original direct
  `SET_ERROR(ERROR_NOT_SUPPORTED)` result. They gained no NetAPI, Xactsrv,
  Rx/RAP or SMB call.
- Message, server, service, use/macro and workstation paths retain their
  `XsNet*` transport forms. Password, remote API and transaction paths retain
  `Rx*`/`RxpTransactSmb` forms. They are not silently transformed into public
  local APIs.
- The selected original `ntstatus.c` remains the only NTSTATUS-to-NetAPI
  mapping owner. NetBIOS, DLC and VDD remain named device-owner transfers.

## Verification

`tools/governance/VerifyRedirectorNegativeContracts.mjs` read the selected
original `vrnetapi.c` and verified the four explicit unavailable provider
bodies retain their original result and no network-provider call. The existing
formal x86 and x64 Redirector graphs were then checked incrementally:
`original-mvdm-redir.lib` and `redirector-bindings.lib` required no work on
either architecture.

## Result

S3 closes the negative contract. This is source/contract evidence only; it
does not claim remote networking, RAP/SMB transport, NetBIOS/DLC/VDD behavior
or guest-network execution.
