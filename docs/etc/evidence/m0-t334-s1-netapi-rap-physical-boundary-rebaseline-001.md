# M0 T334 S1 — NetAPI/RAP Redirector physical boundary rebaseline

## Inputs

- M0 T333/S5 `m0-t333-s5-redirector-network-assignment-disposition-ledger.tsv`.
- Original selected mirrors: `mvdm-host/vdmredir/vrnetapi.c`,
  `mvdm-host/vdmredir/vrremote.c`, `mvdm-host/dos/command/cmdredir.c`, and
  `mvdm-host/inc/vdmredir.h`.
- The byte-identical selected OpenNT status-map mirror
  `opennt-host/netapi/netlib/ntstatus.c`.

## Procedure

1. Located every direct NetAPI, Xactsrv, Rx/RAP or SMB transport call in the
   selected `vrnetapi.c` and `vrremote.c` physical sources.
2. Read each original caller's result, allocation/free pairing, descriptor and
   guest-buffer form.
3. Classified the first viable recovery rung, without treating an available
   modern API as permission to substitute a private RAP protocol.
4. Recorded the resulting eighteen physical edges in
   `m0-t334-s1-netapi-rap-physical-edge-ledger.tsv`.

## Observations

- Four public capability cohorts remain bounded candidates: session-key
  preparation, username, logon-server data, and local computer-name fallback.
  They use `NetWkstaGetInfo`, `NetWkstaUserGetInfo` and matching
  `NetApiBufferFree`; they must retain original conversion and checked guest
  copy behavior.
- `VrGetUserName` already reaches the selected public carrier through the
  existing synchronous lease divergence (`MVDM-HOST-DIV-171`). It is not a
  new provider. S2 must prove its capacity-negative and positive contracts.
- `NetpNtStatusToApiStatus` is a directly composable selected original
  `ntstatus.c` mirror. An adapter error map would duplicate and weaken the
  original contract.
- Every remaining reached remote, message, use/macro, server, service and
  transaction branch is driven by Xactsrv, Rx/RAP or `RxpTransactSmb` data
  formats. A public NetAPI call cannot be presumed layout-equivalent. These
  branches retain exact original unavailable outcomes and named protocol-owner
  transfers.

## Result and S2 admission boundary

S1 closes its physical-boundary objective. S2 may exercise only the four
bounded public cohorts and their original NetAPI allocation, character
conversion, status and checked guest-copy contracts. It may not enable a
RAP/SMB transaction, a remote guest request, NetBIOS/DLC/VDD behavior or a
replacement protocol provider.

## Verification

- The ledger is a physical source review, not runtime network evidence.
- Documentation governance and `git diff --check` are required before the
  S1 delivery.
