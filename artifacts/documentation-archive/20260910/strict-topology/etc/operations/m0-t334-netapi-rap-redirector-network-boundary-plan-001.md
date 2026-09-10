# M0 T334 — NetAPI/RAP Redirector network-boundary recovery

## Intent

T334 continues the named network/RAP transfers from M0 T333. It recovers the
original OpenNT Redirector's reached network-management contracts as a bounded
owner package. It does not create an SMB, RAP, NetBIOS, DLC, RPC, CSRSS, or
kernel substitute.

## Owner source and fixed boundary

- `mvdm-host/vdmredir/vrnetapi.c` and `vrremote.c` retain dispatch and
  contract ownership.
- `mvdm-host/dos/command/cmdredir.c` and `mvdm-host/inc/vdmredir.h` provide
  the original caller and declaration forms.
- `opennt-host/netapi/netlib/ntstatus.c` remains the selected original status
  conversion mirror; adapters do not reimplement its maps.
- Public modern NetAPI calls, if admitted by S2, sit behind the existing
  named Redirector/Win32 adapter boundaries and copy values through the
  session-owned mapping/lease contract.
- `XsNet*`, `Rx*`, `RxRemoteApi`, NetBIOS, DLC, VDD, private RPC, CSRSS and
  kernel facilities retain exact unavailable or named successor-owner
  outcomes. No local equivalence is invented.

## Subtask sequence

1. **S1 — physical boundary rebaseline.** Enumerate every reached original
   external call, declaration, buffer/handle lifetime and failure rule. Make
   the recovery-rung decision before source/build changes.
2. **S2 — bounded public binding recovery.** Compose only the S1-admitted
   public NetAPI subset through same-shaped adapters; preserve original body,
   status conversion, allocation/free pairing and guest-copy checks.
3. **S3 — RAP/remote negative-contract closure.** Exercise original,
   source-shaped unavailable branches and record exact transfers to NetBIOS,
   DLC and VDD; do not manufacture remote success.
4. **S4 — package reconciliation.** Rebuild both formal CCPU40 architecture
   graphs; run focused host-side contract fixtures; review every selected
   source input, divergence and transfer.

## T334 completion standard

Every reached source interface has one auditable ownership/disposition result.
Selected source is directly composed or linked through the smallest same-shaped
adapter. The x86 and x64 formal graphs pass for the selected closure, local
positive and negative binding behavior is proven, and any remote outcome is
described only as the original source-shaped failure or a named later owner
package. This is not a claim of end-to-end guest network execution.
